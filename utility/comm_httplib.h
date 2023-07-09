
#ifndef COMMHTTPLIB_H
#define COMMHTTPLIB_H

#include <QObject>
#include <QThread>
#include <QMutexLocker>


using namespace httplib;

class CommHttpLib: public QObject, public QRunnable {
    Q_OBJECT
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

    explicit CommHttpLib(const QString& request): req_url(request) {
    }
    ~CommHttpLib() {
    }

    static void cmd_exec(const QString& cmd);

  private:
    void resp_handle(const std::string& data);

  protected:
    void run() override;

  private:
    QString req_url;

  signals:
    void resp_arrival();
};

#endif // COMMHTTPLIB_H
