#ifndef RGAUTILITY_H
#define RGAUTILITY_H

#include "rga_structs.h"
#include "http_command.h"


class RgaUtility {
  public:
    explicit RgaUtility(QString addr, RecipeSet recipe);
    RgaUtility();
    ~RgaUtility();

    ///
    /// \brief The ScanHeadChSet enum
    ///
    enum ScanHeadChSet {
        Timestamp               = 0,
        EmissionCurrent         = 1,
        AnodePotential          = 2,
        ElectronEnergy          = 3,
        FocusPotential          = 4,
        FilamentCurrent         = 5,
        Baseline                = 6,
        TotalPressure           = 7,
        TPQuad                  = 8,
        TPDetectorCurrent       = 9,
    };

    enum RecipeInfo {
        RcptName                = 0,
        RcptDwell               = 1,
        RcptRptUnit             = 2,
        RcptTPUnit              = 3,
    };

    ///
    /// \brief The RgaActions enum
    ///
    enum RgaActions {
        NoneAct                 = 0,
        ForceCtrl               = 1,
        AmInCtrl                = 2,
        RleaseCtrl              = 3,
        StopAcquire             = 4,
        StartAcquire            = 5,
        OpenFlmt                = 6,
        CloseFlmt               = 7,
        OpenEm                  = 8,
        CloseEm                 = 9,
        GetSn                   = 10,
        GetSysStatus            = 11,
        GetErrLog               = 12,
        GetLastScan             = 13,
        Reboot                  = 14,
        GetIssueLog             = 15,
        GetCrntFlmt             = 16,
        SetFlmt1st              = 17,
        SetFlmt2nd              = 18,
        SetMeasBegin            = 19,
        GetScanTm               = 20,
        SetScanCnt              = 21,
        GetEmGainReq            = 22,
        GetFcSensReq            = 23,
    };

    ///
    /// \brief The SysStatus enum
    ///
    enum SysStatusCode {
        EmissState              = 0,       ///< Emission regulator, regulated current mode Off/On. 0/1
        EmissPwr                = 1,       ///< Emission regulator, constant power mode Off/On. 0/1
        EmissDegas              = 2,       ///< Emission regulator, degas mode Off/On. 0/1
        EmissPd                 = 3,       ///< Emission regulator, request pending No/Yes. 0/1
        EmissInterLk            = 4,       ///< Emission interlock active No/Yes. 0/1
        SelectedFlmt            = 5,       ///< Selected filament is 1/2. 0/1
        RFGenerator             = 6,       ///< RF generator Off/On. 0/1
        RodPolarity             = 7,       ///< Rod polarity Normal/Reversed. 0/1
        EMState                 = 8,       ///< Electron multiplier Off/On. 0/1
        EMPd                    = 9,       ///< Electron multiplier request pending No/Yes. 0/1
        EMInterLk               = 10,      ///< Electron multiplier interlock active No/Yes. 0/1
        IonEnergyInUse          = 11,      ///< Global ion energy in use No/Yes. 0/1
        PeakTopWiden            = 13,      ///< Peak Top Widen Off/On. 0/1
        DataSmoothing           = 14,      ///< Data Smoothing Off/On. 0/1
        StatusRelay             = 15,      ///< Status Relay Off/On. 0/1
        ElectMeterTempComp      = 16,      ///< Electrometer temperature compensation Off/On. 0/1
        TPLinear                = 17,      ///< Total pressure based partial pressure linearization Off/On. 0/1
        PeakPosSchd             = 18,      ///< Peak position measurement scheduled No/Yes. 0/1
        PeakPosCor              = 19,      ///< Peak position correction Off/On. 0/1
        BaselineCtSch           = 20,      ///< Baseline continuous measurement scheduled No/Yes. 0/1
        BaselineSgSch           = 21,      ///< Baseline single measurement scheduled Off/On. 0/1
        BaselineCorr            = 22,      ///< Baseline correction Off/On. 0/1
        RFGenHiPwr              = 23,      ///< RF generator high power re-resonate Off/On. 0/1
        RFFreqCompen            = 24,      ///< RF frequency change compensation Off/On. 0/1
        RFBdOfst                = 25,      ///< RF board gain and offset compensation Off/On. 0/1
        EMEquCurrent            = 26,      ///< EM equivalent ion current Off/On. 0/1
        IonSrcNor               = 27,      ///< Ion source normalization Off/On. 0/1
        RFBdTempCompen          = 28,      ///< RF board temperature compensate setpointsOff/On. 0/1
        RFGenAwyReson           = 29,      ///< RF generator always re-resonate Off/On. 0/1
        VersAct                 = 30,      ///< Versiscan Active No/Yes. 0/1
        ResetOccu               = 31,      ///< Reset occured since last query No/Yes. 0/1
    };

