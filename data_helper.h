#ifndef DATAHELPER_H
#define DATAHELPER_H

#include "utility/comm_http.h"

class DataHelper {
  public:
    DataHelper();

    ~DataHelper();

    const static QMap<int, QString> C_POSELEMAP;

  public:
    static bool     check_rga_conn (const QString& ip, uint port);

    static void     save_last_config (const QString& rga = "", const QString& recipe = "", const QString& run = "");

    static int      tm_to_sec (const QTime& time);

    static QTime    sec_to_tm (int sec);

    static int      save_config (const QMap<QString, QString>& values, const QString& file_name, const QString& file_folder, const QString& group = "");

    static void     write_data_file (const QString& data, QFile* file);

    static double   pow2_calc (const QJsonArray& data_array, double d_magic = 1);

    static QPen     get_pen (int index);

    static QColor   get_color (int index);

    static QString  del_config_file (const QString& file_name, const QString& file_folder, const QString& group = "");

    static QString  get_file_folder (const QString& folder_type);

    static QString  get_pos_el (int idx = 0);

    static double   cal_pp_val (int i_amu, double d_crnt_val, double d_s_sen, bool b_em_state, double d_em_gain = 1000, int i_unit = 0);

    static QPixmap  get_pixmap (const QString& name, const qreal& ratio, const QSize& size);

    static QStringList  list_config_file (const QString& file_folder);

    static QMap<QString, QString> read_config(const QString& file_name, const QString& file_folder, const QString& group = "myRGA");

    static QMap<QString, QString> gen_recipe_config(
        const QString& em_opt,
        const QString& pre_unit,
        const QString& mass_start,
        const QString& mass_stop,
        const QString& points,
        const QString& method,
        const QString& dwell,
        const QString& flmt,
        const QString& ppamu,
        const QString& rpt_uint,
        const QString& peroid = "0");

    static QVector<double> gen_ppamu_pos(int i_start, int i_stop, int i_ppamu = 1);

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
