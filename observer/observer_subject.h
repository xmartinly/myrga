#ifndef OBSERVERSUBJECT_H
#define OBSERVERSUBJECT_H

#include "data_observer.h"

class ObserverSubject {
  public:
    ObserverSubject();
    ~ObserverSubject();

    void add_obs(DataObserver* obs);

    int remove_all_obs();

    void remove_obs(DataObserver* obs);

    const int count_obs();

    void notify_obs();

    static QList<DataObserver*> STC_OBS;

  private:
    QList<DataObserver*> m_obsrvs;
};

#endif // OBSERVERSUBJECT_H
