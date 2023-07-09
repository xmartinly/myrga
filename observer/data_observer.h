#ifndef DATAOBSERVER_H
#define DATAOBSERVER_H

#include <QObject>

class DataObserver: public QObject {
    Q_OBJECT
  public:
    DataObserver() {}
    virtual ~DataObserver() {}

    ///
    /// \brief update. Pure virtual function for observers interface.
    ///
    virtual void update() = 0;
};

class TextInfoObserver: public DataObserver {

  public:
    explicit TextInfoObserver(QTextBrowser* browser);
    ~TextInfoObserver();

    void update();

  private:
    QTextBrowser*       m_zone = nullptr;
};

class TbObserver: public DataObserver {
  public:
    explicit TbObserver(QToolButton* btn);
    ~TbObserver();
    void update();



  private:
    QToolButton* m_zone = nullptr;
    QString btn_name = "";
    const QSize btn_size = {40, 40};
    const double btn_ratio = 1;
    QPixmap pd_svg = DataHelper::get_pixmap("flash_on", btn_ratio, btn_size, DataHelper::SVG);
    QPixmap flmt_on_svg = DataHelper::get_pixmap("filament_on", btn_ratio, btn_size, DataHelper::SVG);
    QPixmap flmt_off_svg = DataHelper::get_pixmap("filament_off", btn_ratio, btn_size, DataHelper::SVG);
    QPixmap em_on_svg = DataHelper::get_pixmap("em_on", btn_ratio, btn_size, DataHelper::SVG);
    QPixmap em_off_svg = DataHelper::get_pixmap("em_off", btn_ratio, btn_size, DataHelper::SVG);
    QPixmap info_have_err_svg = DataHelper::get_pixmap("information_a", btn_ratio, btn_size, DataHelper::SVG);
    QPixmap info_no_err_svg = DataHelper::get_pixmap("information", btn_ratio, btn_size, DataHelper::SVG);
    QPixmap link_on_svg = DataHelper::get_pixmap("link", btn_ratio, btn_size, DataHelper::SVG);
    QPixmap link_off_svg = DataHelper::get_pixmap("broken_link", btn_ratio, btn_size, DataHelper::SVG);
    QPixmap start_svg = DataHelper::get_pixmap("resume_button", btn_ratio, btn_size, DataHelper::SVG);
    QPixmap stop_svg = DataHelper::get_pixmap("stop_sign", btn_ratio, btn_size, DataHelper::SVG);

    bool is_flmt_on = false;
    bool is_em_on = false;
};


///
/// \brief The ValuesObserver class
///
class TableObserver: public DataObserver {
  public:
    ///
    /// \brief ValuesObserver. constructor with table widget pointer.
    /// \param tbl(QTableWidget *)
    ///
    explicit TableObserver(QTableWidget* tbl);
    ~TableObserver();

    ///
    /// \brief update. update table widget datas.
    /// \param values(const QVariantMap &)
    ///
    void update();

  private:
    ///
    /// \brief m_zone(QTableWidge*)
    ///
    QTableWidget*       m_zone          = nullptr;
};

///
/// \brief The ProgressbarObserver class
///
class ProgressbarObserver: public DataObserver {
  public:
    ///
    /// \brief ProgressbarObserver. constructor.
    /// \param progress_bar(QProgressBar*)
    ///
    ProgressbarObserver(QProgressBar* progress_bar);

    ///
    /// Descontructer. delete m_zone pointer.
    ~ProgressbarObserver();

    ///
    /// \brief update. update progress bar value data.
    /// \param data
    ///
    void update();

  private:
    ///
    /// \brief m_zone(QProgressBar*)
    ///
    QProgressBar*       m_zone          = nullptr;

};


class ChartObserver : public DataObserver {
  public:
    explicit    ChartObserver(QCustomPlot* chart);
    virtual     ~ChartObserver();
    void        rescaleChart();
    void        printChart();
    void        clearChartData();
    void        selectLine(int line_index);
  protected:
    QCustomPlot*        m_zone          = nullptr;
};



class LineChartObserver: public ChartObserver {
  public:
    explicit LineChartObserver(QCustomPlot* chart);
    ~LineChartObserver();
    ///
    /// \brief update line chart
    ///
    void update();
};

class SpecChartObserver: public ChartObserver {
  public:
    explicit SpecChartObserver(QCustomPlot* chart);
    ~SpecChartObserver();
    ///
    /// \brief update spec chart
    ///
    void update();

  private:
    //bar chart
    QCPModify*        m_qcpBar = nullptr;
    //line chart
    QCPGraph*         m_qcpLine = nullptr;
};



#endif // DATAOBSERVER_H
