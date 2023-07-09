
#include "comm_httplib.h"
#include "response_calc.h"
#include "static_container.h"

void CommHttpLib::cmd_enqueue(const QVariant& cmd, bool is_str_list) {
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
    auto res = cli.Get(req);
    std::string result = res ? res->body : cmd.toStdString();
    qDebug() << QString::fromStdString(result);
//    qDebug() << ip_addr << res->status;
//    if(res->status == 200) {
//        QJsonParseError jsonp_err;
//        QJsonDocument json_data = QJsonDocument::fromJson(QByteArray::fromStdString(result), &jsonp_err);
//        if(jsonp_err.error == QJsonParseError::NoError) {
//            QJsonObject data_obj = json_data.object();
//            ResponseCalc* calc = new ResponseCalc(data_obj, this);
//            connect(calc, &ResponseCalc::send_calc_res, this, &CommHttpLib::get_calc_resp);
//            calc_poll->start(calc);
//        }
//    }
}

int CommHttpLib::clear_cmd_queue() {
    int cmd_cnt = cmd_queue.count();
    cmd_queue.clear();
    return cmd_cnt;
}

void CommHttpLib::run() {
    qDebug() << __FUNCTION__;
    this->exec();
//    while(!stop_cli) {
//        send_cmd();
//        {
//            QMutexLocker locker(&dequeue_mutex);
//            if (stop_cli) {
//                break;
//            }
//        }
//    }
//    exec();
}

bool CommHttpLib::get_stop_cli() const {
    return stop_cli;
}

void CommHttpLib::set_stop_cli() {
    QMutexLocker locker(&dequeue_mutex);
    stop_cli = true;
}

void CommHttpLib::send_cmd() {
    if (!cmd_queue.count()) {
        return;
    }
    cmd_exec(cmd_queue.dequeue());
    if (cmd_queue.count() > 100) {
        clear_cmd_queue();
    }
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