  private:
    //error list
    ErrorLog      m_errLog;
    ScanData      m_scanData;
    RecipeSet     m_rcpt;
    SysStatus     m_stat;
    DataHelper    m_helper;
    HttpCommand   m_cmd;

    //set of strings for initialize a scan
    QStringList   m_scanSet;
    //set of strings for idle state
    QStringList   m_idlSet;
    //set of string for closing the rga
    QStringList   m_closeSet;
    //table headers
    QStringList   m_mainTblHead = {
        u8"Addr",     //0
        u8"SN",       //1
        u8"Recipe",   //2
        u8"Filament", //3
        u8"EM",       //4
        u8"Dwell",    //5
        u8"RptUnit",  //6
        u8"Datalog",  //7
        u8"Error",    //8
        u8"RunTm",    //9
        u8"ScanTm",   //10
        u8"TotalPressure",    //11
        u8"ScanNum"   //12
    };

    QFile*        data_file_ptr     = nullptr;
    //if save data to file.
    bool          is_save_data      = true;
    //count for get last scan data
    int           scan_count        = 0;
    //low limit value for show
    double        scan_val_low      = 0;
    //local timestamp
    qint64        local_tmstamp     = 0;
    //scan timestamp
    qint64        start_tmstamp     = 0;
    //rga acquire run time total
    qint64        recipe_run_tm     = 0;
    //spec chart data positions for analog scan
    QVector<double>  spec_data_pos  = {};
    //spec chart x axis ticks
    QVector<double>  spec_x_ticks   = {};
    //spec chart x axis labels
    QVector<QString> spec_x_labels  = {};
    //rga address. eg. http://192.168.1.100:80
    QString          rga_addr          = "";
    //rga tag.
    QString          rga_tag        = "";
    //rga electronic box serial number
    QString          rga_sn         = "";
    //data file name
    QString          data_file_name = "";
    //data in string format and store into file every ten scans or stop triggered.
    QString          string_data    = "";
    //rga ip and port
    QString          rga_ip = "";
    QString          rga_port = "";
    //HttpCommand string generate function pointer define
    typedef const QString (*cmd_str_func_ptr)(const QString&);
    //HttpCommand string generate function pointer array
    cmd_str_func_ptr cmd_str_arr[24];

  public:
    void set_scan_rcpt(const RecipeSet& rcpt);
    void reset_all();
    void setup_func_ptrs();
    void reset_scan_data();

    void set_rga_ip(const QString& ip);
    void set_rga_port(const QString& port);
    const QString get_rga_ip();
    const QString get_rga_port();
    void gen_rga_addr();


    //*************************************************************************************//
    //**  em gain
    const double get_em_gain_val();
    void set_em_gain_val(double d_val);

    //*************************************************************************************//
    //**  fc sensitivity
    const double get_fc_sens_val();
    void set_fc_sens_val(double d_val);

    //*************************************************************************************//
    //**  ticks, data positions, labels
    const QVector<double> get_vd_ticks();
    const QVector<QString> get_vs_labels(const QList<int> pos = {});
    void gen_ticker();
    QVector<double> get_data_pos() const;
    void set_data_pos(const QVector<double>& new_dataPos);
    int get_data_points_cnt();

    //*************************************************************************************//
    //**  rga acquire state
    void set_acquire_state(bool is_in_acq = false);
    const bool get_acquire_state();

    //*************************************************************************************//
    //**  rga main widget label
    void set_rga_label(QLabel* lb);
    QLabel* get_rga_label();
    void set_label_text(bool finish_scan = false);

    //*************************************************************************************//
    //**  data file operations
    void init_data_file(bool is_crateFile = true);
    void write_scan_data(bool final = false);
    const QString get_file_name();


    //*************************************************************************************//
    //**  controll commands
    const QStringList get_scan_set();
    const QStringList get_idle_set();
    const QStringList get_close_set();
    const QStringList get_stop_set();
    const QStringList get_scan_pos();
    void gen_idle_set();
    void gen_scan_set();
    void gen_close_set();
    const QString gen_rga_action(RgaActions action);

    //*************************************************************************************//
    //**  is save data
    void set_is_save_data(bool save_data = true);
    const bool get_is_save_data();

    //*************************************************************************************//
    //**  acquire count
    void set_acquier_cnt(int count = 0);
    const int get_acquire_cnt();

    //*************************************************************************************//
    //**  is analog scan
    bool get_is_alg_scan() const;

