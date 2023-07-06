#ifndef RESPONSECALC_H
#define RESPONSECALC_H

#include <QObject>


class ResponseCalc: public QObject, public QRunnable {
    Q_OBJECT
  public:
    ResponseCalc(const QJsonObject& data);
    ~ResponseCalc();
    void setData(const QJsonObject& data);

  private:

    QJsonObject m_objData;

  protected:
    void run();
};

#endif // RESPONSECALC_H
