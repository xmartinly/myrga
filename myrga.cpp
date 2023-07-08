#include "myrga.h"

#include "ui_myrga.h"
#include "utility/static_container.h"


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
    idle_tmr = new QTimer(this);
    connect(idle_tmr, &QTimer::timeout, this, &MyRga::idle_tmr_action);
    acq_tmr  = new QTimer(this);
    connect(acq_tmr, &QTimer::timeout, this, &MyRga::acq_tmr_action);
    dlg_recipe = new RecipeDlg(this);
    connect(dlg_recipe, &RecipeDlg::start_recipe, this, &MyRga::run_from_recipe);
    dlg_add = new AddRgaDlg(this);
    http_cli = CommHttp::GetInstance();
    rga_inst = new RgaUtility;
    StaticContainer::setCrntRga(rga_inst);
    obs_subj = new ObserverSubject;
    connect(http_cli, &CommHttp::resp_arrival, this, &MyRga::update_obs);
    setup_obs();
    read_current_config();
}
///
/// \brief MyRga::~MyRga
///
MyRga::~MyRga() {
    obs_subj->remove_all_obs();
    QStringList exit_sets = rga_inst->getCloseSet();
    foreach (auto cmd, exit_sets) {
        http_cli->execCmd(cmd);
    }
    delete obs_subj;
    delete rga_inst;
    delete ui;
}

///
/// \brief MyRga::on_tb_comm_clicked
///
void MyRga::on_tb_comm_clicked() {
    dlg_add->exec();
    read_current_config();
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
    if(rga_inst == nullptr) {
        return;
    }
    if(!rga_inst->getInCtrl()) {
        return;
    }
    bool flmt_on = rga_inst->getRgaStatus(RgaUtility::SysStatusCode::EmissState);
    http_cli->execCmd(rga_inst->genRgaAction(flmt_on ? RgaUtility::CloseFlmt : RgaUtility::OpenFlmt));
    ui->tb_flmt->setStyleSheet(border_toolbtn);
}

///
/// \brief MyRga::on_tb_em_clicked
///
void MyRga::on_tb_em_clicked() {
    if(rga_inst == nullptr) {
        return;
    }
    if(!rga_inst->getInCtrl()) {
        return;
    }
    bool em_on = rga_inst->getRgaStatus(RgaUtility::SysStatusCode::EMState);
    http_cli->execCmd(rga_inst->genRgaAction(em_on ? RgaUtility::CloseEm : RgaUtility::OpenEm));
    ui->tb_em->setStyleSheet(border_toolbtn);
}

///
/// \brief MyRga::on_tb_info_clicked
///
void MyRga::on_tb_info_clicked() {
    bool tw_info_visable = ui->tw_info->isVisible();
    ui->tw_info->setVisible(!tw_info_visable);
}


///
/// \brief MyRga::on_tb_review_clicked
///
void MyRga::on_tb_review_clicked() {
}

///
/// \brief MyRga::on_tb_link_clicked
///
void MyRga::on_tb_link_clicked() {
    read_current_config();
}

