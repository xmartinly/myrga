#include "rga_utility.h"
#include "static_container.h"
#include <QDateTime>
#include <QTime>

RgaUtility::RgaUtility(QString addr, RecipeSet recipe): m_rcpt(recipe), m_addr(addr) {
    cmdStrArr[0]    = &HttpCommand::noAction;   //return the address without 'http://'
    cmdStrArr[1]    = &HttpCommand::setForceCtrl;
    cmdStrArr[2]    = &HttpCommand::getAmInCtrl;
    cmdStrArr[3]    = &HttpCommand::setReleaseCtrl;
    cmdStrArr[4]    = &HttpCommand::setScanStop;
    cmdStrArr[5]    = &HttpCommand::setScanStart;
    cmdStrArr[6]    = &HttpCommand::setFlmtOn;
    cmdStrArr[7]    = &HttpCommand::setFlmtOff;
    cmdStrArr[8]    = &HttpCommand::setEmOn;
    cmdStrArr[9]    = &HttpCommand::setEmOff;
    cmdStrArr[10]   = &HttpCommand::getSn;
    cmdStrArr[11]   = &HttpCommand::getSysStatus;
    cmdStrArr[12]   = &HttpCommand::getErrLog;
    cmdStrArr[13]   = &HttpCommand::getLastScanDouble;
    cmdStrArr[14]   = &HttpCommand::setReboot;
    cmdStrArr[15]   = &HttpCommand::getIssueLog;
    cmdStrArr[16]   = &HttpCommand::getCurrentFlmts;
    cmdStrArr[17]   = &HttpCommand::setFlmt1st;
    cmdStrArr[18]   = &HttpCommand::setFlmt2nd;
    cmdStrArr[19]   = &HttpCommand::setMeasBegin;
    cmdStrArr[20]   = &HttpCommand::getTotalTm;
    cmdStrArr[21]   = &HttpCommand::setScanCnt;
    cmdStrArr[22]   = &HttpCommand::getEmGainReq;
    cmdStrArr[23]   = &HttpCommand::getFcSensReq;
}

RgaUtility::~RgaUtility() {
}

bool RgaUtility::emManual() const {
    return m_bEmManual;
}

void RgaUtility::setEmManual(bool newbEmManual) {
    m_bEmManual = newbEmManual;
}

QString RgaUtility::flmtIdxSet() const {
    return m_rcpt.s_flmtIdx;
}

void RgaUtility::setFlmtIdxSet(const QString& news_flmtIdx) {
    m_rcpt.s_flmtIdx = news_flmtIdx;
}

///
/// \brief RgaUtility::acqSet
/// \return
///
bool RgaUtility::acquireSet() const {
    return m_bAcqSet;
}

///
/// \brief RgaUtility::setAcqSet
/// \param newb_acq_set
///
void RgaUtility::setAcquireSet(bool newb_acq_set) {
    m_bAcqSet = newb_acq_set;
}


double RgaUtility::dLimitVal() const {
    return m_dLimitVal;
}

void RgaUtility::setDLimitVal(double newDLimitVal) {
    Q_ASSERT_X(newDLimitVal > 0, Q_FUNC_INFO, "d_val error");
    m_dLimitVal = newDLimitVal;
}

///
/// \brief RgaUtility::getDataPos
/// \param is_all
/// \return
///
QVector<double> RgaUtility::getDataPos() const {
    return m_vdDataPos;
}

///
/// \brief RgaUtility::setDataPos
/// \param new_dataPos
///
void RgaUtility::setDataPos(const QVector<double>& new_dataPos) {
    m_vdDataPos = new_dataPos;
}

///
/// \brief RgaUtility::getRunTm
/// \return
///
QString RgaUtility::getRunTm() const {
    return  QTime::fromMSecsSinceStartOfDay(m_iRunTm).toString("HH:mm:ss");
}

///
/// \brief RgaUtility::addRunTm
///
void RgaUtility::addRunTm() {
    if(acquireState()) {
        m_iRunTm += StaticContainer::STC_IDLINTVL;
    }
}

