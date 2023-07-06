#include "static_container.h"

bool StaticContainer::STC_ISCHARTPAGE   = false;

bool StaticContainer::STC_CELLCLICKED   = false;

bool StaticContainer::STC_ISASCAN       = false;

bool StaticContainer::STC_ISINACQ       = false;

int StaticContainer::STC_ACQINTVL       = 250;

int StaticContainer::STC_IDLINTVL       = 2000;

QString StaticContainer::STC_CRNTID     = "";


QString StaticContainer::STC_RVERSION     = " myRGA Comm. v1.0.1 \t";

RgaUtility* StaticContainer::m_crntRga  = nullptr;

QMap<QString, RgaUtility*> StaticContainer::STC_RGAMAP = {};

QList<int> StaticContainer::STC_SELMASS = {};


StaticContainer::StaticContainer(QObject* parent): QObject(parent) {
}

StaticContainer::~StaticContainer() {
}

RgaUtility* StaticContainer::getCrntRga() {
    return m_crntRga;
}

void StaticContainer::setCrntRga(RgaUtility* new_crntRga) {
    m_crntRga = new_crntRga;
}
