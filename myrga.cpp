#include "myrga.h"
#include "dlgs/addrga_dlg.h"
#include "dlgs/recipe_dlg.h"
#include "ui_myrga.h"

///
/// \brief MyRga::MyRga
/// \param parent
///
MyRga::MyRga(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MyRga) {
    ui->setupUi(this);
    ui->frame_points->setHidden(true);
    ui->tw_info->setVisible(false);
    m_idle_tmr = new QTimer(this);
    connect(m_idle_tmr, &QTimer::timeout, this, &MyRga::idle_tmr_action);
    m_acq_tmr  = new QTimer(this);
    connect(m_acq_tmr, &QTimer::timeout, this, &MyRga::acq_tmr_action);
}
///
/// \brief MyRga::~MyRga
///
MyRga::~MyRga() {
    delete ui;
}





///
/// \brief MyRga::on_tb_comm_clicked
///
void MyRga::on_tb_comm_clicked() {
    open_add_dlg();
}

///
/// \brief MyRga::on_tb_menu_clicked
///
void MyRga::on_tb_menu_clicked() {
    bool frm_left_vis = ui->frm_left->isVisible();
    ui->frm_left->setVisible(!frm_left_vis);
}

///
/// \brief MyRga::on_tb_recipe_clicked
///
void MyRga::on_tb_recipe_clicked() {
    RecipeDlg dlg_recipe;
    dlg_recipe.exec();
}

///
/// \brief MyRga::on_tb_start_clicked
///
void MyRga::on_tb_start_clicked() {
}

///
/// \brief MyRga::on_tb_flmt_clicked
///
void MyRga::on_tb_flmt_clicked() {
}

///
/// \brief MyRga::on_tb_em_clicked
///
void MyRga::on_tb_em_clicked() {
}

///
/// \brief MyRga::on_actionExit_triggered
///
void MyRga::on_actionExit_triggered() {
    this->close();
}

///
/// \brief MyRga::on_cb_method_currentIndexChanged
/// \param index
///
void MyRga::on_cb_method_currentIndexChanged(int index) {
    ui->frame_points->setVisible(index);
    ui->frame_range->setVisible(!index);
}

///
/// \brief MyRga::on_tb_info_clicked
///
void MyRga::on_tb_info_clicked() {
    bool tw_info_visable = ui->tw_info->isVisible();
    ui->tw_info->setVisible(!tw_info_visable);
}

///
/// \brief MyRga::on_actionRecipe_triggered
///
void MyRga::on_actionRecipe_triggered() {
    open_recipe_dlg();
}

///
/// \brief MyRga::on_actionComm_triggered
///
void MyRga::on_actionComm_triggered() {
    open_add_dlg();
}

///
/// \brief MyRga::on_tb_review_clicked
///
void MyRga::on_tb_review_clicked() {
}


///
/// \brief MyRga::acq_tmr_action
///
void MyRga::acq_tmr_action() {
}

///
/// \brief MyRga::idle_tmr_action
///
void MyRga::idle_tmr_action() {
}

///
/// \brief MyRga::initDataTbl
///
void MyRga::init_data_tbl() {
}

///
/// \brief MyRga::initSpecChart
///
void MyRga::init_spec_chart() {
}

///
/// \brief MyRga::initLineChart
///
void MyRga::init_line_chart() {
}

///
/// \brief MyRga::setupObsrvs
///
void MyRga::setup_observers() {
}

void MyRga::open_add_dlg() {
    open_recipe_dlg();
}

void MyRga::open_recipe_dlg() {
    AddRgaDlg dlg_addRga;
    dlg_addRga.exec();
}


