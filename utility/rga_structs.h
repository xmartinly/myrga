#ifndef RGA_STRUCTS_H
#define RGA_STRUCTS_H

///
/// \brief The RgaSysStatus struct
///
struct SysStatus {
    //current filament index
    QString         s_curntFlmt     = "1";
    //system status code in string.
    QString         s_statusCode    = "";
    //system status code in unsigned int
    uint            i_statusCode    = 0;
    //is the rga in acquire state
    bool            b_isInAcquire   = false;
    //is the rga in controll state
    bool            b_isInCtrl      = false;
    //faraday cup sensitivity
    double          d_fcSens        = 0;
    //electron multiple gain
    double          d_emGain        = 0;

    void resetStatus() {
        s_statusCode    = "";
        s_curntFlmt     = "1";
        i_statusCode    = 0;
        d_fcSens        = 0;
        d_emGain        = 0;
        b_isInAcquire   = false;
        b_isInCtrl      = false;
    }
};
Q_DECLARE_METATYPE(SysStatus)

///
/// \brief The RgaErrorLog struct
///
struct ErrorLog {
    //errors count
    int             i_errCount      = 0;
    //errors string list
    QStringList     sl_errList      = {""};

    void resetErrList() {
        i_errCount      = 0;
        sl_errList.clear();
    }
};
Q_DECLARE_METATYPE(ErrorLog)


///
/// \brief The RgaScanData struct
///
struct ScanData {
    //scan number, add one for every completed scan
    int             i_scanNum       = 0;
    //scan data size. including total pressure, timestamp, etc..
    int             i_scanSize      = 0;
    //scan data timestamp
    qint64          i_scanTS        = 0;
    //scan time total for one completed scan
    double          d_scanTmTotal   = 0;
    //total pressure
    double          d_tPress        = 0;
    //base line
    double          d_baseLine      = 0;
    //total pressure at quadro pole
    double          d_tpQuad        = 0;
    //total pressure at dector current
    double          d_tpDectCurnt   = 0;
    //emission current
    double          d_emisCurnt     = 0;
    //anode potential
    double          d_anodePotxl    = 0;
    //ion source electron energe
    double          d_eEnrg         = 0;
    //ion source focus potential
    double          d_focusPotxl    = 0;
    //current filament current
    double          d_flmtCurnt     = 0;
    //if the data is new
    bool            b_isNewData     = false;
    //scan values
    QVector<double> vd_scanValues   = {};

    void resetData() {
        i_scanNum       = 0;
        i_scanSize      = 0;
        i_scanTS        = 0;
        d_tPress        = 0;
        d_baseLine      = 0;
        d_tpQuad        = 0;
        d_tpDectCurnt   = 0;
        d_emisCurnt     = 0;
        d_anodePotxl    = 0;
        d_eEnrg         = 0;
        d_focusPotxl    = 0;
        d_flmtCurnt     = 0;
        b_isNewData     = false;
        vd_scanValues.clear();
    }
};
Q_DECLARE_METATYPE(ScanData)

///
/// \brief The RecipeSet struct
///
struct RecipeSet {
    //scan period
    qint64          i_period        = 0;
    //0: one time; 1: continuous
    QString         s_run           = "1";
    //scan method
    QString         s_method        = "Sweep";
    //scan dwell
    QString         s_dwell         = "16";
    //report unit. Current, PP, PPM
    QString         s_rUnit         = "Current";
    //total pressure unit. 0: Torr; 1: mBar; 2: Pa
    QString         s_pUnit         = "0";
    //em auto operation. 0: manual; 1: auto on
    QString         s_emOpt         = "0";
    //scan ppamu set
    QString         s_ppamu         = "1";
    //recipe name
    QString         s_rcpName       = "temp";
    //filament index set.1/2
    QString         s_flmtIdx       = "1";
    //scan points position when using points
    QStringList     sl_points       = {};
    //scan mass begin when using sweep
    QString         s_startMass     = "0";
    //scan mass stop when using sweep
    QString         s_stopMass      = "100";

    void resetRcp() {
        i_period        = 0;
        s_run           = "0";
        s_method        = "Sweep";
        s_dwell         = "16";
        s_startMass     = "0";
        s_stopMass      = "100";
        s_rUnit         = "Current";
        s_pUnit         = "0";
        s_emOpt         = "0";
        s_ppamu         = "1";
        s_flmtIdx       = "1";
        s_rcpName       = "temp";
        sl_points.clear();
    }
};
Q_DECLARE_METATYPE(RecipeSet)



#endif // RGA_STRUCTS_H
