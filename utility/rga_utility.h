#ifndef RGAUTILITY_H
#define RGAUTILITY_H

#include "rga_structs.h"
#include "http_command.h"


class RgaUtility {
  public:
    explicit RgaUtility(QString addr, RecipeSet recipe);
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
        "Addr",     //0
        "SN",       //1
        "Recipe",   //2
        "Filament", //3
        "EM",       //4
        "Dwell",    //5
        "RptUnit",  //6
        "Datalog",  //7
        "Error",    //8
        "RunTm",    //9
        "ScanTm",   //10
        "TotalPressure",    //11
        "ScanNum"   //12
    };

    QLabel*       m_lb              = nullptr;
    QFile*        data_file_ptr     = nullptr;

    //em manual operation
    bool          is_em_man         = false;
    //acquire set
    bool          acquire_set       = false;
    //if save data to file.
    bool          is_save_data      = false;
    //count for get last scan data
    int           scan_count        = 0;
    //label stars count interval
    double        stars_intvl       = 0;
    //low limit value for show
    double        scan_val_low      = 0;
    //local timestamp
    qint64        local_tmstamp     = 0;
    //rga acquire run time total
    qint64        recipe_run_tm     = 0;

    //spec chart data positions for analog scan
    QVector<double> spec_data_pos   = {};
    //spec chart x axis ticks
    QVector<double> spec_x_ticks    = {};
    //spec chart x axis labels
    QVector<QString> spec_x_labels  = {};

    //rga address. eg. http://192.168.1.100:80
    QString       rga_addr          = "";
    //rga tag.
    QString       rga_tag           = "";
    //rga electronic box serial number
    QString       rga_sn            = "";
    //data file name
    QString       data_file_name    = "";
    //data file headers. contains misc information like start time,ppamu, etc.
    QString       data_file_header  = "";
    //main widget label text.
    QString       lb_text           = "";
    //data in string format and store into file every ten scans or stop triggered.
    QString       string_data       = "";

    QString       flmt_idx;

    //HttpCommand string generate function pointer define
    typedef const QString (*cmdStrFPtr)(QString);
    //HttpCommand string generate function pointer array
    cmdStrFPtr    cmdStrArr[24];


  public:

    void resetAll();

    //*************************************************************************************//
    //**  em gain
    const double getEmGainVal();
    void setEmGainVal(double d_val);

    //*************************************************************************************//
    //**  fc sensitivity
    const double getFcSensVal();
    void setFcSensVal(double d_val);

    //*************************************************************************************//
    //**  ticks, data positions, labels
    const QVector<double> getVdTicks();
    const QVector<QString> getVsLabels();
    void genTicker();
    QVector<double> getDataPos() const;
    void setDataPos(const QVector<double>& new_dataPos);
    int getDataPointsCount();

    //*************************************************************************************//
    //**  is sweep mode
    const bool getIsSweep() const;

    //*************************************************************************************//
    //**  rga acquire state
    void setAcquireState(bool is_in_acq = false);
    const bool acquireState();

    //*************************************************************************************//
    //**  rga main widget label
    void setRgaLabel(QLabel* lb);
    QLabel* getRgaLabel();
    void setLbText(bool finish_scan = false);

    //*************************************************************************************//
    //**  data file operations
    void initDataFile(bool is_crateFile = true);
    void writeScanData(bool final = false);
    const QString genFileHeaders();
    const QString getFileName();
    void genDataFileName();

    //*************************************************************************************//
    //**  controll commands
    const QStringList getScanSet();
    const QStringList getIdlSet();
    const QStringList getCloseSet();
    const QStringList getStopSet();
    const QStringList getScanPos();
    void genIdlSet();
    void genScanSet();
    void genCloseSet();
    const QString genRgaAction(RgaActions action);

    //*************************************************************************************//
    //**  is save data
    void setIsSaveData(bool save_data = true);
    const bool getIsSaveData();

    //*************************************************************************************//
    //**  acquire count
    void setAcquireCnt(int count = 0);
    const int getAcqireCnt();


    //*************************************************************************************//
    //**  is analog scan
    bool getIsAScan() const;

    //*************************************************************************************//
    //**  over time
    int getOverTime();
    void resetOverTime();

    //*************************************************************************************//
    //**  run set, continious/one time
    const bool getRunSet();
    void setRunSet(const QString& s_run);

    //*************************************************************************************//
    //**  data table colums
    const QStringList getTblCol(bool b_isChartPage = false);

    //*************************************************************************************//
    //**  recipe
    const RecipeSet getRcpt();
    const QString getRcptInfo(RecipeInfo info_code);

    //*************************************************************************************//
    //**  pressure unit
    const QString getPrsUnit(bool b_isNum = false);

    //*************************************************************************************//
    //**  ppamu
    const int getPPAmu();

    //*************************************************************************************//
    //**  filament index
    void setFlmtIdx(double index);
    QString getFlmtIdx();

    //*************************************************************************************//
    //**  rga address
    void setRgaAddr(QString addr);
    const QString getRgaAddr();

    //*************************************************************************************//
    //**  rga tag
    void setRgaTag(QString s_tag);
    const QString getRgaTag();

    //*************************************************************************************//
    //**  rga serial number
    void setRgaSn(QString rga_sn);
    const QString getRgaSn();

    //*************************************************************************************//
    //**  scan time total for one
    void setScanTmTotal(double value = 0);
    const double getScanTmTotal();

    //*************************************************************************************//
    //**  rga in control
    void setInCtrl(bool rga_inCtrl);
    const bool getInCtrl();

    //*************************************************************************************//
    //**  scan timestamp
    void setScanTS(qint64 i_tmStmp);
    const qint64 getScanTS();

    //*************************************************************************************//
    //**  local timestamp
    void setLocalTS(qint64 i_tmStmp);
    const qint64 getLocalTS();

    //*************************************************************************************//
    //**  scan number
    void setScanNum(int scan_num);
    const int getScanNum();

    //*************************************************************************************//
    //**  emission current
    void setEmisCurnt(double value);
    const double getEmisCurnt();

    //*************************************************************************************//
    //**  total pressure
    void setTotalPressure(double value = 0);
    const double getTotalPressure();

    //*************************************************************************************//
    //**  base line
    void setBaseLine(double value);
    const double getBaseLine();

    //*************************************************************************************//
    //**  total pressure at quadro pole
    void setTotalPressureQuad(double value = 0);
    const double getTPressureQuad();

    //*************************************************************************************//
    //**  total pressure current
    void setTpDectCurnt(double value = 0);
    const double getTpDectCurnt() ;

    //*************************************************************************************//
    //**  anode potential
    void setAnodePotxl(double value);
    const double getAnodePotxl();

    //*************************************************************************************//
    //**  focus potential
    void setFocusPotxl(double value = 0);
    const double getFocusPotxl();

    //*************************************************************************************//
    //**  electron energe
    void setEltrEnrg(double value = 0);
    const double getEltrEnrg() const;

    //*************************************************************************************//
    //**  filament current
    void setFlmtCurnt(double value = 0);
    const double getFlmtCurnt();

    //*************************************************************************************//
    //**  scan size
    void setScanSize(int scan_size = 0);
    const int getScanSize() const;

    //*************************************************************************************//
    //**  scan values
    void setRgaScanValues(const QVector<double>& values);
    const QStringList getScanValuesSL(bool is_all = true);
    const QVector<double> getScanValues(bool is_all = false);

    //*************************************************************************************//
    //**  error list
    void setErrLog(const QJsonArray& err_list);
    void resetErrList();
    const int getErrCount();
    const QStringList getErrList();

    //*************************************************************************************//
    //**  status
    void resetStatus();
    void setRgaStatus(double status_code);
    const bool getRgaStatus(SysStatusCode status = EmissState);
    const QString getRgaStatusString();

    //*************************************************************************************//
    //**  scan data
    void setScanData(const QJsonObject& rga_scanData);
    void resetScanData();
    void setNewData(bool is_new_data = false);
    const bool getIsNewData();

    //*************************************************************************************//
    //**  run time
    QString getRunTm() const;
    void addRunTm();

    //*************************************************************************************//
    //**  limit value
    double dLimitVal() const;
    void setDLimitVal(double newDLimitVal);

    //*************************************************************************************//
    //**  acquire set
    bool acquireSet() const;
    void setAcquireSet(bool newb_acq_set);

    QString flmtIdxSet() const;
    void setFlmtIdxSet(const QString& news_flmtIdx);
    bool emManual() const;
    void setEmManual(bool newbEmManual);
};

#endif // RGAUTILITY_H