///
/// \brief RgaUtility::resetAll
///
void RgaUtility::resetAll() {
    m_errLog.resetErrList();
    m_stat.resetStatus();
    m_scanData.resetData();
}

///
/// \brief RgaUtility::getEmGainVal
/// \return
///
const double RgaUtility::getEmGainVal() {
    return m_stat.d_emGain;
}

///
/// \brief RgaUtility::setEmGainVal
/// \param d_val
///
void RgaUtility::setEmGainVal(double d_val) {
    Q_ASSERT_X(d_val > 0, Q_FUNC_INFO, "d_val error");
    m_stat.d_emGain = d_val;
}

///
/// \brief RgaUtility::getFcSensVal
/// \return
///
const double RgaUtility::getFcSensVal() {
    return m_stat.d_fcSens;
}

///
/// \brief RgaUtility::setFcSensVal
/// \param d_val
///
void RgaUtility::setFcSensVal(double d_val) {
    Q_ASSERT_X(d_val > 0, Q_FUNC_INFO, "d_val error");
    m_stat.d_fcSens = d_val;
}

///
/// \brief RgaUtility::getVdTicks
/// \return
///
const QVector<double> RgaUtility::getVdTicks() {
    return m_vdTicks;
}

///
/// \brief RgaUtility::getVsLabels
/// \return
///
const QVector<QString> RgaUtility::getVsLabels() {
    return m_vsLabels;
}

///
/// \brief RgaUtility::genTicker
///
void RgaUtility::genTicker() {
    m_vsLabels.clear();
    m_vdTicks.clear();
    m_vdDataPos.clear();
    QStringList sl_points = getScanPos();
    bool b_isSweep = getIsSweep();
    bool b_isAScan = getIsAScan();
    int i_ppamu = getPPAmu();
    int i_start = sl_points.first().toInt();
    int i_stop = sl_points.last().toInt();
    int i_posCnt = b_isSweep ? (i_stop - i_start + 1) : sl_points.count();
    int i_idx = 0;
    for (int var = 0; var < i_posCnt; ++var) {
//        bool b_isLbShow = true;
//        if(b_isSweep) {
//            b_isLbShow = (var % 5 == 0);
//        }
//        m_vsLabels.append(b_isLbShow ? sl_points.at(var) : "");
        m_vsLabels.append(sl_points.at(var));
        m_vdTicks.append(++i_idx);
        m_vdDataPos.append(var);
    }
    if(b_isAScan) {
        m_vdDataPos.clear();
        m_vdDataPos.append(DataHelper::gen_ppamu_pos(i_start, i_stop, i_ppamu));
    }
}

///
/// \brief RgaUtility::getIsSweep
/// \return
///
const bool RgaUtility::getIsSweep() const {
    return m_rcpt.s_method == "Sweep";
}

////
/// \brief RgaUtility::setRgaAcqState
/// \param is_in_acq
///
void RgaUtility::setAcquireState(bool is_in_acq) {
    m_stat.b_isInAcquire = is_in_acq;
}

///
/// \brief RgaUtility::getRgaAcqState
/// \return
///
const bool RgaUtility::acquireState() {
    return m_stat.b_isInAcquire;
}

///
/// \brief RgaUtility::setRgaLabel
/// \param lb
///
void RgaUtility::setRgaLabel(QLabel* lb) {
    m_lb = lb;
    m_lbText = lb->text() + "\n\n";
}

///
/// \brief RgaUtility::getRgaLabel
/// \return
///
QLabel* RgaUtility::getRgaLabel() {
    return m_lb;
}


void RgaUtility::initDataFile(bool is_crateFile) {
    if(!is_crateFile) {
        delete m_dataFile;
        m_dataFile = nullptr;
        m_fileHeader.clear();
        m_fileName.clear();
        return;
    }
    m_fileName = "";
    genDataFileName();
    QString s_fileFolder = DataHelper::get_file_folder("data");
    QDir d_fileFolder = QDir(s_fileFolder);
    if(!d_fileFolder.exists()) {
        d_fileFolder.mkpath(s_fileFolder);
    }
    QString s_filePath = s_fileFolder + '/' + m_fileName;
    m_dataFile = new QFile(s_filePath);
    QString s_fileHeader = genFileHeaders();
    DataHelper::write_data_file(s_fileHeader, m_dataFile);
}

