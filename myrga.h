#ifndef MYRGA_H
#define MYRGA_H

#include "observer/observer_subject.h"
#include "dlgs/addrga_dlg.h"
#include "dlgs/recipe_dlg.h"
#include "utility/rga_utility.h"
#include <QMainWindow>


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

  private:
    Ui::MyRga* ui;
    // recipe dialog pointer
    RecipeDlg* dlg_recipe = nullptr;
    // add rga dialog pointer
    AddRgaDlg* dlg_add = nullptr;
    // acquire command generate timer
    QTimer*                 m_acq_tmr           = nullptr;
    // idle command generate timer
    QTimer*                 m_idle_tmr          = nullptr;
    // http connection instance. singleton
//    CommHttp*               m_http              = nullptr;
    // observer subject
    ObserverSubject         m_subObs;
    // print chart
    QAction*                m_action_print      = nullptr;
    // rescale x and y axis
    QAction*                m_action_rescale    = nullptr;
    // y axis ticker type switch
    QAction*                m_action_yaxis      = nullptr;
    // initialize data table
    void init_data_tbl();
    // initialize spec chart
    void init_spec_chart();
    // initialize line chart
    void init_line_chart();
    // setup data observers
    void setup_observers();
    // initialize rga scan setup.
    void init_scan(RgaUtility* inst);
    // menu action when right click on rga label
    void menu_action(MenuActions action = AcquireCtrl);
    // menu text modify
    void menu_modify();
    // menu pop out
    void menu_popout();
    // set spec chart x axis ticks and labels
    void set_spec_xaxis();

    void save_current();

  public slots:
    // receive run from recipe dialog
    void run_from_recipe(int dur);

};

#endif // MYRGA_H
