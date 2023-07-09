#include "response_calc.h"

#include "static_container.h"
#include "comm_http.h"
#include "rga_utility.h"

ResponseCalc::ResponseCalc(const QJsonObject& data, QObject* obj): m_objData(data), m_pObj(obj) {
}

ResponseCalc::~ResponseCalc() {
//    qDebug() << __FUNCTION__;
}

void ResponseCalc::set_json_obj(const QJsonObject& data) {
    m_objData = data;
}

void ResponseCalc::run() {
    CommHttp::RespType resp_type = CommHttp::RespType::ResRespNone;
    QVariantMap vm_data;
//    vm_data.insert("rga_id", s_id);
    QString s_origin = m_objData.value("origin").toString();
    //am in controll
    if(s_origin.indexOf("amInControl") > -1) {
        resp_type = CommHttp::AmInControl;
        vm_data.insert("data", m_objData.value("data").toBool());
    }
    //scanSetup
    if(s_origin.indexOf("scanSetup") > -1) {
        if(s_origin.indexOf("scanTimeTotal") > -1) { //scan time total
            resp_type = CommHttp::ScanTimeTotal;
            vm_data.insert("data", m_objData.value("data").toDouble());
        } else {
            QJsonObject obj_data = m_objData.value("data").toObject();
            resp_type = CommHttp::ScanStatus;
            bool b_started = false;
            bool b_stopped = false;
            if(obj_data.contains("scanStart")) {
                b_started = obj_data.value("scanStart").toInt();
            }
            if(obj_data.contains("scanStop")) {
                b_stopped = obj_data.value("scanStop").toString() == "EndOfScan";
            }
            vm_data.insert("data", b_started || !b_stopped);
        }
    }
    //em gain val
    if(s_origin.indexOf("emGain") > -1) {
        resp_type = CommHttp::EmGain;
        vm_data.insert("data", m_objData.value("data").toDouble());
    }
    //fc sensitivity val
    if(s_origin.indexOf("ppSensitivityFactor") > -1) {
        resp_type = CommHttp::PpSensitivityFactor;
        vm_data.insert("data", m_objData.value("data").toDouble());
    }
    //system status
    if(s_origin.indexOf("systemStatus") > -1) {
        resp_type = CommHttp::SystemStatus;
        vm_data.insert("data", m_objData.value("data").toDouble());
    }
    //sensor settings
    if(s_origin.indexOf("sensorIonSource") > -1) {
        QJsonObject obj = m_objData.value("data").toObject();
        //filament selected
        if(obj.contains("filamentSelected")) {
            resp_type = CommHttp::FilamentSelected;
            vm_data.insert("data", obj.value("filamentSelected").toDouble());
        }
        if(obj.contains("tPunits")) {
            resp_type = CommHttp::FilamentSelected;
            vm_data.insert("data", obj.value("tPunits").toDouble());
        }
    }
    //scan data
    if(s_origin.indexOf("scans") > -1) {
        auto* inst = StaticContainer::getCrntRga();
        if(inst == nullptr) {
            return;
        }
        QJsonObject jo_data_temp = {};
        jo_data_temp = m_objData.value("data").toObject();
        QJsonObject jo_scan = {};
        jo_scan = m_objData.value("data").toObject();
        if(jo_data_temp.value("scannum").toInt() > inst->get_scan_num()) {
            QJsonArray ja_values = jo_scan.value("values").toArray();
            ja_values.pop_front();
            QJsonArray ja_d_values;
            int i_valCnt = ja_values.count();
            for (int var = 0; var < i_valCnt; ++var) {
                ja_d_values.append(fabs(ja_values[var].toDouble()));
            }
            jo_data_temp.insert("d_values", ja_d_values);
            resp_type = CommHttp::ScanData;
            vm_data.insert("data", jo_data_temp);
        }
    }
    //issue log
    if(s_origin.indexOf("errorLog") > -1 || s_origin.indexOf("issueLog") > -1) {
        QJsonObject jo_data = m_objData.value("data").toObject();
        resp_type = CommHttp::ErrorLog;
        jo_data.insert("data", jo_data.value("errors").toArray());
    }
    //serial number
    if(s_origin.indexOf("serialNumber") > -1) {
        resp_type = CommHttp::SerialNumber;
        vm_data.insert("data", m_objData.value("data").toString());
    }
    emit send_calc_res(static_cast<int>(resp_type), vm_data);
}