void RgaUtility::writeScanData(bool final) {
    if(m_dataFile == nullptr) {
        return;
    }
    QStringList sl_data = getScanValuesSL();
    if(sl_data.isEmpty()) {
        return;
    }
    QString s_pres = sl_data.takeFirst();
    QString s_data = sl_data.join(",");
    s_data.prepend(",,,");
    s_data.prepend(s_pres);
    s_data.prepend(QString::number(getScanTS()) + ",");
    s_data.prepend(QString::number(getScanNum()) + ",");
    s_data.append("\n");
    m_stringData.append(s_data);
    bool b_scanCountTen = m_scanData.i_scanNum % 10 == 0;
    if(b_scanCountTen || final) {
        DataHelper::write_data_file(m_stringData, m_dataFile);
        m_stringData.clear();
    }
}

///
/// \brief RgaUtility::setLbText. set text label
/// \param finish_scan. bool
///
void RgaUtility::setLbText(bool finish_scan) {
    int i_starts = static_cast<int>(m_starIntvl) * m_acqCnt;
    if(finish_scan) {
        i_starts = 0;
        m_acqCnt = 0;
    }
    m_lb->setText("");
    QString s_lb = "";
    for (int var = 0; var < i_starts; ++var) {
        s_lb.append(">");
    }
    m_acqCnt++;
    m_lb->setText(m_lbText + s_lb);
}

///
/// \brief RgaUtility::genFileHeaders
/// \return
///
const QString RgaUtility::genFileHeaders() {
    QString s_sweepRange = m_rcpt.s_startMass + " - " + m_rcpt.s_stopMass;
    QString s_points = m_rcpt.sl_points.join(",");
    QStringList sl_rcptInfo;
    QString ss_addr = m_addr;
    ss_addr.replace("http://", "");
    bool b_isSweep = m_rcpt.s_method == "Sweep";
    QString s_range = b_isSweep ? s_sweepRange : s_points;
    int i_ppamu = m_rcpt.s_ppamu.toInt();
    sl_rcptInfo.append("Recipe:," + m_rcpt.s_rcpName);
    sl_rcptInfo.append("Method:," + m_rcpt.s_method);
    sl_rcptInfo.append("Range:," + s_range);
    sl_rcptInfo.append("Dwell:," + m_rcpt.s_dwell);
    sl_rcptInfo.append("PPamu:," + m_rcpt.s_ppamu);
    sl_rcptInfo.append("Addr:," + ss_addr);
    sl_rcptInfo.append("StartTm:," + QDateTime::currentDateTime().toString("yyyy-mm-dd HH:mm:ss"));
    QString s_head = sl_rcptInfo.join("\n");
    s_head.append("\n");
    s_head += "ScanNum,ScanTm,TotalPressure,,data,";
    QString s_dataPos = s_range;
    if(b_isSweep) {
        s_dataPos.clear();
        int i_start = m_rcpt.s_startMass.toInt();
        int i_stop = m_rcpt.s_stopMass.toInt() + 1;
        for (int var = i_start; var < i_stop; ++var) {
            int i_ppamuVal = i_ppamu;
            s_dataPos.append(QString::number(var));
            while (i_ppamuVal > 0) {
                s_dataPos.append(",");
                i_ppamuVal--;
            }
        }
    }
    s_dataPos.append("\n");
    s_head.append(s_dataPos);
    return s_head;
}

///
/// \brief RgaUtility::getScanSet
/// \return
///
const QStringList RgaUtility::getScanSet() {
    genScanSet();
    resetOverTime();
    return m_scanSet;
}

///
/// \brief RgaUtility::getIdlSet
/// \return
///
const QStringList RgaUtility::getIdlSet() {
    genIdlSet();
    return m_idlSet;
}

