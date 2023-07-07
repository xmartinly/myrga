#ifndef OBSERVERSUBJECT_H
#define OBSERVERSUBJECT_H

#include "data_observer.h"

class ObserverSubject {
  public:
    ObserverSubject();
    ~ObserverSubject();

    void addObserver(DataObserver* obs);

    int removeAll();

    void removeObserver(DataObserver* obs);

    const int countObs();

    void notify_obs();

    static QList<DataObserver*> STC_OBS;

  private:
    QList<DataObserver*> m_obsrvs;
};

#endif // OBSERVERSUBJECT_H
