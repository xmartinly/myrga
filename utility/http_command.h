#ifndef HTTP_COMMAND_H
#define HTTP_COMMAND_H

class HttpCommand {
  public:
    HttpCommand() {};
    ~HttpCommand() {};

  private:
    const QStringList m_slChSets = {"Timestamp",
                                    "EmissionCurrent",
                                    "AnodePotential",
                                    "ElectronEnergy",
                                    "FocusPotential",
                                    "FilamentCurrent",
                                    "Baseline",
                                    "TotalPressure",
                                    "TPQuad",
                                    "TotalPressureDetectorCurrent"
                                   };

  public:
    const QStringList setHeadScan(QString rga_addr, const QList<int>& scan_head_ch_list) {
        QStringList sl_cmds;
        QString s_addrTemp = rga_addr + "/mmsp/scanSetup/channels/%1/set?channelMode=%2&dwell=%3&enabled=True";
        int i_chNo = 1;
        foreach (int i_ch, scan_head_ch_list) {
            QString s_chNo      = QString::number(i_chNo);
            QString s_chMode    = m_slChSets.at(i_ch);
            sl_cmds.append(QString(s_addrTemp).arg(s_chNo, s_chMode, "8"));
            ++i_chNo;
        }
        return sl_cmds;
    }

    const QStringList setupScanChs(QString rga_addr,
                                   QString start_mass,
                                   QString stop_mass,
                                   QString ppamu,
                                   QString report_unit,
                                   QString ch_mod,
                                   QString dwell,
                                   const QStringList& points,
                                   const QList<int>& head_set) {
        QStringList sl_cmds         = setHeadScan(rga_addr, head_set);
        int         i_chHeadCount   = sl_cmds.count();
        int         i_pointsCount   = points.count();
        bool        b_isSweep       = ch_mod == "Sweep";
        QString     s_scanCmd       = rga_addr + "/mmsp/scanSetup/channels/%1/set?channelMode=%2&enabled=True&%3%4&ppamu=%5&dwell=%6&ReportUnits=%7";
        //**********************************************************************************//
        //** Sweep
        if(b_isSweep) {
            sl_cmds.append(QString(s_scanCmd).arg(
                               QString::number(++i_chHeadCount),
                               ch_mod,
                               "startMass=" + start_mass + "&",
                               "stopMass=" + stop_mass,
                               ppamu,
                               dwell,
                               report_unit
                           ));
            return sl_cmds;
        }
        ch_mod = "Single";
        //**********************************************************************************//
        //** Single
        for (int var = 0; var < i_pointsCount; ++var) {
            ppamu = "1";
            sl_cmds.append(QString(s_scanCmd).arg(
                               QString::number(++i_chHeadCount),
                               ch_mod,
                               "startMass=" + points.at(var),
                               "",
                               ppamu,
                               dwell,
                               report_unit
                           ));
        }
        return sl_cmds;
    }

    ///
    /// \brief noAction
    /// \param rga_addr
    /// \return
    ///
    static const QString noAction(QString rga_addr) {
        return rga_addr.replace("http://", "");
    }

    ///
    /// \brief setCh1. TimeStamp
    /// This target set the channel 1 for timestamp reading in the sensor.
    /// The scan must be started to get a timestamp reading.
    /// \return QString.
    ///
    static const QString setCh1Tm(QString rga_addr) {
        return rga_addr + "/mmsp/scanSetup/channels/1/set?channelMode=Timestamp&enabled=True";
    }

    ///
    /// \brief setReboot. Reboot system.
    /// \return QString.
    ///
    static const QString setReboot(QString rga_addr) {
        return rga_addr + "/mmsp/system/set?bootMonitor=1";
    }

    ///
    /// \brief setFlmtOn
    /// \param rga_addr
    /// \return
    ///
    static const QString setFlmtOn(QString rga_addr) {
        return rga_addr + "/mmsp/generalControl/set?setEmission=on";
    }

    ///
    /// \brief setFlmtOff
    /// \param rga_addr
    /// \return
    ///
    static const QString setFlmtOff(QString rga_addr) {
        return rga_addr + "/mmsp/generalControl/set?setEmission=off";
    }

    ///
    /// \brief setStop.
    /// Stops scanning immediately.
    /// \return
    ///
    static const QString setScanStop(QString rga_addr) {
        return rga_addr + "/mmsp/scanSetup/set?scanStop=Immediately";
    }