///
/// \brief RgaUtility::getCloseSet
/// \return
///
const QStringList RgaUtility::getCloseSet() {
    genCloseSet();
    return m_closeSet;
}

const QStringList RgaUtility::getStopSet() {
    m_localTS = 0;
    m_iRunTm = 0;
    QList<RgaActions> actions = {StopAcquire};
    QStringList sl_stop = {};
    foreach (auto act, actions) {
        sl_stop.append(genRgaAction(act));
    }
    return sl_stop;
}

///
/// \brief RgaUtility::genCloseSet
///
void RgaUtility::genCloseSet() {
    m_closeSet.clear();
    QList<RgaActions> actions = {StopAcquire, CloseEm, CloseFlmt, RleaseCtrl};
    foreach (auto act, actions) {
        m_closeSet.append(genRgaAction(act));
    }
}

///
/// \brief RgaUtility::getFileName
/// \return
///
const QString RgaUtility::getFileName() {
    return m_fileName;
}

///
/// \brief RgaUtility::getScanPos
/// \return
///
const QStringList RgaUtility::getScanPos() {
    if(m_rcpt.s_method == "Points") {
        QStringList sl_pointsPos = m_rcpt.sl_points;
        return sl_pointsPos;
    }
    int i_start = m_rcpt.s_startMass.toInt();
    int i_stop  = m_rcpt.s_stopMass.toInt() + 1;
    QStringList sl_pos = {};
    for (int var = i_start; var < i_stop; ++var) {
        sl_pos.append(QString::number(var));
    }
    return sl_pos;
}

///
/// \brief RgaUtility::setIsSaveData
/// \param save_data
///
void RgaUtility::setIsSaveData(bool save_data) {
    if(save_data) {
        initDataFile();
    }
    m_bSaveData = save_data;
}

///
/// \brief RgaUtility::getIsSaveData
/// \return
///
const bool RgaUtility::getIsSaveData() {
    return m_bSaveData;
}

///
/// \brief RgaUtility::setAcquireCnt
/// \param count
///
void RgaUtility::setAcquireCnt(int count) {
    m_acqCnt = count;
}

///
/// \brief RgaUtility::getAcqireCnt
/// \return
///
const int RgaUtility::getAcqireCnt() {
    return m_acqCnt;
}

///
/// \brief RgaUtility::genDataFileName
///
void RgaUtility::genDataFileName() {
    if(m_fileName == "") {
        m_fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss_") + m_tag + "_" + m_rcpt.s_rcpName + ".csv";
    }
}

///
/// \brief RgaUtility::getDataPointsCount
/// \return
///
int RgaUtility::getDataPointsCount() {
    if(m_rcpt.s_method == "Points") {
        return 0;
    }
    int i_start     = m_rcpt.s_startMass.toInt();
    int i_stop      = m_rcpt.s_stopMass.toInt();
    int i_ppamu     = m_rcpt.s_ppamu.toInt();
    int i_range     = i_stop - i_start;
    return i_range * i_ppamu;
}

///
/// \brief RgaUtility::getIsAScan. return true if ppamu > 1
/// \return
///
bool RgaUtility::getIsAScan() const {
    int i_ppamu = m_rcpt.s_ppamu.toInt();
    return i_ppamu > 1;
}

int RgaUtility::getOverTime() {
    qint64 i_now = QDateTime::currentMSecsSinceEpoch();
    if(!m_localTS) {
        return false;
    }
    return m_localTS - i_now;
}

void RgaUtility::resetOverTime() {
    m_localTS = QDateTime::currentMSecsSinceEpoch() + m_rcpt.i_period;
}

///
/// \brief RgaUtility::getRunSet
/// \return
///
const bool RgaUtility::getRunSet() {
    return m_rcpt.s_run.toInt();
}

///
/// \brief RgaUtility::setRunSet
/// \param s_run
///
void RgaUtility::setRunSet(const QString& s_run) {
    m_rcpt.s_run = s_run;
}


