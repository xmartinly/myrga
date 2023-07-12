#include "data_helper.h"

QString border_toolbtn = "border: 2px dashed darkMagenta; border-radius = 2px;";
QString border_none = "border: none;";

const QMap<int, QString> DataHelper::C_POSELEMAP  = {
    {1, u8"Hydrogenium"},
    {2, u8"Hydrogen"},
    {3, u8"Helium(3)"},
    {4, u8"Helium"},
    {12, u8"Carbon(C)"},
    {14, u8"Nitrogen(N)"},
    {16, u8"Oxygen(O)/Methane"},
    {17, u8"HO/Ammonia"},
    {18, u8"Water"},
    {20, u8"Neon"},
    {28, u8"Nitrogen(N2)/Carbon Monoxide"},
    {30, u8"Silane"},
    {31, u8"Ethyl/Methyl Alcohol"},
    {32, u8"Oxygen(O2))"},
    {34, u8"Hydrogen Sulfide"},
    {40, u8"Argon"},
    {41, u8"Hexane"},
    {43, u8"Acetone/Heptane/Methyl ethyl ketone/MP oil/TMP oil"},
    {44, u8"Carbon Dioxide"},
    {45, u8"Isopropyl Alcohol"},
    {50, u8"DP oil PPE"},
    {51, u8"Halocarbon 23"},
    {69, u8"Carbon tetrafluoride/DP oil Fomblin/Halocarbon 13/PFK*/PFTBA**/Trifluoromethane"},
    {77, u8"Chlorobenzene"},
    {78, u8"Benzene/DP oil DC705"},
    {84, u8"Krypton"},
    {85, u8"Halocarbon 12/Silicon Tetrafluoride"},
    {91, u8"Toluene"},
    {95, u8"Trichloroethylene"},
    {97, u8"Trichloroethane"},
    {101, u8"Halocarbon 11/113"},
    {117, u8"Carbon tetrachloride"},
    {132, u8"Xenon"},
};

DataHelper::DataHelper() {}

DataHelper::~DataHelper() {}

///
/// \brief DataHelper::checkConnect. check connection from the host.
/// \param ip, QString
/// \param port, uint
/// \return bool, connect state.
///
bool DataHelper::check_rga_conn(const QString& ip, uint port) {
    QTcpSocket socket;
    socket.setProxy(QNetworkProxy::NoProxy);
    socket.connectToHost(ip, static_cast<uint>(port));
    bool b_connected = socket.waitForConnected(500);
    socket.disconnectFromHost();
    return b_connected;
}

///
/// \brief DataHelper::writeDataFile. write data to file.
/// \param data, QString.
/// \param file, QFile pointer.
///
void DataHelper::write_data_file(const QString& data, QFile* file) {
    if (file->open(QFile::WriteOnly | QIODevice::Append)) {
        file->write(data.toStdString().c_str());
        file->close();
    }
}

///
/// \brief DataHelper::getColor
/// \param index
/// \return
///
QColor DataHelper::get_color(int index) {
    uint8_t r = (index + 47) % 256;
    uint8_t g = (r * 151) % 256;
    uint8_t b = (g * 97) % 256;
    return QColor(r, g, b);
}

///
/// \brief DataHelper::getPen
/// \param index
/// \return
///
QPen DataHelper::get_pen(int index) {
    return QPen(get_color(index), 2);
}

///
/// \brief DataHelper::saveConf. Save config file.
/// \param values. QMap<QString, QString>
/// \param file_name. QString
/// \param file_folder. QString
/// \param group. QString
/// \return int. Section count saved.
///
int DataHelper::save_config(const QMap<QString, QString>& values,
                            const QString& file_name, const QString& file_folder,
                            const QString& group) {
    QString s_iniFile = del_config_file(file_name, file_folder, group);
    int i_writeCount = 0;
    QSettings f_iniFile(s_iniFile, QSettings::IniFormat);
    f_iniFile.beginGroup(group);
    QMap<QString, QString>::const_iterator i;
    for (i = values.constBegin(); i != values.constEnd(); ++i) {
        f_iniFile.setValue(i.key(), i.value());
        i_writeCount++;
    }
    f_iniFile.endGroup();
    return i_writeCount;
}

///
/// \brief DataHelper::delFile. Delete config file.
/// \param file_name. QString
/// \param file_folder. QString
/// \return QString. File absolute path.
///
QString DataHelper::del_config_file(const QString& file_name, const QString& file_folder, const QString& group) {
    QString s_iniFile = file_folder + '/' + file_name;
    QFile file(s_iniFile);
    if (file.exists() && group == "") {
        file.remove();
    }
    QSettings f_iniFile(s_iniFile, QSettings::IniFormat);
    f_iniFile.remove(group);
    return s_iniFile;
}

