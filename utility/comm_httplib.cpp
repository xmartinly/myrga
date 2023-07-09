
#include "comm_httplib.h"
#include "response_calc.h"
#include "static_container.h"

void CommHttpLib::cmd_exec(const QString& cmd) {
}

void CommHttpLib::run() {
    using namespace httplib;
    QStringList req_cmd = req_url.split("@");
    if(req_url == "" && req_cmd.count() != 2) {
        return;
    }
    Client cli(req_cmd.at(0).toStdString());
    cli.set_connection_timeout(0, 100000); // 580 milliseconds
    cli.set_read_timeout(0, 100000); // 50 milliseconds
    cli.set_decompress(true);
    cli.set_keep_alive(false);
    auto res = cli.Get(req_cmd[1].toStdString());
    QString s_result = "";
    s_result = res ? QString::fromStdString(res->body) : req_url + " % " + QString::number(static_cast<int>(res.error()));
    qDebug() << s_result;
//    emit sendResp(QString::number(QDateTime::currentMSecsSinceEpoch() - i_start), s_result, static_cast<int>(res.error()));
}


void CommHttpLib::resp_handle(const std::string& data) {
}
