#ifndef MYRGA_H
#define MYRGA_H


#include <QMainWindow>
#include "observer/observer_subject.h"
#include "dlgs/addrga_dlg.h"
#include "dlgs/recipe_dlg.h"
#include "utility/rga_utility.h"


QT_BEGIN_NAMESPACE
namespace Ui {
    class MyRga;
}
QT_END_NAMESPACE

class MyRga : public QMainWindow

{
    Q_OBJECT

    enum MenuActions {
        AcquireCtrl,
        FilamentCtrl,
        EmCtrl,
        DataSaveCtrl,
        RebootCtrl,
        RgaEdit,
        LimitValSet,
    };

  public:
    MyRga(QWidget* parent = nullptr);
    ~MyRga();


  private slots:
    void on_tb_comm_clicked();

    void on_tb_menu_clicked();

    void on_tb_recipe_clicked();

    void on_tb_flmt_clicked();

    void on_tb_em_clicked();

    void acq_tmr_action();

    void idle_tmr_action();

    void on_cb_method_currentIndexChanged(int index);

    void on_tb_info_clicked();

    void on_actionRecipe_triggered();

    void on_actionComm_triggered();

    void on_actionExit_triggered();

    void on_tb_review_clicked();

    void on_tb_ctrl_clicked();

    void on_tb_link_clicked();

    void tbl_click(int row, int);

  private:
    Ui::MyRga* ui;
    // RgaUtility
    RgaUtility* rga_inst = nullptr;
    // observer subject
    ObserverSubject* obs_subj = nullptr;
    // recipe dialog pointer
    RecipeDlg* dlg_recipe = nullptr;
    // add rga dialog pointer
    AddRgaDlg* dlg_add = nullptr;
    // acquire command generate timer
    QTimer* acq_tmr           = nullptr;
    // idle command generate timer
    QTimer* idle_tmr          = nullptr;
    // http request
    CommHttp* http_cli = nullptr;
    // observer subject
    ObserverSubject m_subObs;
    // print chart
    QAction* m_action_print = nullptr;
    // rescale x and y axis
    QAction* m_action_rescale = nullptr;
    // y axis ticker type switch
    QAction* m_action_yaxis = nullptr;
    // padding svg
    QPixmap pd_svg = DataHelper::get_pixmap("flash_on");
    // is misc info opened
    bool tw_info_visable = false;

  private:
    // initialize data table
    void init_data_tbl(bool is_misc_info = false);
    // initialize spec chart
    void init_spec_chart();
    // initialize line chart
    void init_line_chart();
    // setup spectrum chart x axis
    void set_spec_xAxis();
    // initialize rga scan setup.
    void init_scan();
    // menu action when right click on rga label
    void menu_action(MenuActions action = AcquireCtrl);
    // menu text modify
    void menu_modify();
    // menu pop out
    void menu_popout();
    // set spec chart x axis ticks and labels
    void set_spec_xaxis();
    // save current scan settings
    void save_current();
    // read current config
    void read_current_config(bool only_rcpt = false);
    // setup observers
    void setup_obs();
    // close event. disconnect rga, close filament, release control
    void closeEvent(QCloseEvent* event);
    // read last rcpt and setup the checkbox, line_edit,etc
    void set_last_rcpt();


  public slots:
    // receive run from recipe dialog
    void run_from_recipe(int dur);
    // update observers
    void update_obs();

};

#endif // MYRGA_H