///
/// \brief DataHelper::listConfFile. List files name from the given folder.
/// \param file_folder. QString
/// \return QStringList. Files name list.
///
QStringList DataHelper::list_config_file(const QString& file_folder) {
    QStringList sl_files;
    sl_files.clear();
    QDir dir(file_folder);
    if (!dir.exists()) {
        return sl_files;
    }
    dir.setFilter(QDir::Dirs | QDir::Files);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList files = dir.entryInfoList();
    foreach (QFileInfo fi, files) {
        if (fi.fileName() == "." || fi.fileName() == "..") {
            continue;
        }
        if (fi.isDir()) {
            continue;
        } else {
            QString s_filePath = fi.filePath();
            int i_slashIdx = s_filePath.lastIndexOf('/');
            int i_dotIdx = s_filePath.lastIndexOf('.');
            sl_files << s_filePath.mid(i_slashIdx + 1, i_dotIdx - i_slashIdx - 1);
        }
    }
    return sl_files;
}

///
/// \brief DataHelper::readConf
/// \param file_name
/// \param file_folder
/// \param group
/// \return
///
QMap<QString, QString>
DataHelper::read_config(const QString& file_name, const QString& file_folder, const QString& group) {
    QMap<QString, QString> qm_values = {};
    QString s_iniFile = file_folder + '/' + file_name;
    QFile file(s_iniFile);
    if (!file.exists()) {
        QMessageBox::warning(nullptr, u8"Read Error",
                             u8"Config not found. " + file_name);
        return qm_values;
    }
    QSettings f_iniFile(s_iniFile, QSettings::IniFormat);
    f_iniFile.beginGroup(group);
    QStringList keyList = f_iniFile.childKeys();
    foreach (QString key, keyList) {
        qm_values.insert(key,
                         f_iniFile.value(key).toString().toStdString().c_str());
    }
    f_iniFile.endGroup();
    return qm_values;
}

QMap<QString, QString> DataHelper::gen_recipe_config(const QString& em_opt,
        const QString& pre_unit,
        const QString& mass_start,
        const QString& mass_stop,
        const QString& points,
        const QString& method,
        const QString& dwell,
        const QString& flmt,
        const QString& ppamu,
        const QString& rpt_uint,
        const QString& peroid,
        const QString& run) {
    QMap<QString, QString> rcpt;
    rcpt.insert("EmOpt",      em_opt);
    rcpt.insert("PressureUnit", pre_unit);
    rcpt.insert("Peroid",     peroid);
    rcpt.insert("StartMass",  mass_start);
    rcpt.insert("StopMass",   mass_stop);
    rcpt.insert("Points",     points);
    rcpt.insert("Method",     method);
    rcpt.insert("Dwell",      dwell);
    rcpt.insert("Flmt",       flmt);
    rcpt.insert("PPAmu",      ppamu);
    rcpt.insert("ReportUnit", rpt_uint);
    rcpt.insert("Run",        run);
    return rcpt;
}

QVector<double> DataHelper::gen_ppamu_pos(int i_start, int i_stop, int i_ppamu) {
    if(i_start == i_stop || i_start > i_stop) {
        QString s_err = QString("invalid start(%1) or stop(%2) value.").arg(i_start).arg(i_stop);
        qDebug() << Q_FUNC_INFO << s_err;
        return {};
    }
    QVector<double> vd_step = {};
    int i_range = i_stop - i_start;
    double d_single = 1 / static_cast<double>(i_ppamu);
    int i_len   = i_ppamu * i_range + 1;
    for (int var = 0; var < i_len; ++var) {
        vd_step.append(1 + d_single * var);
    }
    return vd_step;
}

///
/// \brief DataHelper::pow2Calc
/// \param data
/// \param d_magic
/// \return
///
double DataHelper::pow2_calc(const QJsonArray& data_array, double d_magic) {
    int i_first = data_array.first().toInt();
    int i_second = 0;
    if (data_array.count() == 2) {
        i_second = data_array.last().toInt();
        return fabs(i_first) * 1e-8 * pow(2.0, i_second) / d_magic;
    }
    return i_first;
}

///
/// \brief DataHelper::tmToSec. Convert QTime to seconds.
/// \param time. QTime
/// \return int.
///
int DataHelper::tm_to_sec(const QTime& time) {
    int i_sec = 0;
    i_sec = time.hour() * 3600 + time.minute() * 60;
    return i_sec;
}

///
/// \brief DataHelper::getFileFolder
/// \param folder_type
/// \return
///
QString DataHelper::get_file_folder(const QString& folder_type) {
    QString app_folder = QCoreApplication::applicationDirPath() + "/";
    if (folder_type.length() < 1) {
        return app_folder;
    }
    return app_folder + folder_type;
}

