#include "rga_utility.h"
#include "static_container.h"
#include <QDateTime>
#include <QTime>

RgaUtility::RgaUtility(QString addr, RecipeSet recipe): m_rcpt(recipe), rga_addr(addr) {
    setup_func_ptrs();
}

RgaUtility::RgaUtility() {
    setup_func_ptrs();
}

RgaUtility::~RgaUtility() {
    if(data_file_ptr) {
        delete data_file_ptr;
    }
}

void RgaUtility::set_scan_rcpt(const RecipeSet& rcpt) {
    m_rcpt = rcpt;
}

bool RgaUtility::get_em_manual() const {
    return m_rcpt.s_emOpt == "1";
}

void RgaUtility::set_em_manual(int em_manual) {
    if((em_manual < 0) || (em_manual > 1)) {
        return;
    }
    m_rcpt.s_emOpt = QString::number(em_manual);
}


double RgaUtility::get_limit_val() const {
    return scan_val_low;
}

void RgaUtility::set_limit_val(double new_limit) {
    Q_ASSERT_X(new_limit > 0, Q_FUNC_INFO, "d_val error");
    scan_val_low = new_limit;
}

///
/// \brief RgaUtility::getDataPos
/// \param is_all
/// \return
///
QVector<double> RgaUtility::get_data_pos() const {
    return spec_data_pos;
}

///
/// \brief RgaUtility::setDataPos
/// \param new_dataPos
///
void RgaUtility::set_data_pos(const QVector<double>& new_dataPos) {
    spec_data_pos = new_dataPos;
}

///
/// \brief RgaUtility::getRunTm
/// \return
///
QString RgaUtility::get_str_runtm() const {
    qint64 time_elapsed = QDateTime::currentMSecsSinceEpoch() - start_tmstamp;
    return QTime::fromMSecsSinceStartOfDay(time_elapsed).toString("HH:mm:ss");
}

///
/// \brief RgaUtility::addRunTm
///
void RgaUtility::add_runtm() {
    if(get_acquire_state()) {
        recipe_run_tm += StaticContainer::STC_IDLINTVL;
    }
}

///
/// \brief RgaUtility::resetAll
///
void RgaUtility::reset_all() {
    m_errLog.resetErrList();
    m_stat.resetStatus();
    m_scanData.resetData();
}

void RgaUtility::setup_func_ptrs() {
    cmd_str_arr[0]    = &HttpCommand::noAction;   //return the address without 'http://'
    cmd_str_arr[1]    = &HttpCommand::setForceCtrl;
    cmd_str_arr[2]    = &HttpCommand::getAmInCtrl;
    cmd_str_arr[3]    = &HttpCommand::setReleaseCtrl;
    cmd_str_arr[4]    = &HttpCommand::setScanStop;
    cmd_str_arr[5]    = &HttpCommand::setScanStart;
    cmd_str_arr[6]    = &HttpCommand::setFlmtOn;
    cmd_str_arr[7]    = &HttpCommand::setFlmtOff;
    cmd_str_arr[8]    = &HttpCommand::setEmOn;
    cmd_str_arr[9]    = &HttpCommand::setEmOff;
    cmd_str_arr[10]   = &HttpCommand::getSn;
    cmd_str_arr[11]   = &HttpCommand::getSysStatus;
    cmd_str_arr[12]   = &HttpCommand::getErrLog;
    cmd_str_arr[13]   = &HttpCommand::getLastScanDouble;
    cmd_str_arr[14]   = &HttpCommand::setReboot;
    cmd_str_arr[15]   = &HttpCommand::getIssueLog;
    cmd_str_arr[16]   = &HttpCommand::getCurrentFlmts;
    cmd_str_arr[17]   = &HttpCommand::setFlmt1st;
    cmd_str_arr[18]   = &HttpCommand::setFlmt2nd;
    cmd_str_arr[19]   = &HttpCommand::setMeasBegin;
    cmd_str_arr[20]   = &HttpCommand::getTotalTm;
    cmd_str_arr[21]   = &HttpCommand::setScanCnt;
    cmd_str_arr[22]   = &HttpCommand::getEmGainReq;
    cmd_str_arr[23]   = &HttpCommand::getFcSensReq;
}

