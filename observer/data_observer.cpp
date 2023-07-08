#include "observer/data_observer.h"
#include "utility/static_container.h"
#include "utility/rga_utility.h"

/*************************************************************************************/
/*  TableObserver
**************************************************************************************/
///
/// \brief TableObserver::TableObserver
/// \param QTableWidget* tbl
///
TableObserver::TableObserver(QTableWidget* tbl)
    : m_zone(tbl) {
}

///
/// \brief TableObserver::~TableObserver
///
TableObserver::~TableObserver() {
    m_zone = nullptr;
    delete m_zone;
}

///
/// \brief TableObserver::update
/// \param data
///
void TableObserver::update() {
    if(m_zone == nullptr) {
        return;
    }
    RgaUtility* inst = StaticContainer::getCrntRga();
    if(!StaticContainer::STC_ISCHARTPAGE) {
        if(!m_zone->rowCount()) {
            return;
        }
        bool b_isFlmtPending = inst->getRgaStatus(RgaUtility::EmissPd);
        bool b_isFlmtOpen = inst->getRgaStatus(RgaUtility::EmissState);
        bool b_isEmPending = inst->getRgaStatus(RgaUtility::EMPd);
        bool b_isEmOpen = inst->getRgaStatus(RgaUtility::EMState);
        bool b_isInCtrl = inst->getInCtrl();
        bool b_isSaveData = inst->getIsSaveData();
        int  i_errCnt = inst->getErrCount();
        bool b_isHasErr = i_errCnt > 0;
        QString s_errCnt = QString::number(i_errCnt);
        QString s_flmtState = "Off";
        QString s_emState = "Off";
        if(b_isFlmtPending) {
            s_flmtState = "Pending";
        }
        if(b_isFlmtOpen) {
            s_flmtState = "On";
        }
        if(b_isEmPending) {
            s_emState = "Pending";
        }
        if(b_isEmOpen) {
            s_emState = "On";
        }
        //        "Addr",     //0
        //        "SN",       //1
        //        "Recipe",   //2
        //        "Filament", //3
        //        "EM",       //4
        //        "Dwell",    //5
        //        "RptUnit",  //6
        //        "DataLog",  //7
        //        "Error",    //8
        //        "RunTm",    //9
        //        "ScanTm",   //10
        //        "TotalPressure",    //11
        //        "ScanNum"   //12
        //addr  0
        m_zone->item(0, 1)->setText(inst->getRgaAddr());
        //sn    1
        m_zone->item(1, 1)->setText(inst->getRgaSn());
        m_zone->item(1, 1)->setBackground(b_isInCtrl ? Qt::darkGreen : Qt::white);
        m_zone->item(1, 1)->setForeground(b_isInCtrl ? Qt::white : Qt::black);
        //recipe    2
        m_zone->item(2, 1)->setText(inst->getRcptInfo(RgaUtility::RcptName));
        //filament status   3
        m_zone->item(3, 1)->setText(s_flmtState + "\t" + inst->getFlmtIdx());
        m_zone->item(3, 1)->setBackground(b_isFlmtOpen ? Qt::darkGreen : Qt::white);
        m_zone->item(3, 1)->setForeground(b_isFlmtOpen ? Qt::white : Qt::black);
        //em status     4
        m_zone->item(4, 1)->setText(s_emState);
        m_zone->item(4, 1)->setBackground(b_isEmOpen ? Qt::darkGreen : Qt::white);
        m_zone->item(4, 1)->setForeground(b_isEmOpen ? Qt::white : Qt::black);
        //dwell     5
        m_zone->item(5, 1)->setText(inst->getRcptInfo(RgaUtility::RcptDwell));
        //report unit       6
        m_zone->item(6, 1)->setText(inst->getRcptInfo(RgaUtility::RcptRptUnit));
        //datalog state       7
        m_zone->item(7, 1)->setText(b_isSaveData ? "On" : "Off");
        if(b_isSaveData) {
            m_zone->item(7, 1)->setToolTip(inst->getFileName());
        }
        //error list    8
        m_zone->item(8, 1)->setText(s_errCnt);
        m_zone->item(8, 1)->setForeground(b_isHasErr ? Qt::darkMagenta : Qt::black);
        //run tm    9
        m_zone->item(9, 1)->setText(inst->getRunTm() + "");
        //scan tm   10
        m_zone->item(10, 1)->setText(QString::number(inst->getScanTmTotal(), 'f', 2) + " ms");
        //total pressure
        m_zone->item(11, 1)->setText(QString::number(inst->getTotalPressure(), 'e', 2) + " " +
                                     inst->getRcptInfo(RgaUtility::RcptTPUnit));
        //scan number
        m_zone->item(12, 1)->setText(QString::number(inst->getScanNum()));
    } else {
        QStringList sl_data = inst->getScanValuesSL(false);
        int i_dataCnt = sl_data.count();
        if(!i_dataCnt) {
            return;
        }
        for (int var = 0; var < i_dataCnt; ++var) {
            m_zone->item(var, 1)->setText(sl_data.at(var));
        }
    }
    m_zone->update();
}


