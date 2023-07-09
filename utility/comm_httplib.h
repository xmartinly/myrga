
#ifndef COMMHTTPLIB_H
#define COMMHTTPLIB_H

#include <QObject>
#include <QThread>
#include <QMutexLocker>


using namespace httplib;

class CommHttpLib : public QThread {
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


    CommHttpLib(QObject* parent = nullptr): QThread(parent), stop_cli(false) {
    }

    ~CommHttpLib() {
        quit();
        wait();
    }

    void cmd_enqueue(const QVariant& cmd, bool is_str_list = false);

    void cmd_exec(const QString& cmd);

    int clear_cmd_queue();

    bool get_stop_cli() const;
    void set_stop_cli();



  protected:
    virtual void run() Q_DECL_OVERRIDE;

  private:
    QQueue<QString>         cmd_queue = {};
    QThreadPool*            calc_poll = nullptr;
    bool                    stop_cli = false;

    QMutex                  dequeue_mutex;

  private slots:
    void send_cmd();

  public slots:
    void get_calc_resp(int type_, const QVariantMap& vm_data);

  signals:
    void resp_arrival();
};

#endif // COMMHTTPLIB_H