///
/// \brief RgaUtility::getEmGainVal
/// \return
///
const double RgaUtility::get_em_gain_val() {
    return m_stat.d_emGain;
}

///
/// \brief RgaUtility::setEmGainVal
/// \param d_val
///
void RgaUtility::set_em_gain_val(double d_val) {
    Q_ASSERT_X(d_val > 0, Q_FUNC_INFO, "d_val error");
    m_stat.d_emGain = d_val;
}

///
/// \brief RgaUtility::getFcSensVal
/// \return
///
const double RgaUtility::get_fc_sens_val() {
    return m_stat.d_fcSens;
}

///
/// \brief RgaUtility::setFcSensVal
/// \param d_val
///
void RgaUtility::set_fc_sens_val(double d_val) {
    Q_ASSERT_X(d_val > 0, Q_FUNC_INFO, "d_val error");
    m_stat.d_fcSens = d_val;
}

///
/// \brief RgaUtility::getVdTicks
/// \return
///
const QVector<double> RgaUtility::get_vd_ticks() {
    return spec_x_ticks;
}

///
/// \brief RgaUtility::getVsLabels
/// \return
///
const QVector<QString> RgaUtility::get_vs_labels(const QList<int> pos) {
    if(!pos.count()) {
        return spec_x_labels;
    }
    QVector<QString> labels = {};
    return labels;
}

///
/// \brief RgaUtility::genTicker
///
void RgaUtility::gen_ticker() {
    spec_x_labels.clear();
    spec_x_ticks.clear();
    spec_data_pos.clear();
    QStringList sl_points = get_scan_pos();
    bool b_isSweep = get_is_sweep();
    bool b_isAScan = get_is_alg_scan();
    int i_ppamu = get_ppamu();
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
        spec_x_labels.append(sl_points.at(var));
        spec_x_ticks.append(++i_idx);
        spec_data_pos.append(var);
    }
    if(b_isAScan) {
        spec_data_pos.clear();
        spec_data_pos.append(DataHelper::gen_ppamu_pos(i_start, i_stop, i_ppamu));
    }
}

///
/// \brief RgaUtility::getIsSweep
/// \return
///
const bool RgaUtility::get_is_sweep() const {
    return m_rcpt.s_method == "Sweep";
}

////
/// \brief RgaUtility::setRgaAcqState
/// \param is_in_acq
///
void RgaUtility::set_acquire_state(bool is_in_acq) {
    start_tmstamp = QDateTime::currentMSecsSinceEpoch();
    m_stat.b_isInAcquire = is_in_acq;
}

///
/// \brief RgaUtility::getRgaAcqState
/// \return
///
const bool RgaUtility::get_acquire_state() {
    return m_stat.b_isInAcquire;
}

///
/// \brief RgaUtility::setRgaLabel
/// \param lb
///
//void RgaUtility::set_rga_label(QLabel* lb) {
//    m_lb = lb;
//    lb_text = lb->text() + "\n\n";
//}

///
/// \brief RgaUtility::getRgaLabel
/// \return
///
//QLabel* RgaUtility::get_rga_label() {
//    return m_lb;
//}


void RgaUtility::init_data_file(bool is_crateFile) {
    if(!is_crateFile) {
        delete data_file_ptr;
        data_file_ptr = nullptr;
        data_file_name.clear();
        return;
    }
    data_file_name = "";
    gen_file_name();
    QString s_fileFolder = DataHelper::get_file_folder("data");
    QDir d_fileFolder = QDir(s_fileFolder);
    if(!d_fileFolder.exists()) {
        d_fileFolder.mkpath(s_fileFolder);
    }
    QString s_filePath = s_fileFolder + '/' + data_file_name;
    data_file_ptr = new QFile(s_filePath);
    QString s_fileHeader = gen_file_header();
    DataHelper::write_data_file(s_fileHeader, data_file_ptr);
}