    ///
    /// \brief setTpUnits.
    /// Set the ion source total pressure reporting units.
    /// 0=Torr, 1=mbar, 2=Pascal.
    /// \return
    ///
    static const QString setTpUnits(QString rga_addr, QString s_unitCode = "2") {
        return rga_addr + "/mmsp/sensorIonSource/set?tPunits=" + s_unitCode;
    }

    ///
    /// \brief setScanIntvl.
    /// Set or Get the interval between the start of scans in milliseconds.
    /// When set to zero, scans will occur head to tail with no delay between.
    /// When set to a value less than ScanTimeTotal, the interval will be adjusted to zero.
    /// When set to a value greater than ScanTimeTotal and less than ScanTimeTotal+3ms,
    /// the interval will be adjusted to ScanTimeTotal+3ms.
    /// Range of values 0, 5-1000000000 milliseconds.
    /// \param intvl
    /// \return
    ///
    static const QString setScanIntvl(QString rga_addr, QString s_intvl = "0") {
        return rga_addr + "/mmsp/scanSetup/set?scanInterval=" + s_intvl;
    }

    ///
    /// \brief setScanCnt
    /// \param count
    /// \return
    ///
    static const QString setScanCnt(QString rga_addr) {
        return rga_addr + "/mmsp/scanSetup/set?scanCount=-1";
    }

    ///
    /// \brief setScanStart
    /// \param start
    /// \return
    ///
    static const QString setScanStart(QString rga_addr) {
        return rga_addr + "/mmsp/scanSetup/set?scanStart=1";
    }

    ///
    /// \brief scanStart
    /// \return
    ///
    static const QString setMeasBegin(QString rga_addr) {
        return rga_addr + "/mmsp/measurement/data/get";
    }

    ///
    /// \brief setChannelsRange
    /// This target queries or updates the start and stopping channel (inclusive) of the Versatile scan.
    /// For a channel to be used, it must fall within the startChannel to stopChannel range AND it must be enabled.
    /// \param stop_chan(QString), default 0.
    /// \return
    ///
    static const QString setChannelsRange(QString rga_addr, QString s_chCnt = "12") {
        return rga_addr + "/mmsp/scanSetup/set?startChannel=1&stopChannel=" + s_chCnt;
    }

    ///
    /// \brief setReportUnits. Set Report unit. Default is current.
    /// \param report_current(bool).
    /// \param ch_no(int).
    /// \return
    ///
    static const QString setReportUnits(QString rga_addr, bool report_current, int ch_no = 0) {
        if (!ch_no) {
            return "";
        }
        QString s_unit  = report_current ? "Current" : "PP";
        QString s_ch_no = QString::number(ch_no);
        return rga_addr + QString("/mmsp/scanSetup/channels/%1/set?ReportUnits=2%").arg(s_ch_no, s_unit);
    }

    ///
    /// \brief setEmOn
    /// \param rga_addr
    /// \return
    ///
    static const QString setEmOn(QString rga_addr) {
        return rga_addr + "/mmsp/generalControl/set?setEM=on" ;
    }

    ///
    /// \brief setEmOff
    /// \param rga_addr
    /// \return
    ///
    static const QString setEmOff(QString rga_addr) {
        return rga_addr + "/mmsp/generalControl/set?setEM=off" ;
    }

    ///
    /// \brief setReleaseCtrl.
    /// Releases rga control.
    /// \return
    ///
    static const QString setReleaseCtrl(QString rga_addr) {
        return rga_addr + "/mmsp/communication/control/release";
    }

    ///
    /// \brief setForceCtrl.
    /// Forcibly takes control even if another client has control.
    /// Clients cannot force control from a higher privileged client.
    /// \return
    ///
    static const QString setForceCtrl(QString rga_addr) {
        return rga_addr + "/mmsp/communication/control/set?force";
    }

    ///
    /// \brief getAmInCtrl.
    /// Get current session is in control.
    /// \return
    ///
    static const QString getAmInCtrl(QString rga_addr) {
        return rga_addr + "/mmsp/communication/amInControl/get";
    }

    ///
    /// \brief getCtrlInfo.
    /// Get current Ctrl Info.
    /// \return
    ///
    static const QString getCtrlInfo(QString rga_addr) {
        return rga_addr + "/mmsp/communication/controlInfo/get";
    }

    ///
    /// \brief getErrLog.
    /// Get the current error log.
    /// \return
    ///
    static const QString getErrLog(QString rga_addr) {
        return rga_addr + "/mmsp/communication/errorLog/get";
    }

    ///
    /// \brief getIssueLog.
    /// Get the current Issue log.
    /// \return
    ///
    static const QString getIssueLog(QString rga_addr) {
        return rga_addr + "/mmsp/communication/issueLog/get";
    }

