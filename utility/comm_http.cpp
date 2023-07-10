#include "comm_http.h"

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
    m_tmr = new QTimer(this);
    connect(m_tmr, &QTimer::timeout, this, &CommHttp::send_queue_cmd);
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
void CommHttp::cmd_exec(QString cmd) {
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
            connect(calc, &ResponseCalc::send_calc_res, this, &CommHttp::get_calc_resp);
            m_pool->start(calc);
        }
    }
}

///
/// \brief CommHttp::cmdEnQueue
/// \param cmd
/// \param is_str_list
///
void CommHttp::cmd_enqueue(QVariant cmd, bool is_str_list) {
    QMutexLocker locker(&m_mutex);
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
void CommHttp::send_queue_cmd() {
    QMutexLocker locker(&m_mutex);
    if (!m_cmdQ.count()) {
        return;
    }
    QString s_cmd = m_cmdQ.dequeue();
    d->manager->get(QNetworkRequest(QUrl(s_cmd)));
    if (m_cmdQ.count() > 100) {
        clear_cmd_queue();
    }
}

///
/// \brief CommHttp::clearCmdQ. clear command queue
///
void CommHttp::clear_cmd_queue() {
    m_cmdQ.clear();
}

void CommHttp::get_calc_resp(int type_, const QVariantMap& vm_data) {
    RespType type = static_cast<RespType>(type_);
    auto inst = StaticContainer::get_crnt_rga();
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