void RgaUtility::write_scan_data(bool final) {
    if(data_file_ptr == nullptr) {
        return;
    }
    QStringList sl_data = get_scan_sl_val();
    if(sl_data.isEmpty()) {
        return;
    }
    QString s_pres = sl_data.takeFirst();
    QString s_data = sl_data.join(",");
    s_data.prepend(",,,");
    s_data.prepend(s_pres);
    s_data.prepend(QString::number(get_scan_tmstmp()) + ",");
    s_data.prepend(QString::number(get_scan_num()) + ",");
    s_data.append("\n");
    string_data.append(s_data);
    bool b_scanCountTen = m_scanData.i_scanNum % 10 == 0;
    if(b_scanCountTen || final) {
        DataHelper::write_data_file(string_data, data_file_ptr);
        string_data.clear();
    }
}

///
/// \brief RgaUtility::setLbText. set text label
/// \param finish_scan. bool
///
//void RgaUtility::set_label_text(bool finish_scan) {
//    int i_starts = static_cast<int>(stars_intvl) * scan_count;
//    if(finish_scan) {
//        i_starts = 0;
//        scan_count = 0;
//    }
//    m_lb->setText("");
//    QString s_lb = "";
//    for (int var = 0; var < i_starts; ++var) {
//        s_lb.append(">");
//    }
//    scan_count++;
//    m_lb->setText(lb_text + s_lb);
//}