/*************************************************************************************/
/*  ProgressbarObserver
**************************************************************************************/
ProgressbarObserver::ProgressbarObserver(QProgressBar* progress_bar): m_zone(progress_bar) {
}

///
/// \brief ProgressbarObserver::~ProgressbarObserver
///
ProgressbarObserver::~ProgressbarObserver() {
    m_zone = nullptr;
    delete m_zone;
}

///
/// \brief ProgressbarObserver::update
/// \param data
///
void ProgressbarObserver::update() {
    if(m_zone == nullptr) {
        return;
    }
}

///
/// \brief ChartObserver::ChartObserver
/// \param chart
///
ChartObserver::ChartObserver(QCustomPlot* chart): m_zone(chart) {
}

///
/// \brief ChartObserver::~ChartObserver
///
ChartObserver::~ChartObserver() {
    m_zone = nullptr;
    delete m_zone;
}

///
/// \brief ChartObserver::rescaleChart
///
void ChartObserver::rescaleChart() {
    m_zone->setProperty("rescale", false);
    m_zone->rescaleAxes();
    m_zone->replot(QCustomPlot::rpQueuedReplot);
}

///
/// \brief ChartObserver::printChart
///
void ChartObserver::printChart() {
    m_zone->setProperty("print", false);
    QPrinter printer;
//    printer->setPageSize(QPrinter::A4);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setColorMode(QPrinter::Color);
    printer.setFullPage(false);
    QCPPainter painter(&printer);
    int plotWidth = 1460;
    int plotHeight = 1000;
    double scale = 0.75;
//    painter.setMode(QCPPainter::pmVectorized);
//    painter.setMode(QCPPainter::pmNoCaching);
//    painter.setMode(QCPPainter::pmNonCosmetic); // comment this out if you want cosmetic thin lines (always 1 pixel thick independent of pdf zoom level)
    painter.scale(scale, scale);
    m_zone->toPainter(&painter, plotWidth, plotHeight);
}

///
/// \brief ChartObserver::clearChartData
///
void ChartObserver::clearChartData() {
    m_zone->setProperty("clear_data", false);
    for (int var = 0; var < m_zone->graphCount(); ++var) {
        m_zone->graph(var)->data().data()->clear();
    }
    m_zone->replot(QCustomPlot::rpQueuedReplot);
}

/*************************************************************************************/
/*  LineChartObserver
**************************************************************************************/
///
/// \brief LineChartObserver::LineChartObserver
/// \param chart
///
LineChartObserver::LineChartObserver(QCustomPlot* chart): ChartObserver(chart) {
}

///
/// \brief LineChartObserver::~LineChartObserver
///
LineChartObserver::~LineChartObserver() {
}

