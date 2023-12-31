#ifndef RESPONSECALC_H
#define RESPONSECALC_H

#include <QObject>



class ResponseCalc: public QObject, public QRunnable {
    Q_OBJECT
  public:
    ResponseCalc(const QJsonObject& data, QObject* m_pObj);
    ~ResponseCalc();
    void set_json_obj(const QJsonObject& data);

  private:
    QJsonObject     m_objData;
    QObject*        m_pObj = nullptr;
  signals:
    void send_calc_res(int, const QVariantMap&);

  protected:
    void run();
};

#endif // RESPONSECALC_H
