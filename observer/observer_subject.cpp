#include "observer_subject.h"

QList<DataObserver*> ObserverSubject::STC_OBS;

ObserverSubject::ObserverSubject() {
}

///
/// \brief ObserverSubject::~ObserverSubject
///
ObserverSubject::~ObserverSubject() {
}

///
/// \brief ObserverSubject::addObserver
/// \param obs
///
void ObserverSubject::add_obs(DataObserver* obs) {
    m_obsrvs.append(obs);
}

int ObserverSubject::remove_all_obs() {
    qDeleteAll(m_obsrvs.begin(), m_obsrvs.end());
    m_obsrvs.clear();
    return m_obsrvs.count();
}

///
/// \brief ObserverSubject::removeObserver
/// \param obs
///
void ObserverSubject::remove_obs(DataObserver* obs) {
    m_obsrvs.removeOne(obs);
}

///
/// \brief ObserverSubject::notifyObs
/// \param data
///
void ObserverSubject::notify_obs() {
    if(m_obsrvs.count() < 1) {
        return;
    }
    foreach (auto obs, m_obsrvs) {
        obs->update();
    }
}

///
/// \brief ObserverSubject::countObs
/// \return const int, m_obsrvs count.
///
const int ObserverSubject::count_obs() {
    return m_obsrvs.count();
}