///
/// \brief LineChartObserver::update
/// \param data
///
void LineChartObserver::update() {
    if(m_zone == nullptr) {
        return;
    }
    RgaUtility* inst = StaticContainer::getCrntRga();
    int i_xVal = (int)QTime::currentTime().msecsSinceStartOfDay() / 1000;
    static double lastPointKey = 0;
    double d_scanTm = inst->getScanTmTotal() / 1000;
    if ((i_xVal - lastPointKey) < d_scanTm) { //add a point according to the scan time
        return;
    }
    if(!inst->getIsNewData() || !StaticContainer::STC_ISCHARTPAGE) {
        return;
    }
    QVector<double> ld_values = inst->getScanValues();
    int i_dataCount = ld_values.count();
    if(ld_values.isEmpty()) {
        return;
    }
    lastPointKey = i_xVal;
    for(int valuePos = 0; valuePos < i_dataCount; valuePos++) {
        m_zone->graph(valuePos)->addData(i_xVal,
                                         ld_values.at(valuePos)
                                        );
    }
    m_zone->xAxis->rescale();
    m_zone->replot(QCustomPlot::rpQueuedReplot);
}

///
/// \brief SpecChartObserver::SpecChartObserver
/// \param chart
///
SpecChartObserver::SpecChartObserver(QCustomPlot* chart): ChartObserver(chart) {
    if(m_qcpBar == nullptr) {
        m_qcpBar = new QCPModify(m_zone->xAxis, m_zone->yAxis);
        m_qcpBar->setWidthType(QCPBars::wtAbsolute);
        m_qcpBar->setPen(QPen(QColor("#53297C").lighter(130)));
        m_qcpBar->setBrush(QColor("#2E598A"));
    }
    if(m_qcpLine == nullptr) {
        m_qcpLine = m_zone->addGraph();
        m_qcpLine->setPen(QPen(QColor("#2E598A"), 2));
        m_qcpLine->setVisible(false);
    }
}

///
/// \brief SpecChartObserver::~SpecChartObserver
///
SpecChartObserver::~SpecChartObserver() {
    m_qcpBar = nullptr;
    delete m_qcpBar;
    m_qcpLine = nullptr;
    delete m_qcpLine;
}

///
/// \brief SpecChartObserver::update
///
void SpecChartObserver::update() {
    if(m_zone == nullptr || !StaticContainer::STC_ISINACQ || !StaticContainer::STC_ISCHARTPAGE) {
        return;
    }
    RgaUtility* inst = StaticContainer::getCrntRga();
    if(!inst->getIsNewData() && !StaticContainer::STC_CELLCLICKED) {
        return;
    }
    bool b_isAScan = inst->getIsAScan();
    m_qcpLine->setVisible(b_isAScan);
    QVector<double> ld_values = inst->getScanValues();
    QVector<double> ld_valuesAll = inst->getScanValues(true);
    QVector<double> vd_ticks = inst->getVdTicks();
    QVector<double> vd_pos = inst->getDataPos();
    if(ld_values.isEmpty() || ld_valuesAll.isEmpty()) {
        return;
    }
    ld_values.pop_front();
    ld_valuesAll.pop_front();
    int i_dataCnt = ld_values.count();
    int i_lineWidth = 18 - i_dataCnt / 6;
    if(i_lineWidth < 0) {
        i_lineWidth = 4;
    }
    m_qcpBar->setWidth(b_isAScan ? 1 : i_lineWidth);
    m_qcpLine->setData(vd_pos, ld_valuesAll);
    m_qcpBar->setData(vd_ticks, ld_values);
    m_zone->yAxis->setRange(0, DataHelper::findMax(ld_values) * 1.17);
    m_zone->replot(QCustomPlot::rpQueuedReplot);
}


TextInfoObserver::TextInfoObserver(QTextBrowser* browser): m_zone(browser) {
}

TextInfoObserver::~TextInfoObserver() {
    m_zone = nullptr;
    delete m_zone;
}

