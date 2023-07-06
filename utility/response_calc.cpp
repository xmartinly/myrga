#include "response_calc.h"

#include "static_container.h"
#include "rga_utility.h"

ResponseCalc::ResponseCalc(const QJsonObject& data): m_objData(data) {
}

ResponseCalc::~ResponseCalc() {
}

void ResponseCalc::setData(const QJsonObject& data) {
    m_objData = data;
}

void ResponseCalc::run() {
    QString s_id = m_objData.value("id").toString();
    RgaUtility* inst = StaticContainer::STC_RGAMAP.value(s_id);
    QString s_origin = m_objData.value("origin").toString();
    //am in controll
    if(s_origin.indexOf("amInControl") > -1) {
        inst->setInCtrl(m_objData.value("data").toBool());
        return;
    }
    //scanSetup
    if(s_origin.indexOf("scanSetup") > -1) {
        if(s_origin.indexOf("scanTimeTotal") > -1) { //scan time total
            inst->setScanTmTotal(m_objData.value("data").toDouble());
            return;
        }
        QJsonObject obj_data = m_objData.value("data").toObject();
        bool b_started = false;
        bool b_stopped = false;
        if(obj_data.contains("scanStart")) {
            b_started = obj_data.value("scanStart").toInt();
        }
        if(obj_data.contains("scanStop")) {
            b_stopped = obj_data.value("scanStop").toString() == "EndOfScan";
        }
        inst->setAcquireState(b_started || !b_stopped);
        return;
    }
    //em gain val
    if(s_origin.indexOf("emGain") > -1) {
        inst->setEmGainVal(m_objData.value("data").toDouble());
        return;
    }
    //fc sensitivity val
    if(s_origin.indexOf("ppSensitivityFactor") > -1) {
        inst->setFcSensVal(m_objData.value("data").toDouble());
        return;
    }
    //system status
    if(s_origin.indexOf("systemStatus") > -1) {
        inst->setRgaStatus(m_objData.value("data").toDouble());
        return;
    }
    //sensor settings
    if(s_origin.indexOf("sensorIonSource") > -1) {
        QJsonObject obj = m_objData.value("data").toObject();
        //filament selected
        if(obj.contains("filamentSelected")) {
            inst->setFlmtIdx(obj.value("filamentSelected").toDouble());
        }
//        if(obj.contains("tPunits")) {
//            inst->setTpDectCurnt(m_objData.value("data").toDouble());
//        }
        return;
    }
    //scan data
    if(s_origin.indexOf("scans") > -1) {
        QJsonObject jo_data = m_objData.value("data").toObject();
        if(jo_data.value("scannum").toInt() > inst->getScanNum()) {
            QJsonArray ja_values = jo_data.value("values").toArray();
            ja_values.pop_front();
            QJsonArray ja_d_values;
            int i_valCnt = ja_values.count();
            for (int var = 0; var < i_valCnt; ++var) {
                ja_d_values.append(fabs(ja_values[var].toDouble()));
            }
            jo_data.insert("d_values", ja_d_values);
            inst->setScanData(jo_data);
            if(inst->getIsSaveData()) {
                inst->writeScanData();
            }
        }
        return;
    }
    //issue log
    if(s_origin.indexOf("errorLog") > -1 || s_origin.indexOf("issueLog") > -1) {
        QJsonObject jo_data = m_objData.value("data").toObject();
        QJsonArray ja_err = jo_data.value("errors").toArray();
        inst->setErrLog(ja_err);
        return;
    }
    //serial number
    if(s_origin.indexOf("serialNumber") > -1) {
        inst->setRgaSn(m_objData.value("data").toString());
        return;
    }
}
