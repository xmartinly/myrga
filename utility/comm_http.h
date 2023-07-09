#ifndef COMMHTTP_H
#define COMMHTTP_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>

///
/// \brief The CommHttp class
///
class CommHttp : public QObject {
    Q_OBJECT

  private:
    explicit CommHttp(QObject* parent = nullptr);
    QNetworkAccessManager*  m_qnaHttp = nullptr;
    QNetworkRequest         m_req;
    QList<QNetworkCookie>   m_allCookies;
    QTimer*                 m_tmr = nullptr;
    QQueue<QString>         m_cmdQ;
    QThreadPool*            m_pool;
    QMutex                  m_mutex ;

    CommHttp(CommHttp&) = delete;
    CommHttp& operator=(CommHttp ch) = delete;

    class Private;
    friend class Private;
    Private* d;

  private slots:
    void replyFinishedSlot(QNetworkReply* reply);

  public:

    enum RespType {
        ResRespNone,
        AmInControl,
        SerialNumber,
        ErrorLog,
        IssueLog,
        ScanData,
        FilamentSelected,
        TPunits,
        SystemStatus,
        PpSensitivityFactor,
        EmGain,
        ScanStart,
        ScanStop,
        ScanTimeTotal,
        ScanStatus,
    };

    void cmd_enqueue(QVariant cmd, bool is_str_list = false);
    static CommHttp* GetInstance();
    void cmd_exec(QString cmd);
    ~CommHttp();

  public slots:
    void send_queue_cmd();

    void clear_cmd_queue();

    void get_calc_resp(int type_, const QVariantMap& vm_data);

  signals:

    void resp_arrival();
};

#endif  // COMMHTTP_H