///
/// \brief RgaUtility::genIdlSet
///
void RgaUtility::genIdlSet() {
    m_idlSet.clear();
    QList<RgaActions> idl_actions = {
        AmInCtrl,
        GetSn,
        GetSysStatus,
        GetIssueLog
    };
    bool b_isEmOpened = getRgaStatus(EMState);
    bool b_isFlmtOpened = getRgaStatus(EmissState);
    bool b_isSetEmOn  = m_rcpt.s_emOpt == "On";
    foreach (auto act, idl_actions) {
        m_idlSet.append(genRgaAction(act));
    }
    if(b_isSetEmOn && !b_isEmOpened && b_isFlmtOpened && !m_bEmManual) {
        m_idlSet.append(genRgaAction(OpenEm));
    }
    if(!getEmGainVal()) {
        m_idlSet.append(genRgaAction(GetEmGainReq));
    }
    if(!getFcSensVal()) {
        m_idlSet.append(genRgaAction(GetFcSensReq));
    }
}

///
/// \brief RgaUtility::genScanSet
///
void RgaUtility::genScanSet() {
    m_scanSet.clear();
    QList<ScanHeadChSet> headList = {Timestamp, TotalPressure};
    QList<int> i_headList = {};
    foreach (auto ch, headList) {
        i_headList.append((int)ch);
    }
    if(m_scanSet.count() > 0) {
        m_scanSet.clear();
    }
    m_scanSet.append(genRgaAction(StopAcquire));
    m_scanSet = m_cmd.setupScanChs(
                    m_addr,
                    m_rcpt.s_startMass,
                    m_rcpt.s_stopMass,
                    m_rcpt.s_ppamu,
                    m_rcpt.s_rUnit,
                    m_rcpt.s_method,
                    m_rcpt.s_dwell,
                    m_rcpt.sl_points,
                    i_headList
                );
    m_scanSet.append(m_cmd.setChannelsRange(m_addr, QString::number(m_scanSet.count())));
    RgaActions flmt_idx = flmtIdxSet() == "1" ? SetFlmt1st : SetFlmt2nd;
    m_scanSet.append(genRgaAction(flmt_idx));
    m_scanSet.append(m_cmd.setTpUnits(m_addr, m_rcpt.s_pUnit));
    m_scanSet.append(genRgaAction(SetScanCnt));
    m_scanSet.append(genRgaAction(OpenFlmt));
    m_scanSet.append(genRgaAction(StartAcquire));
    m_scanSet.append(genRgaAction(SetMeasBegin));
    m_scanSet.append(genRgaAction(GetScanTm));
}

///
/// \brief RgaUtility::genRgaAction
/// \param action
/// \return
///
const QString RgaUtility::genRgaAction(RgaActions action) {
    return (cmdStrArr[static_cast<int>(action)])(m_addr);
}

///
/// \brief RgaUtility::getRcpt
/// \return
///
const RecipeSet RgaUtility::getRcpt() {
    return m_rcpt;
}

///
/// \brief RgaUtility::getRcptInfo
/// \param info_code
/// \return
///
const QString RgaUtility::getRcptInfo(RecipeInfo info_code) {
    QString s_info = "";
    switch (info_code) {
        case RcptName:
            s_info = m_rcpt.s_rcpName;
            break;
        case RcptDwell:
            s_info = m_rcpt.s_dwell;
            break;
        case RcptRptUnit:
            s_info = m_rcpt.s_rUnit;
            break;
        case RcptTPUnit:
            s_info = getPrsUnit();
            break;
        default:
            break;
    }
    return s_info;
}

///
/// \brief RgaUtility::getPrsUnit
/// \return
///
const QString RgaUtility::getPrsUnit(bool b_isNum) {
    int i_prsUnit = m_rcpt.s_pUnit.toInt();
    if(b_isNum) {
        return m_rcpt.s_pUnit;
    }
    QString s_unit;
    switch (i_prsUnit) {
        case 0:
            s_unit = "Torr";
            break;
        case 1:
            s_unit = "mBar";
            break;
        case 2:
            s_unit = "Pa";
            break;
        default:
            s_unit = "";
            break;
    }
    return s_unit;
}

