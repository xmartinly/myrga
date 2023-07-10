#ifndef STATICCONTAINER_H
#define STATICCONTAINER_H

#include <QObject>
#include "rga_utility.h"

class StaticContainer: public QObject {
    Q_OBJECT
  public:
    explicit StaticContainer(QObject* parent = nullptr);
    ~StaticContainer();

    static QMap<QString, RgaUtility*> STC_RGAMAP;


    static QList<int> STC_SELMASS;

    static QString    STC_CRNTID;

    static QString    STC_RVERSION;

    static bool    STC_ISCHARTPAGE;

    static bool    STC_CELLCLICKED;

    static bool    STC_ISASCAN;

    static bool    STC_ISINACQ;

    static bool    STC_ISMISCINFO;

    static bool    STC_ISDEBUG;

    static int     STC_ACQINTVL;

    static int     STC_IDLINTVL;

    static int     STC_LONGINTVL;

    static RgaUtility* get_crnt_rga();
    static void setCrntRga(RgaUtility* new_crntRga);

  private:
    static RgaUtility*     m_crntRga;
};

#endif // STATICCONTAINER_H
