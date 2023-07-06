#ifndef COMMHTTP_H
#define COMMHTTP_H

#include <QObject>
#include "thirdparty/httplib.h"

///
/// \brief The CommHttp class
///
class CommHttp : public QObject {
    Q_OBJECT
  private:
    explicit CommHttp(QObject* parent = nullptr);
    httplib::Client* http = nullptr;
//    QNetworkAccessManager*  m_qnaHttp = nullptr;
//    QNetworkRequest         m_req;
//    QList<QNetworkCookie>   m_allCookies;
    QTimer*                 m_tmr = nullptr;
    QQueue<QString>         m_cmdQ;
    QThreadPool*            m_pool;
    //    QString                 m_clientInfo = "Mozilla/5.0 (Windows NT 10.0;
    //    Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/104.0.0.0
    //    Safari/537.36"; QString                 m_contenInfo =
    //    "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9";

    CommHttp(CommHttp&) = delete;
    CommHttp& operator=(CommHttp ch) = delete;

    class Private;
    friend class Private;
    Private* d;

  private slots:
//    void replyFinishedSlot(QNetworkReply* reply);

  public:
    void cmdEnQueue(QVariant cmd, bool is_str_list = false);
    static CommHttp* GetInstance();
    void execCmd(QString cmd);
    ~CommHttp();

  public slots:
    void sendQueueCmd();

    void clearCmdQ();

  signals:

    void respArrival();
};

#endif  // COMMHTTP_H