///
/// \brief RgaUtility::getPPAmu
/// \return
///
const int RgaUtility::getPPAmu() {
    return m_rcpt.s_ppamu.toInt();
}

///
/// \brief RgaUtility::setFlmtIdx
/// \param index
///
void RgaUtility::setFlmtIdx(double index) {
    if(!index) {
        return;
    }
    m_stat.s_curntFlmt = QString::number(index);
}

///
/// \brief RgaUtility::getFlmtIdx
/// \return
///
QString RgaUtility::getFlmtIdx() {
    return m_stat.s_curntFlmt;
}

///
/// \brief RgaUtility::getTblHead
/// \param b_isChartPage
/// \return
///
const QStringList RgaUtility::getTblCol(bool b_isChartPage) {
    QStringList sl_header = {"TotalPressure"};
    bool b_isSweep = m_rcpt.s_method == "Sweep";
    if(!b_isChartPage) {
        return m_mainTblHead;
    }
    if(b_isSweep) {
        int i_startMass     = m_rcpt.s_startMass.toInt();
        int i_stopMass      = m_rcpt.s_stopMass.toInt();
        for (int var = i_startMass; var < i_stopMass + 1; ++var) {
            sl_header.append(QString::number(var) + " m/z");
        }
    } else {
        foreach (QString s_p, m_rcpt.sl_points) {
            sl_header.append(s_p + " m/z");
        }
    }
    return sl_header;
}

///
/// \brief RgaUtility::setRgaAddr
/// \param addr
///
void RgaUtility::setRgaAddr(QString addr) {
    m_addr = addr;
}

///
/// \brief RgaUtility::getRgaAddr
/// \return
///
const QString RgaUtility::getRgaAddr() {
    return m_addr;
}

///
/// \brief RgaUtility::setRgaTag
/// \param s_tag
///
void RgaUtility::setRgaTag(QString s_tag) {
    m_tag = s_tag;
}

///
/// \brief RgaUtility::getRgaTag
/// \return
///
const QString RgaUtility::getRgaTag() {
    return m_tag;
}

///
/// \brief RgaUtility::setRgaSn
/// \param rga_sn
///
void RgaUtility::setRgaSn(QString rga_sn) {
    m_sn = rga_sn;
}

///
/// \brief RgaUtility::getRgaSn
/// \return
///
const QString RgaUtility::getRgaSn() {
    return m_sn;
}

///
/// \brief RgaUtility::setScanTmTotal
/// \param value
///
void RgaUtility::setScanTmTotal(double value) {
    m_scanData.d_scanTmTotal = value;
}

///
/// \brief RgaUtility::getScanTmTotal
/// \return
///
const double RgaUtility::getScanTmTotal() {
    return m_scanData.d_scanTmTotal;
}

///
/// \brief RgaUtility::setInCtrl
/// \param rga_inCtrl
///
void RgaUtility::setInCtrl(bool rga_inCtrl) {
    m_stat.b_isInCtrl = rga_inCtrl;
}

///
/// \brief RgaUtility::getInCtrl
/// \return
///
const bool RgaUtility::getInCtrl() {
    return m_stat.b_isInCtrl;
}

///
/// \brief RgaUtility::setScanTS
/// \param i_tmStmp
///
void RgaUtility::setScanTS(qint64 i_tmStmp) {
    m_scanData.i_scanTS = i_tmStmp;
}

///
/// \brief RgaUtility::getScanTS
/// \return
///
const qint64 RgaUtility::getScanTS() {
    return m_scanData.i_scanTS;
}

///
/// \brief RgaUtility::setLocalTS
/// \param i_tmStmp
///
void RgaUtility::setLocalTS(qint64 i_tmStmp) {
    m_scanData.i_scanTS = i_tmStmp;
}

///
/// \brief RgaUtility::setScanNum
/// \param scan_num
///
void RgaUtility::setScanNum(int scan_num) {
    m_scanData.i_scanNum = scan_num;
}

///
/// \brief RgaUtility::getScanNum
/// \return
///
const int RgaUtility::getScanNum() {
    return m_scanData.i_scanNum;
}