///
/// \brief DataHelper::secToTm. Calculate seconds to QTime.
/// \param sec. int
/// \return QTime
///
QTime DataHelper::sec_to_tm(int sec) {
    QTime time;
    int i_hour = 0;
    int i_min = 0;
    i_hour = sec / 3600;
    i_min = (sec % 3600) / 60;
    time.setHMS(i_hour, i_min, 0);
    return time;
}

///
/// \brief DataHelper::getPosEl. get amu element string
/// \param idx
/// \return
///
QString DataHelper::get_pos_el(int idx) {
    QString s_el = "*";
    if(C_POSELEMAP.contains(idx)) {
        s_el = C_POSELEMAP.value(idx);
    }
    return s_el;
}

double DataHelper::cal_pp_val(int i_amu, double d_crnt_val, double d_s_sen, bool b_em_state, double d_em_gain, int i_unit) {
    Q_ASSERT_X(d_crnt_val > 0, Q_FUNC_INFO, "d_crnt_val error");
    double d_magic = 1;
    //0: torr, 1: mbar, 2: pa
    double d_pressure_factor = 1;
    switch (i_unit) {
        case 1: //mbar
            d_pressure_factor *= 1.3332;
            break;
        case 2: //Pa
            d_pressure_factor *= 133.32;
            break;
    }
    double d_ff_n28 = 1.00, d_ff_ab = 1.00, d_xf_ab = 1.00, d_tf_b = 28.0 / i_amu, d_df_ab = 1.00, d_em_g = 1.00;
    if(b_em_state) {
        d_em_g = d_em_gain;
//        d_df_ab = pow(d_tf_b, 0.3);
    }
    switch (i_amu) {
        case 2:                     //H2
            d_ff_ab = 0.98;
            d_xf_ab = 0.44;
            break;
        case 4:                     //He
            d_ff_ab = 1.00;
            d_xf_ab = 0.14;
            break;
        case 16:
            d_ff_ab = 0.46;
            d_xf_ab = 1.60;
            break;
        case 18:                     //H2O
            d_ff_ab = 0.75;
            d_xf_ab = 1.0;
            break;
        case 20:                     //Ne
            d_ff_ab = 0.90;
            d_xf_ab = 0.23;
            break;
        case 28:                     //N2 CO2
            d_ff_ab = 0.94;
            d_xf_ab = 1.00;
            break;
        case 31:
            d_ff_ab = 0.49;
            d_xf_ab = 3.6;
            break;
        case 32:                     //O2
            d_ff_ab = 0.95;
            d_xf_ab = 1.00;
            break;
        case 34:
            d_ff_ab = 0.52;
            d_xf_ab = 2.20;
            break;
        case 40:                     //Ar
            d_ff_ab = 0.88;
            d_xf_ab = 1.20;
            break;
        case 41:
            d_ff_ab = 0.21;
            d_xf_ab = 6.60;
            break;
        case 43:                     //CO2
            d_ff_ab = 0.58;
            d_xf_ab = 3.60;
            break;
        case 44:                     //CO2
            d_ff_ab = 0.85;
            d_xf_ab = 1.40;
            break;
        case 78:
            d_ff_ab = 0.53;
            d_xf_ab = 5.90;
            break;
        case 84:                     //Kr
            d_ff_ab = 0.45;
            d_xf_ab = 1.70;
            break;
        case 85:
            d_ff_ab = 0.62;
            d_xf_ab = 2.70;
            break;
        case 91:
            d_ff_ab = 0.53;
            d_xf_ab = 6.80;
            break;
        default:
            d_ff_ab = 1.00;
            d_xf_ab = 1.00;
    }
//    qDebug() << b_em_state << d_em_g << d_s_sen;
    double d_calc_factor = d_ff_n28 / (d_ff_ab * d_xf_ab * d_tf_b * d_df_ab * d_s_sen * d_em_g);
    return d_crnt_val * d_calc_factor * d_pressure_factor * d_magic;
}

///
/// \brief DataHelper::getPixmap
/// \param name
/// \param ratio
/// \param size
/// \return
///
QPixmap DataHelper::get_pixmap(const QString& name, const qreal& ratio,
                               const QSize& size, PictureType type) {
    QString pic_path = ":/Resource/Picture/%1/%2.%3";
    QString pic_meta = "";
    switch (type) {
        case SVG:
            pic_meta = "svg";
            break;
        case JPG:
            pic_meta = "jpg";
            break;
        case PNG:
            pic_meta = "png";
            break;
        default:
            pic_meta = "ico";
            break;
    }
    const QIcon& icon = QIcon(pic_path.arg(pic_meta).arg(name).arg(pic_meta));
    QPixmap pixmap =
        icon.pixmap(size * ratio)
        .scaled(size * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap.setDevicePixelRatio(ratio);
    return pixmap;
}
