#include "static_container.h"

bool StaticContainer::STC_CELLCLICKED   = false;

bool StaticContainer::STC_ISINACQ       = false;

bool StaticContainer::STC_ISMISCINFO    = false;

bool StaticContainer::STC_ISDEBUG       = false;

int StaticContainer::STC_ACQINTVL       = 250;

int StaticContainer::STC_IDLINTVL       = 500;

int StaticContainer::STC_LONGINTVL      = 2000;

QString StaticContainer::STC_RVERSION     = " myRGA Comm. v1.1.5 \t";

QMap<QString, QString> StaticContainer::STC_RVER = {
    {"2023/8/23 v1.1.5", u8"fix pressure unit issue"},
    {"2023/6/28 v1.1.4", u8"fix data save issue"},
    {"2023/5/6 v1.0.3", u8"fix scan time not show issue"},
    {"2023/5/5 v1.0.2", u8"multithread calculate response, lower system resource"},

};

RgaUtility* StaticContainer::m_crntRga  = nullptr;

QMap<QString, RgaUtility*> StaticContainer::STC_RGAMAP = {};

QList<int> StaticContainer::STC_SELMASS = {};


StaticContainer::StaticContainer(QObject* parent): QObject(parent) {
}

StaticContainer::~StaticContainer() {
}

RgaUtility* StaticContainer::get_crnt_rga() {
    return m_crntRga;
}

void StaticContainer::setCrntRga(RgaUtility* new_crntRga) {
    m_crntRga = new_crntRga;
}