///
/// \brief RgaUtility::setEmisCurnt
/// \param value
///
void RgaUtility::setEmisCurnt(double value) {
    m_scanData.d_emisCurnt = value;
}

///
/// \brief RgaUtility::getEmisCurnt
/// \return
///
const double RgaUtility::getEmisCurnt() {
    return m_scanData.d_emisCurnt;
}

///
/// \brief RgaUtility::setTotalPressure
/// \param value
///
void RgaUtility::setTotalPressure(double value) {
    m_scanData.d_tPress = value;
}

///
/// \brief RgaUtility::getTotalPressure
/// \return
///
const double RgaUtility::getTotalPressure() {
    return m_scanData.d_tPress;
}

///
/// \brief RgaUtility::setBaseLine
/// \param value
///
void RgaUtility::setBaseLine(double value) {
    m_scanData.d_baseLine = value;
}

///
/// \brief RgaUtility::getBaseLine
/// \return
///
const double RgaUtility::getBaseLine() {
    return m_scanData.d_baseLine;
}

///
/// \brief RgaUtility::setTotalPressureQuad
/// \param value
///
void RgaUtility::setTotalPressureQuad(double value) {
    m_scanData.d_tpQuad = value;
}

///
/// \brief RgaUtility::getTPressureQuad
/// \return
///
const double RgaUtility::getTPressureQuad() {
    return m_scanData.d_tpQuad;
}

///
/// \brief RgaUtility::setTpDectCurnt
/// \param value
///
void RgaUtility::setTpDectCurnt(double value) {
    m_scanData.d_tpDectCurnt = value;
}

///
/// \brief RgaUtility::getTpDectCurnt
/// \return
///
const double RgaUtility::getTpDectCurnt() {
    return m_scanData.d_tpDectCurnt;
}

///
/// \brief RgaUtility::setAnodePotxl
/// \param value
///
void RgaUtility::setAnodePotxl(double value) {
    m_scanData.d_anodePotxl = value;
}

///
/// \brief RgaUtility::getAnodePotxl
/// \return
///
const double RgaUtility::getAnodePotxl() {
    return m_scanData.d_anodePotxl;
}

///
/// \brief RgaUtility::setFocusPotxl
/// \param value
///
void RgaUtility::setFocusPotxl(double value) {
    m_scanData.d_focusPotxl = value;
}

///
/// \brief RgaUtility::getFocusPotxl
/// \return
///
const double RgaUtility::getFocusPotxl() {
    return m_scanData.d_focusPotxl;
}

///
/// \brief RgaUtility::setEltrEnrg
/// \param value
///
void RgaUtility::setEltrEnrg(double value) {
    m_scanData.d_eEnrg = value;
}

///
/// \brief RgaUtility::getEltrEnrg
/// \return
///
const double RgaUtility::getEltrEnrg() const {
    return m_scanData.d_eEnrg;
}

///
/// \brief RgaUtility::setFlmtCurnt
/// \param value
///
void RgaUtility::setFlmtCurnt(double value) {
    m_scanData.d_flmtCurnt = value;
}

///
/// \brief RgaUtility::getFlmtCurnt
/// \return
///
const double RgaUtility::getFlmtCurnt() {
    return m_scanData.d_flmtCurnt;
}

///
/// \brief RgaUtility::setScanSize
/// \param scan_size
///
void RgaUtility::setScanSize(int scan_size) {
    m_scanData.i_scanSize = scan_size;
}

///
/// \brief RgaUtility::getScanSize
/// \return
///
const int RgaUtility::getScanSize() const {
    return m_scanData.i_scanSize;
}

///
/// \brief RgaUtility::setRgaScanValues
/// \param values
///
void RgaUtility::setRgaScanValues(const QVector<double>& values) {
    m_scanData.vd_scanValues = values;
}

