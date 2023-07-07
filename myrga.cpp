#include "myrga.h"

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
    dlg_recipe = new RecipeDlg(this);
    connect(dlg_recipe, &RecipeDlg::start_recipe, this, &MyRga::run_from_recipe);
    dlg_add = new AddRgaDlg(this);
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
    dlg_add->exec();
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
    dlg_recipe->exec();
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
    dlg_recipe->exec();
}

///
/// \brief MyRga::on_actionComm_triggered
///
void MyRga::on_actionComm_triggered() {
    dlg_add->exec();
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

void MyRga::save_current() {
    QMap<QString, QString> recipe = DataHelper::gen_recipe_config(
                                        "Off",
                                        QString::number(ui->cb_unitpressure->currentIndex()),
                                        QString::number(ui->sb_start->value()),
                                        QString::number(ui->sb_end->value()),
                                        ui->le_points->text(),
                                        ui->cb_method->currentText(),
                                        ui->cb_dwell->currentText(),
                                        ui->cb_flmt->currentText(),
                                        ui->cb_ppamu->currentText(),
                                        ui->cb_unitreport->currentText(),
                                        "0");
    DataHelper::save_config(recipe, "lastrun.ini", DataHelper::get_file_folder(""), "Recipe");
}


void MyRga::run_from_recipe(int dur) {
    qDebug() << dur << "run_from_recipe";
}



void MyRga::on_tb_ctrl_clicked() {
    QMap<QString, QString> recipe;
    recipe = DataHelper::gen_recipe_config(
                 "Off",
                 QString::number(ui->cb_unitpressure->currentIndex()),
                 QString::number(ui->sb_start->value()),
                 QString::number(ui->sb_end->value()),
                 ui->le_points->text(),
                 ui->cb_method->currentText(),
                 ui->cb_dwell->currentText(),
                 ui->cb_flmt->currentText(),
                 ui->cb_ppamu->currentText(),
                 ui->cb_unitreport->currentText(),
                 "-1"
             );
    if(!DataHelper::save_config(recipe, "lastrun.ini", DataHelper::get_file_folder(""), "Recipe")) {
        QMessageBox::warning(nullptr, u8"Failed", u8"Please check the settings.");
        return;
    }
}

