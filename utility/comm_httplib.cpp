
#include "comm_httplib.h"
#include "response_calc.h"
#include "static_container.h"

void CommHttpLib::cmd_enqueue(const QVariant& cmd, bool is_str_list) {
    QMutexLocker locker(&dequeue_mutex);
    if (!is_str_list) {
        cmd_queue.enqueue(cmd.toString());
        return;
    }
    QStringList sl_cmds = cmd.toStringList();
    foreach (QString _s, sl_cmds) {
        cmd_queue.enqueue(_s);
    }
}

void CommHttpLib::cmd_exec(const QString& cmd) {
    QStringList cmd_split = cmd.split("@");
    std::string addr = cmd_split.at(0).toStdString();
    std::string req = cmd_split.at(1).toStdString();
    Client cli(addr);
    cli.set_connection_timeout(0, 50000);
    cli.set_read_timeout(0, 50000);
    cli.set_keep_alive(true);
    auto res = cli.Get(req);
    std::string result = res ? res->body : cmd.toStdString();
    qDebug() << static_cast<int>(res.error());
    if(res->status == 200) {
        QJsonParseError jsonp_err;
        QJsonDocument json_data = QJsonDocument::fromJson(QByteArray::fromStdString(result), &jsonp_err);
        if(jsonp_err.error == QJsonParseError::NoError) {
            auto* inst = StaticContainer::getCrntRga();
            if(inst == nullptr) {
                return;
            }
            QJsonObject data_obj = json_data.object();
            QString s_origin = data_obj.value("origin").toString();
            //am in controll
            if(s_origin.indexOf("amInControl") > -1) {
                inst->set_in_ctrl(data_obj.value("data").toBool());
            }
            //scanSetup
            if(s_origin.indexOf("scanSetup") > -1) {
                if(s_origin.indexOf("scanTimeTotal") > -1) { //scan time total
                    inst->set_in_ctrl(data_obj.value("data").toDouble());
                } else {
                    QJsonObject json_obj = data_obj.value("data").toObject();
                    bool b_started = false;
                    bool b_stopped = false;
                    if(json_obj.contains("scanStart")) {
                        b_started = json_obj.value("scanStart").toInt();
                    }
                    if(json_obj.contains("scanStop")) {
                        b_stopped = json_obj.value("scanStop").toString() == "EndOfScan";
                    }
                    inst->set_acquire_state( b_started || !b_stopped);
                }
            }
            //em gain val
            if(s_origin.indexOf("emGain") > -1) {
                inst->set_em_gain_val(data_obj.value("data").toDouble());
            }
            //fc sensitivity val
            if(s_origin.indexOf("ppSensitivityFactor") > -1) {
                inst->set_fc_sens_val(data_obj.value("data").toDouble());
            }
            //system status
            if(s_origin.indexOf("systemStatus") > -1) {
                inst->set_status(data_obj.value("data").toDouble());
            }
            //sensor settings
            if(s_origin.indexOf("sensorIonSource") > -1) {
                QJsonObject obj = data_obj.value("data").toObject();
                //filament selected
                if(obj.contains("filamentSelected")) {
                    inst->set_flmt_idx(obj.value("data").toDouble());
                }
//                if(obj.contains("tPunits")) {
//                    inst->set(obj.value("data").toDouble());
//                    resp_type = CommHttp::FilamentSelected;
//                    vm_data.insert("data", obj.value("tPunits").toDouble());
//                }
            }
            //scan data
            if(s_origin.indexOf("scans") > -1) {
//                QJsonObject jo_data_temp = {};
//                jo_data_temp = m_objData.value("data").toObject();
//                QJsonObject jo_scan = {};
//                jo_scan = m_objData.value("data").toObject();
//                if(jo_data_temp.value("scannum").toInt() > inst->get_scan_num()) {
//                    QJsonArray ja_values = jo_scan.value("values").toArray();
//                    ja_values.pop_front();
//                    QJsonArray ja_d_values;
//                    int i_valCnt = ja_values.count();
//                    for (int var = 0; var < i_valCnt; ++var) {
//                        ja_d_values.append(fabs(ja_values[var].toDouble()));
//                    }
//                    jo_data_temp.insert("d_values", ja_d_values);
//                    resp_type = CommHttp::ScanData;
//                    vm_data.insert("data", jo_data_temp);
//                }
            }
            //issue log
            if(s_origin.indexOf("errorLog") > -1 || s_origin.indexOf("issueLog") > -1) {
                inst->set_err_list(data_obj.value("data").toArray());
            }
            //serial number
            if(s_origin.indexOf("serialNumber") > -1) {
                inst->set_rga_sn(data_obj.value("data").toString());
            }
            emit resp_arrival();
        }
    }
}

int CommHttpLib::clear_cmd_queue() {
    int cmd_cnt = cmd_queue.count();
    cmd_queue.clear();
    return cmd_cnt;
}

void CommHttpLib::run() {
    while(!stop_cli) {
        if(!cmd_queue.count()) {
            msleep(20);
        } else {
            send_cmd();
        }
    }
    exec();
}

void CommHttpLib::set_stop_cli() {
    stop_cli = true;
}

void CommHttpLib::send_cmd() {
    if (!cmd_queue.count()) {
        return;
    }
    QMutexLocker locker(&dequeue_mutex);
    QString s_cmd = cmd_queue.dequeue();
    qDebug() << s_cmd;
    cmd_exec(s_cmd);
}

void CommHttpLib::get_calc_resp(int type_, const QVariantMap& vm_data) {
    RespType type = static_cast<RespType>(type_);
    auto inst = StaticContainer::getCrntRga();
    if(inst == nullptr || type == ResRespNone) {
        return;
    }
    //am in controll
    if(type == AmInControl) {
        inst->set_in_ctrl(vm_data.value("data").toBool());
    }
    //scanSetup
    if(type == ScanTimeTotal) {
        inst->set_scan_tm_total(vm_data.value("data").toDouble());
    }
    //em gain val
    if(type == EmGain) {
        inst->set_em_gain_val(vm_data.value("data").toDouble());
    }
    //fc sensitivity val
    if(type == PpSensitivityFactor) {
        inst->set_fc_sens_val(vm_data.value("data").toDouble());
    }
    //system status
    if(type == SystemStatus) {
        inst->set_status(vm_data.value("data").toDouble());
    }
    //filament selected
    if(type == FilamentSelected) {
        inst->set_flmt_idx(vm_data.value("data").toDouble());
    }
    //scan data
    if(type == ScanData) {
        QJsonObject jo_scan_data = vm_data.value("data").toJsonObject();
        inst->set_scan_data(jo_scan_data);
        if(inst->get_is_save_data()) {
            inst->write_scan_data();
        }
    }
    //issue log
    if(type == ErrorLog) {
        inst->set_err_list(vm_data.value("data").toJsonArray());
    }
    //issue log
    if(type == ScanStatus) {
        inst->set_acquire_state(vm_data.value("data").toBool());
    }
    //serial number
    if(type == SerialNumber) {
        inst->set_rga_sn(vm_data.value("data").toString());
    }
    emit resp_arrival();
}
