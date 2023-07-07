﻿#include "comm_http.h"

#include "response_calc.h"
#include "rga_utility.h"
#include "static_container.h"
///
/// \brief The CommHttp::Private class. CTOR,
///
class CommHttp::Private {
  public:
    Private(CommHttp* q) : manager(new QNetworkAccessManager(q)) {
        manager->setProxy(QNetworkProxy::NoProxy);
    }
    QNetworkAccessManager* manager;
};

///
/// \brief CommHttp. constructor
/// \param parent
///
CommHttp::CommHttp(QObject* parent) : QObject(parent) {
    m_pool = new QThreadPool(this);
    m_pool->setMaxThreadCount(QThread::idealThreadCount());
    //    m_pool->setStackSize();
    m_tmr = new QTimer(this);
    connect(m_tmr, &QTimer::timeout, this, &CommHttp::sendQueueCmd);
    d = new CommHttp::Private(this);
    connect(d->manager, &QNetworkAccessManager::finished, this,
            &CommHttp::replyFinishedSlot);
}

///
/// \brief CommHttp::GetInstance
/// \return instance pointer
///
CommHttp* CommHttp::GetInstance() {
    static CommHttp networkUtil;
    return &networkUtil;
}

///
/// \brief CommHttp::execCmd. execute command
/// \param cmd
///
void CommHttp::execCmd(QString cmd) {
    d->manager->get(QNetworkRequest(QUrl(cmd)));
}

///
/// \brief CommHttp::~CommHttp
/// disconnect QNetworkAccessManager finished slot.
/// delete m_qnaHttp pointer.
///
CommHttp::~CommHttp() {
    delete d;
    d = nullptr;
}

///
/// \brief CommHttp::replyFinishedSlot. response handle.
/// \param reply
///
void CommHttp::replyFinishedSlot(QNetworkReply* reply) {
    reply->deleteLater();
    if (reply->error() == QNetworkReply::NoError) {
        QList<QNetworkCookie> lc_cookie =
            d->manager->cookieJar()->cookiesForUrl(reply->url());
        QString s_rgaIp = lc_cookie.value(0).domain();
        QByteArray data = reply->readAll();
        QJsonParseError jsonpe;
        QJsonDocument json_data = QJsonDocument::fromJson(data, &jsonpe);
        if (jsonpe.error == QJsonParseError::NoError) {
            QJsonObject data_obj = json_data.object();
            data_obj.insert("id", s_rgaIp.replace(".", ""));
            ResponseCalc* calc = new ResponseCalc(data_obj, this);
            connect(calc, &ResponseCalc::sendCalcResult, this, &CommHttp::getCalcResp);
            m_pool->start(calc);
        }
    }
}

///
/// \brief CommHttp::cmdEnQueue
/// \param cmd
/// \param is_str_list
///
void CommHttp::cmdEnQueue(QVariant cmd, bool is_str_list) {
    if (!m_tmr->isActive()) {
        m_tmr->start(10);
    }
    if (!is_str_list) {
        m_cmdQ.enqueue(cmd.toString());
        return;
    }
    QStringList sl_cmds = cmd.toStringList();
    foreach (QString _s, sl_cmds) {
        m_cmdQ.enqueue(_s);
    }
}

///
/// \brief CommHttp::sendQueueCmd. timeout action
///
void CommHttp::sendQueueCmd() {
    if (!m_cmdQ.count()) {
        return;
    }
    QString s_cmd = m_cmdQ.dequeue();
    qDebug() << s_cmd;
    d->manager->get(QNetworkRequest(QUrl(s_cmd)));
    if (m_cmdQ.count() > 100) {
        clearCmdQ();
    }
}

///
/// \brief CommHttp::clearCmdQ. clear command queue
///
void CommHttp::clearCmdQ() {
    m_cmdQ.clear();
}

void CommHttp::getCalcResp(int type_, const QVariantMap& vm_data) {
    RespType type = static_cast<RespType>(type_);
//    QString s_id = vm_data.value("rga_id").toString();
    auto inst = StaticContainer::getCrntRga();
    if(inst == nullptr || type == ResRespNone) {
        return;
    }
//    RgaUtility* inst = StaticContainer::STC_RGAMAP.value(s_id);
    //am in controll
    if(type == AmInControl) {
        inst->setInCtrl(vm_data.value("data").toBool());
    }
    //scanSetup
    if(type == ScanTimeTotal) {
        inst->setScanTmTotal(vm_data.value("data").toDouble());
    }
    //em gain val
    if(type == EmGain) {
        inst->setEmGainVal(vm_data.value("data").toDouble());
    }
    //fc sensitivity val
    if(type == PpSensitivityFactor) {
        inst->setFcSensVal(vm_data.value("data").toDouble());
    }
    //system status
    if(type == SystemStatus) {
        inst->setRgaStatus(vm_data.value("data").toDouble());
    }
    //filament selected
    if(type == FilamentSelected) {
        inst->setFlmtIdx(vm_data.value("data").toDouble());
    }
    //scan data
    if(type == ScanData) {
        QJsonObject jo_scan_data = vm_data.value("data").toJsonObject();
        inst->setScanData(jo_scan_data);
        if(inst->getIsSaveData()) {
            inst->writeScanData();
        }
    }
    //issue log
    if(type == ErrorLog) {
        inst->setErrLog(vm_data.value("data").toJsonArray());
    }
    //issue log
    if(type == ScanStatus) {
        inst->setAcquireState(vm_data.value("data").toBool());
    }
    //serial number
    if(type == SerialNumber) {
        inst->setRgaSn(vm_data.value("data").toString());
    }
    emit respArrival();
}