    //*************************************************************************************//
    //**  over time
    int get_over_tm();
    void reset_over_tm();

    //*************************************************************************************//
    //**  run set, continious/one time
    const bool get_run_set();
    void set_run_set(const QString& s_run);
    void set_run_set(int i_run);

    //*************************************************************************************//
    //**  data table colums
    const QStringList get_tbl_col(bool b_isChartPage = false);

    //*************************************************************************************//
    //**  recipe
    const QString get_rcpt_info(RecipeInfo info_code);

    //*************************************************************************************//
    //**  pressure unit
    const QString get_pres_unit(bool b_isNum = false);

    //*************************************************************************************//
    //**  ppamu
    const int get_ppamu();

    //*************************************************************************************//
    //**  filament index
    void set_flmt_idx(int flmt_idx);
    QString get_flmt_idx();
    const QString get_flmt_setted();

    //*************************************************************************************//
    //**  rga address
    void set_rga_addr(QString addr);
    const QString get_rga_addr();

    //*************************************************************************************//
    //**  rga tag
    void set_rga_tag(QString s_tag);
    const QString get_rga_tag();

    //*************************************************************************************//
    //**  rga serial number
    void set_rga_sn(const QString& sn);
    const QString get_rga_sn();

    //*************************************************************************************//
    //**  scan time total for one
    void set_scan_tm_total(double value = 0);
    const double get_scan_tm_total();

    //*************************************************************************************//
    //**  rga in control
    void set_in_ctrl(bool rga_inCtrl);
    const bool get_in_ctrl();

    //*************************************************************************************//
    //**  local timestamp
    void set_local_tmstmp(qint64 i_tmStmp);

    //*************************************************************************************//
    //**  scan number
    void set_scan_num(int scan_num);
    const int get_scan_num();

    //*************************************************************************************//
    //**  emission current
    void set_emis_curnt(double value);
    const double get_emis_curnt();

    //*************************************************************************************//
    //**  total pressure
    void set_total_pres(double value = 0);
    const double get_total_pres();

    //*************************************************************************************//
    //**  base line
    void set_baseline(double value);
    const double get_baseline();

    //*************************************************************************************//
    //**  total pressure at quadro pole
    void set_total_pres_quad(double value = 0);
    const double get_total_pres_quad();

    //*************************************************************************************//
    //**  total pressure current
    void set_total_pres_curnt(double value = 0);
    const double get_total_pres_curnt() ;

    //*************************************************************************************//
    //**  anode potential
    void set_anode_potxl(double value);
    const double set_anode_potxl();

    //*************************************************************************************//
    //**  focus potential
    void set_focus_potxl(double value = 0);
    const double get_focus_potxl();

    //*************************************************************************************//
    //**  electron energe
    void set_elctr_enrg(double value = 0);
    const double get_elctr_enrg() const;

    //*************************************************************************************//
    //**  scan values
    const QStringList get_scan_sl_val(bool is_all = true);
    const QVector<double> get_scan_val(bool is_all = false);

    //*************************************************************************************//
    //**  error list
    void set_err_list(const QJsonArray& err_list);
    const int get_err_cnt();
    const QStringList get_err_list();

    //*************************************************************************************//
    //**  status
    void set_status(double status_code);
    const bool get_status(SysStatusCode status = EmissState);
    const QString get_str_status();

    //*************************************************************************************//
    //**  scan data
    void set_scan_data(const QJsonObject& rga_scanData);
    const bool get_is_new_data();
    //*************************************************************************************//
    //**  run time
    QString get_str_runtm() const;

    bool get_em_auto() const;

    void set_em_auto(int em_auto);

  private:
    void set_scan_val(const QVector<double>& values);
    void reset_err_list();
    void reset_status();
    void set_is_new_data(bool is_new_data = false);
    const QString gen_file_header();
    void gen_file_name();
    void add_runtm();
    //*************************************************************************************//
    //**  is sweep mode
    const bool get_is_sweep() const;

    //*************************************************************************************//
    //**  limit value
    double get_limit_val() const;
    void set_limit_val(double new_limit);

    //*************************************************************************************//
    //**  scan size
    void set_scan_size(int scan_size = 0);
    const int get_scan_size() const;

    //*************************************************************************************//
    //**  filament current
    void set_flmt_curnt(double value = 0);
    const double get_flmt_curnt();

    //*************************************************************************************//
    //**  scan timestamp
    void set_scan_tmstmp(qint64 i_tmStmp);
    const qint64 get_scan_tmstmp();
};

#endif // RGAUTILITY_H