///
/// \brief RgaUtility::getScanValuesSL
/// \param data_index
/// \return
///
const QStringList RgaUtility::getScanValuesSL(bool is_all) {
    if(m_scanData.vd_scanValues.count() < 1) {
        return {};
    }
    QVector<double> vd_values = getScanValues(is_all);
    QStringList sl_val;
    foreach (double val, vd_values) {
        sl_val.append(QString::number(val, 'e', 3));
    }
    return sl_val;
}

///
/// \brief RgaUtility::getScanValues
/// \param is_all
/// \return
///
const QVector<double> RgaUtility::getScanValues(bool is_all) {
    if(m_scanData.vd_scanValues.count() < 1) {
        return {};
    }
    QVector<double> vd_values = m_scanData.vd_scanValues;
    int i_ppamu = getPPAmu();
    if(is_all || i_ppamu == 1) {
        return vd_values;
    }
    QVector<double> vd_res = {};
    int i_startMass = m_rcpt.s_startMass.toInt();
    int i_stopMass = m_rcpt.s_stopMass.toInt();
    int i_massRange = i_stopMass - i_startMass + 1;
    double d_pres = vd_values.takeFirst();
    for (int var = 0; var < i_massRange ; ++var) {
        vd_res.append(vd_values.at(var * i_ppamu));
    }
    vd_res.prepend(d_pres);
    return vd_res;
}

///
/// \brief RgaUtility::setErrLog
/// \param err_list
///
void RgaUtility::setErrLog(const QJsonArray& err_list) {
    m_errLog.i_errCount = 0;
    m_errLog.sl_errList.clear();
    foreach(auto s, err_list) {
        QString s_errLevel = s["level"].toString();
        if(s_errLevel.indexOf("HardwareAnomaly") > -1 || s_errLevel.indexOf("HardwareError") > -1) {
            m_errLog.sl_errList.append(s["level"].toString() + ":\n" + s["issue"].toString() + "\n");
            ++m_errLog.i_errCount;
        }
    }
}

void RgaUtility::resetErrList() {
    m_errLog.resetErrList();
}

const int RgaUtility::getErrCount() {
    return m_errLog.i_errCount;
}

const QStringList RgaUtility::getErrList() {
    return m_errLog.sl_errList;
}


///
/// \brief RgaUtility::resetStatus
///
void RgaUtility::resetStatus() {
    m_stat.resetStatus();
}

///
/// \brief RgaUtility::setRgaStatus
/// \param status_code
///
void RgaUtility::setRgaStatus(double status_code) {
    m_stat.i_statusCode = (uint)status_code;
    addRunTm();
}

///
/// \brief RgaUtility::getRgaStatus
/// \param status
/// \return
///
const bool RgaUtility::getRgaStatus(SysStatusCode status) {
    return (m_stat.i_statusCode & (1 << (31 - static_cast<int>(status))));
}

///
/// \brief RgaUtility::setScanData
/// \param rga_scanData
/// \param ch_heads
/// \param is_pp
/// \return
///
void RgaUtility::setScanData(const QJsonObject& rga_scanData) {
    int i_scanNum               = rga_scanData.value("scannum").toInt();
    resetScanData();
    //**********************************************************************************//
    //** scannum
    setScanNum(i_scanNum);
    //**********************************************************************************//
    //** RealTm
    setLocalTS(QDateTime::currentMSecsSinceEpoch());
    //**********************************************************************************//
    //** scansize
    int i_scanSize              = rga_scanData.value("scansize").toInt();
    setScanSize(i_scanSize);
    //**********************************************************************************//
    //** scan values
    QJsonArray ja_scanVal   = rga_scanData.value("d_values").toArray();
    setTotalPressure(ja_scanVal.first().toDouble());
    foreach (QJsonValue val, ja_scanVal) {
        m_scanData.vd_scanValues.append(val.toDouble());
    }
    setNewData(true);
}

///
/// \brief RgaUtility::resetScanData
///
void RgaUtility::resetScanData() {
    m_scanData.resetData();
}

void RgaUtility::setNewData(bool is_new_data) {
    m_scanData.b_isNewData = is_new_data;
}

const bool RgaUtility::getIsNewData() {
    return m_scanData.b_isNewData;
}