///
/// \brief RgaUtility::genFileHeaders
/// \return
///
const QString RgaUtility::gen_file_header() {
    QString s_sweepRange = m_rcpt.s_startMass + " - " + m_rcpt.s_stopMass;
    QString s_points = m_rcpt.sl_points.join(",");
    QStringList sl_rcptInfo;
    QString ss_addr = rga_addr;
    ss_addr.replace("http://", "");
    bool b_isSweep = m_rcpt.s_method == "Sweep";
    QString s_range = b_isSweep ? s_sweepRange : s_points;
    int i_ppamu = m_rcpt.s_ppamu.toInt();
    sl_rcptInfo.append("Method:," + m_rcpt.s_method);
    sl_rcptInfo.append("Dwell:," + m_rcpt.s_dwell);
    sl_rcptInfo.append("PPamu:," + m_rcpt.s_ppamu);
    sl_rcptInfo.append("FCSens:," + QString::number(get_fc_sens_val(), 'e', 3));
    sl_rcptInfo.append("EMGain:," + QString::number(get_em_gain_val(), 'e', 3));
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
const QStringList RgaUtility::get_scan_set() {
    gen_scan_set();
    reset_over_tm();
    return m_scanSet;
}

///
/// \brief RgaUtility::getIdlSet
/// \return
///
const QStringList RgaUtility::get_idle_set() {
    gen_idle_set();
    return m_idlSet;
}

///
/// \brief RgaUtility::getCloseSet
/// \return
///
const QStringList RgaUtility::get_close_set() {
    gen_close_set();
    return m_closeSet;
}

const QStringList RgaUtility::get_stop_set() {
    local_tmstamp = 0;
    recipe_run_tm = 0;
    QList<RgaActions> actions = {StopAcquire};
    QStringList sl_stop = {};
    foreach (auto act, actions) {
        sl_stop.append(gen_rga_action(act));
    }
    return sl_stop;
}

///
/// \brief RgaUtility::genCloseSet
///
void RgaUtility::gen_close_set() {
    m_closeSet.clear();
    QList<RgaActions> actions = {StopAcquire, CloseEm, CloseFlmt, RleaseCtrl};
    foreach (auto act, actions) {
        m_closeSet.append(gen_rga_action(act));
    }
}

///
/// \brief RgaUtility::getFileName
/// \return
///
const QString RgaUtility::get_file_name() {
    return data_file_name;
}

///
/// \brief RgaUtility::getScanPos
/// \return
///
const QStringList RgaUtility::get_scan_pos() {
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
void RgaUtility::set_is_save_data(bool save_data) {
    if(save_data) {
        init_data_file();
    }
    is_save_data = save_data;
}

///
/// \brief RgaUtility::getIsSaveData
/// \return
///
const bool RgaUtility::get_is_save_data() {
    return is_save_data;
}

///
/// \brief RgaUtility::setAcquireCnt
/// \param count
///
void RgaUtility::set_acquier_cnt(int count) {
    scan_count = count;
}

///
/// \brief RgaUtility::getAcqireCnt
/// \return
///
const int RgaUtility::get_acquire_cnt() {
    return scan_count;
}

///
/// \brief RgaUtility::genDataFileName
///
void RgaUtility::gen_file_name() {
    if(data_file_name == "") {
        data_file_name = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss_") + rga_tag + "_" + m_rcpt.s_rcpName + ".csv";
    }
}

///
/// \brief RgaUtility::getDataPointsCount
/// \return
///
int RgaUtility::get_data_points_cnt() {
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
bool RgaUtility::get_is_alg_scan() const {
    int i_ppamu = m_rcpt.s_ppamu.toInt();
    return i_ppamu > 1;
}

int RgaUtility::get_over_tm() {
    if(m_rcpt.s_run == "1") {
        return 1;
    }
    qint64 i_now = QDateTime::currentMSecsSinceEpoch();
    if(!local_tmstamp) {
        return 0;
    }
    return local_tmstamp - i_now;
}

void RgaUtility::reset_over_tm() {
    local_tmstamp = QDateTime::currentMSecsSinceEpoch() + m_rcpt.i_period;
}

///
/// \brief RgaUtility::getRunSet
/// \return
///
const bool RgaUtility::get_run_set() {
    return m_rcpt.s_run.toInt();
}

///
/// \brief RgaUtility::setRunSet
/// \param i_run
///
void RgaUtility::set_run_set(int i_run) {
    m_rcpt.s_run = QString::number(i_run);
}
///
/// \brief RgaUtility::setRunSet
/// \param s_run
///
void RgaUtility::set_run_set(const QString& s_run) {
    m_rcpt.s_run = s_run;
}

///
/// \brief RgaUtility::genIdlSet
///
void RgaUtility::gen_idle_set() {
    m_idlSet.clear();
    QList<RgaActions> idl_actions = {
        AmInCtrl,
        GetSn,
        GetSysStatus,
        GetIssueLog
    };
    bool b_isEmOpened = get_status(EMState);
    bool b_isFlmtOpened = get_status(EmissState);
    bool b_isSetEmOn  = m_rcpt.s_emOpt == "On";
    foreach (auto act, idl_actions) {
        m_idlSet.append(gen_rga_action(act));
    }
    if(b_isSetEmOn && !b_isEmOpened && b_isFlmtOpened && m_rcpt.s_emOpt == "1") {
        m_idlSet.append(gen_rga_action(OpenEm));
    }
    if(!get_em_gain_val()) {
        m_idlSet.append(gen_rga_action(GetEmGainReq));
    }
    if(!get_fc_sens_val()) {
        m_idlSet.append(gen_rga_action(GetFcSensReq));
    }
}

///
/// \brief RgaUtility::genScanSet
///
void RgaUtility::gen_scan_set() {
    m_scanSet.clear();
    QList<ScanHeadChSet> headList = {Timestamp, TotalPressure};
    QList<int> i_headList = {};
    foreach (auto ch, headList) {
        i_headList.append((int)ch);
    }
    if(m_scanSet.count() > 0) {
        m_scanSet.clear();
    }
    m_scanSet.append(gen_rga_action(StopAcquire));
    m_scanSet = m_cmd.setupScanChs(
                    rga_addr,
                    m_rcpt.s_startMass,
                    m_rcpt.s_stopMass,
                    m_rcpt.s_ppamu,
                    m_rcpt.s_rUnit,
                    m_rcpt.s_method,
                    m_rcpt.s_dwell,
                    m_rcpt.sl_points,
                    i_headList
                );
    m_scanSet.append(m_cmd.setChannelsRange(rga_addr, QString::number(m_scanSet.count())));
//    if(m_stat.s_curntFlmt != m_rcpt.s_flmtIdx) {
//        m_scanSet.append(gen_rga_action(CloseFlmt));
//        m_scanSet.append(gen_rga_action(CloseFlmt));
//        m_scanSet.append(gen_rga_action(CloseFlmt));
//    }
//    RgaActions flmt_idx = m_rcpt.s_flmtIdx == "1" ? SetFlmt1st : SetFlmt2nd;
//    m_scanSet.append(gen_rga_action(flmt_idx));
    m_scanSet.append(m_cmd.setTpUnits(rga_addr, m_rcpt.s_pUnit));
    m_scanSet.append(gen_rga_action(SetScanCnt));
    m_scanSet.append(gen_rga_action(OpenFlmt));
    m_scanSet.append(gen_rga_action(StartAcquire));
    m_scanSet.append(gen_rga_action(SetMeasBegin));
    m_scanSet.append(gen_rga_action(GetScanTm));
}

///
/// \brief RgaUtility::genRgaAction
/// \param action
/// \return
///
const QString RgaUtility::gen_rga_action(RgaActions action) {
    return (cmd_str_arr[static_cast<int>(action)])(rga_addr);
}

///
/// \brief RgaUtility::getRcptInfo
/// \param info_code
/// \return
///
const QString RgaUtility::get_rcpt_info(RecipeInfo info_code) {
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
            s_info = get_pres_unit();
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
const QString RgaUtility::get_pres_unit(bool b_isNum) {
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
const int RgaUtility::get_ppamu() {
    return m_rcpt.s_ppamu.toInt();
}

///
/// \brief RgaUtility::setFlmtIdx
/// \param index
///
void RgaUtility::set_flmt_idx(int flmt_idx) {
    if(!flmt_idx || flmt_idx > 2 || flmt_idx < 0) {
        return;
    }
    m_stat.s_curntFlmt = QString::number(flmt_idx);
}

///
/// \brief RgaUtility::getFlmtIdx
/// \return
///
QString RgaUtility::get_flmt_idx() {
    return m_stat.s_curntFlmt;
}

const QString RgaUtility::get_flmt_setted() {
    return m_rcpt.s_flmtIdx;
}


///
/// \brief RgaUtility::getTblHead
/// \param b_isChartPage
/// \return
///
const QStringList RgaUtility::get_tbl_col(bool b_isChartPage) {
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
void RgaUtility::set_rga_addr(QString address) {
    rga_addr = address;
    QString addr = address.replace("http://", "");
    QStringList sl_addr = addr.split(":");
    rga_ip = sl_addr.at(0);
    rga_port = sl_addr.at(1);
}

///
/// \brief RgaUtility::getRgaAddr
/// \return
///
const QString RgaUtility::get_rga_addr() {
    return rga_addr;
}

///
/// \brief RgaUtility::setRgaTag
/// \param s_tag
///
void RgaUtility::set_rga_tag(QString s_tag) {
    rga_tag = s_tag;
}

///
/// \brief RgaUtility::getRgaTag
/// \return
///
const QString RgaUtility::get_rga_tag() {
    return rga_tag;
}

///
/// \brief RgaUtility::setRgaSn
/// \param rga_sn
///
void RgaUtility::set_rga_sn(const QString& sn) {
    rga_sn = sn;
}

///
/// \brief RgaUtility::getRgaSn
/// \return
///
const QString RgaUtility::get_rga_sn() {
    return rga_sn;
}

///
/// \brief RgaUtility::setScanTmTotal
/// \param value
///
void RgaUtility::set_scan_tm_total(double value) {
    m_scanData.d_scanTmTotal = value;
}

///
/// \brief RgaUtility::getScanTmTotal
/// \return
///
const double RgaUtility::get_scan_tm_total() {
    return m_scanData.d_scanTmTotal;
}

///
/// \brief RgaUtility::setInCtrl
/// \param rga_inCtrl
///
void RgaUtility::set_in_ctrl(bool rga_inCtrl) {
    m_stat.b_isInCtrl = rga_inCtrl;
}

///
/// \brief RgaUtility::getInCtrl
/// \return
///
const bool RgaUtility::get_in_ctrl() {
    return m_stat.b_isInCtrl;
}

///
/// \brief RgaUtility::setScanTS
/// \param i_tmStmp
///
void RgaUtility::set_scan_tmstmp(qint64 i_tmStmp) {
    m_scanData.i_scanTS = i_tmStmp;
}

///
/// \brief RgaUtility::getScanTS
/// \return
///
const qint64 RgaUtility::get_scan_tmstmp() {
    return m_scanData.i_scanTS;
}

///
/// \brief RgaUtility::setLocalTS
/// \param i_tmStmp
///
void RgaUtility::set_local_tmstmp(qint64 i_tmStmp) {
    m_scanData.i_scanTS = i_tmStmp;
}

///
/// \brief RgaUtility::setScanNum
/// \param scan_num
///
void RgaUtility::set_scan_num(int scan_num) {
    m_scanData.i_scanNum = scan_num;
}

///
/// \brief RgaUtility::getScanNum
/// \return
///
const int RgaUtility::get_scan_num() {
    return m_scanData.i_scanNum;
}

///
/// \brief RgaUtility::setEmisCurnt
/// \param value
///
void RgaUtility::set_emis_curnt(double value) {
    m_scanData.d_emisCurnt = value;
}

///
/// \brief RgaUtility::getEmisCurnt
/// \return
///
const double RgaUtility::get_emis_curnt() {
    return m_scanData.d_emisCurnt;
}

///
/// \brief RgaUtility::setTotalPressure
/// \param value
///
void RgaUtility::set_total_pres(double value) {
    m_scanData.d_tPress = value;
}

///
/// \brief RgaUtility::getTotalPressure
/// \return
///
const double RgaUtility::get_total_pres() {
    return m_scanData.d_tPress;
}

///
/// \brief RgaUtility::setBaseLine
/// \param value
///
void RgaUtility::set_baseline(double value) {
    m_scanData.d_baseLine = value;
}

///
/// \brief RgaUtility::getBaseLine
/// \return
///
const double RgaUtility::get_baseline() {
    return m_scanData.d_baseLine;
}

///
/// \brief RgaUtility::setTotalPressureQuad
/// \param value
///
void RgaUtility::set_total_pres_quad(double value) {
    m_scanData.d_tpQuad = value;
}

///
/// \brief RgaUtility::getTPressureQuad
/// \return
///
const double RgaUtility::get_total_pres_quad() {
    return m_scanData.d_tpQuad;
}

///
/// \brief RgaUtility::setTpDectCurnt
/// \param value
///
void RgaUtility::set_total_pres_curnt(double value) {
    m_scanData.d_tpDectCurnt = value;
}

///
/// \brief RgaUtility::getTpDectCurnt
/// \return
///
const double RgaUtility::get_total_pres_curnt() {
    return m_scanData.d_tpDectCurnt;
}

///
/// \brief RgaUtility::setAnodePotxl
/// \param value
///
void RgaUtility::set_anode_potxl(double value) {
    m_scanData.d_anodePotxl = value;
}

///
/// \brief RgaUtility::getAnodePotxl
/// \return
///
const double RgaUtility::set_anode_potxl() {
    return m_scanData.d_anodePotxl;
}

///
/// \brief RgaUtility::setFocusPotxl
/// \param value
///
void RgaUtility::set_focus_potxl(double value) {
    m_scanData.d_focusPotxl = value;
}

///
/// \brief RgaUtility::getFocusPotxl
/// \return
///
const double RgaUtility::get_focus_potxl() {
    return m_scanData.d_focusPotxl;
}

///
/// \brief RgaUtility::setEltrEnrg
/// \param value
///
void RgaUtility::set_elctr_enrg(double value) {
    m_scanData.d_eEnrg = value;
}

///
/// \brief RgaUtility::getEltrEnrg
/// \return
///
const double RgaUtility::get_elctr_enrg() const {
    return m_scanData.d_eEnrg;
}

///
/// \brief RgaUtility::setFlmtCurnt
/// \param value
///
void RgaUtility::set_flmt_curnt(double value) {
    m_scanData.d_flmtCurnt = value;
}

///
/// \brief RgaUtility::getFlmtCurnt
/// \return
///
const double RgaUtility::get_flmt_curnt() {
    return m_scanData.d_flmtCurnt;
}

///
/// \brief RgaUtility::setScanSize
/// \param scan_size
///
void RgaUtility::set_scan_size(int scan_size) {
    m_scanData.i_scanSize = scan_size;
}

///
/// \brief RgaUtility::getScanSize
/// \return
///
const int RgaUtility::get_scan_size() const {
    return m_scanData.i_scanSize;
}

///
/// \brief RgaUtility::setRgaScanValues
/// \param values
///
void RgaUtility::set_scan_val(const QVector<double>& values) {
    m_scanData.vd_scanValues = values;
}

///
/// \brief RgaUtility::getScanValuesSL
/// \param data_index
/// \return
///
const QStringList RgaUtility::get_scan_sl_val(bool is_all) {
    if(m_scanData.vd_scanValues.count() < 1) {
        return {};
    }
    QVector<double> vd_values = get_scan_val(is_all);
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
const QVector<double> RgaUtility::get_scan_val(bool is_all) {
    if(m_scanData.vd_scanValues.count() < 1) {
        return {};
    }
    QVector<double> vd_values = m_scanData.vd_scanValues;
    int i_ppamu = get_ppamu();
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
void RgaUtility::set_err_list(const QJsonArray& err_list) {
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

void RgaUtility::reset_err_list() {
    m_errLog.resetErrList();
}

const int RgaUtility::get_err_cnt() {
    return m_errLog.i_errCount;
}

const QStringList RgaUtility::get_err_list() {
    return m_errLog.sl_errList;
}


///
/// \brief RgaUtility::resetStatus
///
void RgaUtility::reset_status() {
    m_stat.resetStatus();
}

///
/// \brief RgaUtility::setRgaStatus
/// \param status_code
///
void RgaUtility::set_status(double status_code) {
    m_stat.i_statusCode = (uint)status_code;
    add_runtm();
}

///
/// \brief RgaUtility::getRgaStatus
/// \param status
/// \return
///
const bool RgaUtility::get_status(SysStatusCode status) {
    return (m_stat.i_statusCode & (1 << (31 - static_cast<int>(status))));
}

const QString RgaUtility::get_str_status() {
    return "getRgaStatusString";
}

///
/// \brief RgaUtility::setScanData
/// \param rga_scanData
/// \param ch_heads
/// \param is_pp
/// \return
///
void RgaUtility::set_scan_data(const QJsonObject& rga_scanData) {
    int i_scanNum               = rga_scanData.value("scannum").toInt();
    reset_scan_data();
    //**********************************************************************************//
    //** scannum
    set_scan_num(i_scanNum);
    //**********************************************************************************//
    //** RealTm
    set_local_tmstmp(QDateTime::currentMSecsSinceEpoch());
    //**********************************************************************************//
    //** scansize
    int i_scanSize              = rga_scanData.value("scansize").toInt();
    set_scan_size(i_scanSize);
    //**********************************************************************************//
    //** scan values
    QJsonArray ja_scanVal   = rga_scanData.value("d_values").toArray();
    set_total_pres(ja_scanVal.first().toDouble());
    foreach (QJsonValue val, ja_scanVal) {
        m_scanData.vd_scanValues.append(val.toDouble());
    }
    set_is_new_data(true);
}

///
/// \brief RgaUtility::resetScanData
///
void RgaUtility::reset_scan_data() {
    m_scanData.resetData();
}

void RgaUtility::set_rga_ip(const QString& ip) {
    rga_ip = ip;
}

void RgaUtility::set_rga_port(const QString& port) {
    rga_port = port;
}

const QString RgaUtility::get_rga_ip() {
    return rga_ip;
}

const QString RgaUtility::get_rga_port() {
    return rga_port;
}

void RgaUtility::gen_rga_addr() {
    QString addr = "http://1%2%";
    rga_addr = addr.arg(rga_ip, rga_port == "" ? "" : ":" + rga_port);
}

void RgaUtility::set_is_new_data(bool is_new_data) {
    m_scanData.b_isNewData = is_new_data;
}

const bool RgaUtility::get_is_new_data() {
    return m_scanData.b_isNewData;
}