    ///
    /// \brief getSysStatus.
    /// The System Status gives a single message status of the primary MMSP functions.
    /// \return
    ///
    static const QString getSysStatus(QString rga_addr) {
        return rga_addr + "/mmsp/status/systemStatus/get";
    }

    ///
    /// \brief getDectType.
    /// Enumerated detector type: 0=FC, 1=CDEM, 2=MCP, 3=366B
    /// \return
    ///
    static const QString getDectType(QString rga_addr) {
        return rga_addr + "/mmsp/sensorInfo/detectorType/get";
    }

    ///
    /// \brief getInputState.
    /// \return
    ///
    static const QString getInputState(QString rga_addr) {
        return rga_addr + "/mmsp/digitalInput/ioChannel/get";
    }

    ///
    /// \brief get Sn.
    /// This target returns a 8 digit numeric string containing the Serial Number found on the MMSP label.
    /// \return
    ///
    static const QString getSn(QString rga_addr) {
        return rga_addr + "/mmsp/electronicsInfo/serialNumber/get";
    }

    ///
    /// \brief getScanData.
    /// This target is the primary means to acquire data taken using the Versatile Scan engine.
    /// \return
    ///
    static const QString getLastScan(QString rga_addr) {
        return rga_addr + "/mmsp/measurement/scansPow2/-1/get";
    }

    ///
    /// \brief getLastScanDouble
    /// \param rga_addr
    /// \return
    ///
    static const QString getLastScanDouble(QString rga_addr) {
        return rga_addr + "/mmsp/measurement/scans/-1/get";
    }
    ///
    /// \brief getLastScanBin
    /// \param rga_addr
    /// \return
    ///
    static const QString getLastScanBin(QString rga_addr) {
        return rga_addr + "/mmsp/measurement/binaryScans/-1/get";
    }

    ///
    /// \brief getHardwareError.
    /// The hardware errors message includes the primary Linxon errors.
    /// \return
    ///
    static const QString getHardwareError(QString rga_addr) {
        return rga_addr + "/mmsp/status/hardwareErrors/get";
    }

    ///
    /// \brief getEmGain
    /// \param rga_addr
    /// \return
    ///
    static const QString getEmGainReq(QString rga_addr) {
        return rga_addr + "/mmsp/sensorDetector/emGain/get";
    }

    ///
    /// \brief getEmGain
    /// \param rga_addr
    /// \return
    ///
    static const QString getFcSensReq(QString rga_addr) {
        return rga_addr + "/mmsp/sensorIonSource/ppSensitivityFactor/get";
    }

    ///
    /// \brief getCurrentFlmts.
    /// Get the currently selected filament.
    /// \return
    ///
    static const QString getCurrentFlmts(QString rga_addr) {
        return rga_addr + "/mmsp/sensorIonSource/filamentSelected/get";
    }

    ///
    /// \brief setCurrentFlmtOne
    /// \param rga_addr
    /// \return
    ///
    static const QString setFlmt1st(QString rga_addr) {
        return rga_addr + "/mmsp/sensorIonSource/set?filamentSelected=1";
    }

    ///
    /// \brief setCurrentFlmtTwo
    /// \param rga_addr
    /// \return
    ///
    static const QString setFlmt2nd(QString rga_addr) {
        return rga_addr + "/mmsp/sensorIonSource/set?filamentSelected=2";
    }

    ///
    /// \brief setCurrentFlmtTwo
    /// \param rga_addr
    /// \return
    ///
    static const QString setFlmtIdx(QString rga_addr, QString idx) {
        return rga_addr + "/mmsp/sensorIonSource/set?filamentSelected=" + idx;
    }

    ///
    /// \brief getMassRange.
    /// Range in AMU e.g. 100, 200, 300
    /// \return
    ///
    static const QString getMassRange(QString rga_addr) {
        return rga_addr + "/mmsp/electronicsInfo/massRange/get";
    }

    ///
    /// \brief getTotalTm.
    /// Average time required to complete any scan subsequent to the first scan after a start is issued.
    /// \return
    ///
    static const QString getTotalTm(QString rga_addr) {
        return rga_addr + "/mmsp/scanSetup/scanTimeTotal/get";
    }

    static const QString setThrehold(const QString& rga_addr, const QString& value) {
        return rga_addr + "/mmsp/sensorIonSource/set?tPThresholdCurrent=" + value;
    }

    static const QString getThrehold(const QString& rga_addr) {
        return rga_addr + "/mmsp/sensorIonSource/get?tPThresholdCurrent";
    }

};


#endif // HTTP_COMMAND_H