void TextInfoObserver::update() {
    if(m_zone == nullptr || !StaticContainer::STC_ISINACQ || !StaticContainer::STC_ISCHARTPAGE) {
        return;
    }
    RgaUtility* inst = StaticContainer::getCrntRga();
    int i_selCnt = StaticContainer::STC_SELMASS.count();
    QVector<double> ld_values = inst->getScanValues();
    if(!inst->getIsNewData() && !StaticContainer::STC_CELLCLICKED) {
        return;
    }
    if(ld_values.isEmpty() || StaticContainer::STC_SELMASS.isEmpty()) {
        return;
    }
    QString s_info = "";
    double d_pressure = ld_values.takeFirst();
    bool b_isCurrent = inst->getRcptInfo(RgaUtility::RcptRptUnit) == "Current";
    QStringList sl_pos = inst->getScanPos();
//    QList<int> li_posSel;
    QStringList sl_hints;
    QStringList sl_vals;
//    QStringList sl_pp;
//    QStringList sl_ppm;
    QStringList sl_mzs;
    foreach (int t, StaticContainer::STC_SELMASS) {
        QString s_mz = sl_pos.at(t - 1);
        int i_mz = s_mz.toInt();
        double d_val = ld_values.at(t - 1);
        QString s_val = QString::number(d_val, 'e', 2);
        sl_hints.append(DataHelper::get_pos_el(i_mz));
        double d_pp = d_val;
        double d_ppm = 0;
        if(b_isCurrent) {
            d_pp = DataHelper::cal_pp_val(
                       i_mz,
                       d_val,
                       inst->getFcSensVal(),
                       inst->getRgaStatus(RgaUtility::EMState),
                       inst->getEmGainVal(),
                       inst->getPrsUnit(true).toInt()
                   );
            s_val.append("; PP:" + QString::number(d_pp, 'e', 3) + " " + inst->getPrsUnit());
        }
        d_ppm =  1000000 * d_pp / d_pressure;
        s_val.append("; PPM:" + QString::number(d_ppm, 'e', 3));
        sl_vals.append(s_val);
        sl_mzs.append(s_mz);
    }
    QString s_temp = "m/z:\t%1\nValue:\t%2\nElement:\t%3\n*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n\n";
    for (int var = 0; var < i_selCnt; ++var) {
        s_info.append(s_temp.arg(
                          sl_mzs.at(var),
                          sl_vals.at(var),
                          sl_hints.at(var)
                      ));
    }
    m_zone->clear();
    m_zone->setText(s_info);
}

//TBFlmtObserver::TBFlmtObserver(QToolButton* chart_btn): m_zone(chart_btn) {
//}

//TBFlmtObserver::~TBFlmtObserver() {
//    m_zone = nullptr;
//    delete m_zone;
//}

//void TBFlmtObserver::update() {
//    if(m_zone == nullptr || StaticContainer::STC_ISCHARTPAGE) {
//        return;
//    }
//    RgaUtility* inst = StaticContainer::getCrntRga();
//    m_zone->setDisabled(inst->getScanNum() < 3);
//}

TbObserver::TbObserver(QToolButton* btn): m_zone(btn) {
    btn_name = btn->objectName();
}

TbObserver::~TbObserver() {
}

void TbObserver::update() {
    auto inst = StaticContainer::getCrntRga();
    if(inst == nullptr || btn_name == "") {
        return;
    }
    bool rga_in_ctrl = inst->getInCtrl();
    bool rga_flmt_on = inst->getRgaStatus(RgaUtility::EmissState);
    bool rga_em_on = inst->getRgaStatus(RgaUtility::EMState);
    bool rga_err = inst->getErrCount() > 0;
    if(btn_name == "tb_flmt") {
        m_zone->setIcon(rga_flmt_on ? flmt_on : flmt_off);
    }
    if(btn_name == "tb_info") {
        m_zone->setIcon(rga_err ? info_have_err : info_no_err);
    }
    if(btn_name == "tb_em") {
        m_zone->setIcon(rga_em_on ? em_on : em_off);
    }
    if(btn_name == "tb_link") {
        m_zone->setIcon(rga_in_ctrl ? link_on : link_off);
    }
}
