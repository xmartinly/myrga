#ifndef DATAHELPER_H
#define DATAHELPER_H


class DataHelper {
  public:
    DataHelper();

    ~DataHelper();

    const static QMap<int, QString> C_POSELEMAP;

  public:
    static bool     checkConnect    (const QString& ip, uint port);

    static void     save_last_config(const QString& rga = "", const QString& recipe = "", const QString& run = "");

    static int      tmToSec         (QTime time);

    static int      saveConf        (const QMap<QString, QString>& values, QString file_name, QString file_folder, QString group = "Virgoa");

    static void     writeDataToFile (const QString& data, QFile* file);

    static double   pow2Calc        (const QJsonArray& data_array, double d_magic = 1);

    static QPen     getPen          (int index);

    static QColor   getColor        (int index);

    static QString  delFile         (QString file_name, QString file_folder);

    static QString  getFileFolder   (QString folder_type);

    static QTime    secToTm         (int sec);

    static QString  getPosEl       (int idx = 0);

    static double   calPPVal       (int i_amu, double d_crnt_val, double d_s_sen, bool b_em_state, double d_em_gain = 1000, int i_unit = 0);

    static QStringList  listConfFile(QString file_folder);

    static QPixmap      getPixmap   (const QString& name, const qreal& ratio, const QSize& size);

    static QMap<QString, QString> readConf(QString file_name, QString file_folder, QString group = "Virgoa");

    static QVector<double> genPPamuPos(int i_start, int i_stop, int i_ppamu = 1);

    template <typename T_>
    static T_ findMax(const QVector<T_> data_vector) {
        T_ t_max = 0;
        foreach(T_ val, data_vector) {
            t_max = val > t_max ? val : t_max;
        }
        return t_max;
    }

};


#endif // DATAHELPER_H