///
/// \brief MyRga::on_tb_ctrl_clicked
///
void MyRga::on_tb_ctrl_clicked() {
    if(rga_inst->getAcquireState()) {
        rga_inst->getAcquireState() = false;
        acq_tmr->stop();
        rga_inst->acquireSet();
        return;
    }
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
    read_current_config();
    int try_count = 0;
    while(!rga_inst->getInCtrl() && try_count > 50) {
        QThread::msleep(100);
        ++try_count;
    }
    http_cli->cmdEnQueue(rga_inst->getScanSet(), true);
    rga_inst->setEmManual(true);
    if(acq_tmr->isActive()) {
        return;
    }
    acq_tmr->start(StaticContainer::STC_ACQINTVL);
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
/// \brief MyRga::idle_tmr_action
///
void MyRga::idle_tmr_action() {
    StaticContainer::STC_ISINACQ = acq_tmr->isActive();
    RgaUtility* inst = StaticContainer::getCrntRga();
    if(inst == nullptr) {
        return;
    }
    http_cli->cmdEnQueue(inst->getIdlSet(), true);
    if(!inst->getInCtrl()) {
        http_cli->cmdEnQueue(inst->genRgaAction(RgaUtility::ForceCtrl));
        http_cli->cmdEnQueue(inst->genRgaAction(RgaUtility::AmInCtrl));
        return;
    }
    bool b_run = inst->getRunSet();
    int i_overTm = inst->getOverTime();
    bool b_saveData = inst->getIsSaveData();
    if(!b_run && i_overTm < 0) {
        inst->setAcquireState(false);
    }
    if(b_run && i_overTm < 0 && b_saveData) {
        inst->resetOverTime();
    }
    if(idle_tmr->isActive()) {
        return;
    }
    idle_tmr->start(StaticContainer::STC_IDLINTVL);
}

///
/// \brief MyRga::acq_tmr_action
///
void MyRga::acq_tmr_action() {
    if(rga_inst->getScanTmTotal() < 1) {
        return;
    }
    if(!rga_inst->getRgaStatus(RgaUtility::EmissState)) {
        return;
    }
    if(rga_inst->getAcquireState() && rga_inst->getAcquireState()) {
        http_cli->cmdEnQueue(rga_inst->genRgaAction(RgaUtility::GetLastScan));
    }
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
/// \brief MyRga::save_current
///
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

///
/// \brief MyRga::run_from_recipe
/// \param dur
///
void MyRga::run_from_recipe(int dur) {
    qDebug() << Q_FUNC_INFO << "run_from_recipe";
}

///
/// \brief MyRga::read_current_config
///
void MyRga::read_current_config(bool only_rcpt) {
    RgaUtility* inst = StaticContainer::getCrntRga();
    QString file_name = "lastrun.ini";
    QString file_folder = DataHelper::get_file_folder("");
    QMap<QString, QString> qm_rcp, qm_rga_conn;
    qm_rcp = DataHelper::read_config(file_name, file_folder, "Recipe");
    qm_rga_conn = DataHelper::read_config(file_name, file_folder, "Rga");
    RecipeSet recpt;
    recpt.s_rcpName     = "myRGA";
    recpt.i_period      = qm_rcp.value("Peroid").toInt() * 1000;
    recpt.s_method      = qm_rcp.value("Method").toStdString().c_str();
    recpt.s_dwell       = qm_rcp.value("Dwell").toStdString().c_str();
    recpt.s_rUnit       = qm_rcp.value("ReportUnit").toStdString().c_str();
    recpt.s_pUnit       = qm_rcp.value("PressureUnit").toStdString().c_str();
    recpt.s_emOpt       = qm_rcp.value("EmOpt").toStdString().c_str();
    recpt.s_ppamu       = qm_rcp.value("PPAmu").toStdString().c_str();
    recpt.s_flmtIdx     = qm_rcp.value("Flmt").toStdString().c_str();
    recpt.s_startMass   = qm_rcp.value("StartMass").toStdString().c_str();
    recpt.s_stopMass    = qm_rcp.value("StopMass").toStdString().c_str();
    QString s_points    = qm_rcp.value("Points").toStdString().c_str();
    recpt.sl_points     = s_points.split("/");
    inst->setScanRecipe(recpt);
    if(only_rcpt) {
        return;
    }
    QString s_ip = "";
    QString s_tag = "";
    QString s_port = "";
    if(!qm_rga_conn.contains("IP")) {
        return;
    }
    s_ip = qm_rga_conn.value("IP").toStdString().c_str();
    if(!qm_rga_conn.contains("Tag")) {
        return;
    }
    s_tag = qm_rga_conn.value("Tag").toStdString().c_str();
    if(!qm_rga_conn.contains("Port")) {
        return;
    }
    s_port = qm_rga_conn.value("Port").toStdString().c_str();
    inst->resetAll();
    inst->setRgaAddr("http://" + s_ip + ":" + s_port);
    inst->setRgaTag(s_tag);
    idle_tmr->start(StaticContainer::STC_IDLINTVL);
}

///
/// \brief MyRga::setup_obs
///
void MyRga::setup_obs() {
    //******************************************************************//
    //** link button
    DataObserver* link_btn_obs = new TbObserver(ui->tb_link);
    link_btn_obs->setObjectName("link");
    obs_subj->add_obs(link_btn_obs);
    //******************************************************************//
    //** flmt button
    DataObserver* flmt_btn_obs = new TbObserver(ui->tb_flmt);
    flmt_btn_obs->setObjectName("flmt");
    obs_subj->add_obs(flmt_btn_obs);
    //******************************************************************//
    //** em button
    DataObserver* em_btn_obs = new TbObserver(ui->tb_em);
    em_btn_obs->setObjectName("em");
    obs_subj->add_obs(em_btn_obs);
    //******************************************************************//
    //** info button
    DataObserver* info_btn_obs = new TbObserver(ui->tb_info);
    info_btn_obs->setObjectName("info");
    obs_subj->add_obs(info_btn_obs);
}

///
/// \brief MyRga::update_obs
///
void MyRga::update_obs() {
    obs_subj->notify_obs();
}

///
/// \brief MyRga::closeEvent
/// \param event
///
void MyRga::closeEvent(QCloseEvent* event) {
//    QMessageBox::StandardButton result = QMessageBox::question(this, u8"Exit", "Are you sure to exit?",
//                                         QMessageBox::Yes | QMessageBox::No,
//                                         QMessageBox::No);
//    if (result == QMessageBox::Yes) {
//        event->accept();
//    } else {
//        event->ignore();
//    }
}


