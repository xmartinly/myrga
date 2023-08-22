#define WIN32_LEAN_AND_MEAN

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
    ui->frame_points->setVisible(false);
    ui->frm_misc->setVisible(false);
    ui->tb_review->setVisible(false);
    rga_inst = new RgaUtility;
    idle_tmr = new QTimer(this);
    connect(idle_tmr, &QTimer::timeout, this, &MyRga::idle_tmr_action);
    acq_tmr  = new QTimer(this);
    connect(acq_tmr, &QTimer::timeout, this, &MyRga::acq_tmr_action);
    acq_tmr->setInterval(StaticContainer::STC_ACQINTVL);
    dlg_recipe = new RecipeDlg(this);
    connect(dlg_recipe, &RecipeDlg::start_recipe, this, &MyRga::run_from_recipe);
    dlg_add = new AddRgaDlg(this);
    StaticContainer::setCrntRga(rga_inst);
    obs_subj = new ObserverSubject;
    http_cli = CommHttp::GetInstance();
    connect(http_cli, &CommHttp::resp_arrival, this, &MyRga::update_obs);
    chart_right_b_menu = new QMenu(this);
    action_y_type->setObjectName("action_y_type");
    action_rescale->setObjectName("action_rescale");
    action_print->setObjectName("action_print");
    chart_right_b_menu->addAction(action_y_type);
    chart_right_b_menu->addAction(action_rescale);
    chart_right_b_menu->addAction(action_print);
    connect(chart_right_b_menu, &QMenu::triggered, this, &MyRga::chart_actions);
    connect(ui->qcp_line, &QCustomPlot::mousePress, this, &MyRga::chart_right_click);
    connect(ui->qcp_spec, &QCustomPlot::mousePress, this, &MyRga::chart_right_click);
    versio_label = new QLabel(StaticContainer::STC_RVERSION);
    versio_label->setAlignment(Qt::AlignRight);
    auto line_title   = new QCPTextElement(ui->qcp_line, "", QFont("Courier New", 10, QFont::Bold));
    ui->qcp_line->legend->insertRow(0);
    ui->qcp_line->legend->addElement(0, 0, line_title);
//    prog_bar = new QProgressBar(this);
//    prog_bar->setOrientation(Qt::Horizontal);  // 水平方向
//    prog_bar->setRange(0, 100); // 最小值
//    prog_bar->setValue(100);  // 当前进度
    statusBar()->setSizeGripEnabled(false);
    statusBar()->addPermanentWidget(versio_label, 1);
//    statusBar()->addWidget(prog_bar, 0);
    setup_obs();
    read_current_config();
    set_last_rcpt();
    init_data_tbl(true);
}
///
/// \brief MyRga::~MyRga
///
MyRga::~MyRga() {
    obs_subj->remove_all_obs();
    delete obs_subj;
    delete rga_inst;
    rga_inst = nullptr;
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
    if(acq_tmr->isActive()) {
        acq_tmr->stop();
    }
}
///
/// \brief MyRga::on_tb_flmt_clicked
///
void MyRga::on_tb_flmt_clicked() {
    if(rga_inst == nullptr) {
        return;
    }
    if(!rga_inst->get_in_ctrl()) {
        return;
    }
    bool flmt_on = rga_inst->get_status(RgaUtility::SysStatusCode::EmissState);
    http_cli->cmd_exec(rga_inst->gen_rga_action(flmt_on ? RgaUtility::CloseFlmt : RgaUtility::OpenFlmt));
}
///
/// \brief MyRga::on_tb_em_clicked
///
void MyRga::on_tb_em_clicked() {
    if(rga_inst == nullptr) {
        return;
    }
    if(!rga_inst->get_in_ctrl()) {
        return;
    }
    bool em_on = rga_inst->get_status(RgaUtility::SysStatusCode::EMState);
    http_cli->cmd_exec(rga_inst->gen_rga_action(em_on ? RgaUtility::CloseEm : RgaUtility::OpenEm));
}
///
/// \brief MyRga::on_tb_info_clicked
///
void MyRga::on_tb_info_clicked() {
    tw_info_visable = !ui->frm_misc->isVisible();
    ui->frm_misc->setVisible(tw_info_visable);
    StaticContainer::STC_ISMISCINFO = tw_info_visable;
}
///
/// \brief MyRga::on_tb_review_clicked
///
void MyRga::on_tb_review_clicked() {
//    http_cli->cmd_exec(rga_inst->gen_rga_action(RgaUtility::Reboot));
}
///
/// \brief MyRga::on_tb_link_clicked
///
//void MyRga::on_tb_link_clicked() {
//    read_current_config();
//}
///
/// \brief MyRga::on_tb_ctrl_clicked
///
void MyRga::on_tb_ctrl_clicked() {
    QMap<QString, QString> recipe = DataHelper::gen_recipe_config(
                                        "0",
                                        QString::number(ui->cb_unitpressure->currentIndex()),
                                        QString::number(ui->sb_start->value()),
                                        QString::number(ui->sb_end->value()),
                                        ui->le_points->text(),
                                        QString::number(ui->cb_method->currentIndex()),
                                        QString::number(ui->cb_dwell->currentIndex()),
                                        QString::number(ui->cb_flmt->currentIndex()),
                                        QString::number(ui->cb_ppamu->currentIndex()),
                                        QString::number(ui->cb_unitreport->currentIndex()),
                                        "1");
    DataHelper::save_config(recipe, "lastrun.ini", DataHelper::get_file_folder(""), "Recipe");
    if(rga_inst->get_acquire_state()) {
        stop_scan();
    } else {
        start_scan();
    }
    rga_inst->set_em_auto(0);
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
    ui->cb_ppamu->setDisabled(index);
    ui->cb_ppamu->setCurrentIndex(0);
    ui->cb_unitreport->setDisabled(index);
    ui->cb_unitreport->setCurrentIndex(0);
}
///
/// \brief MyRga::idle_tmr_action
///
void MyRga::idle_tmr_action() {
    bool in_acq = rga_inst->get_acquire_state();
    QString label_text = ("%1 | " + StaticContainer::STC_RVERSION).arg(in_acq ?  u8"收集中" :  u8"已停止");
    versio_label->setText(label_text);
    StaticContainer::STC_ISINACQ = acq_tmr->isActive();
    if(in_acq && !acq_tmr->isActive()) {
        acq_tmr->start(StaticContainer::STC_ACQINTVL);
    }
    if(rga_inst == nullptr) {
        return;
    }
    http_cli->cmd_enqueue(rga_inst->get_idle_set(), true);
    if(!rga_inst->get_in_ctrl()) {
        http_cli->cmd_enqueue(rga_inst->gen_rga_action(RgaUtility::ForceCtrl));
        http_cli->cmd_enqueue(rga_inst->gen_rga_action(RgaUtility::AmInCtrl));
        return;
    }
    bool run_set = rga_inst->get_continuous_run();
    int over_tm = rga_inst->get_over_tm();
    if(run_set && over_tm < 1000) {
        rga_inst->reset_over_tm();
    }
    if(!run_set && over_tm < 0) {
        stop_scan();
    }
}
///
/// \brief MyRga::acq_tmr_action
///
void MyRga::acq_tmr_action() {
    if(rga_inst->get_scan_tm_total() < 1) {
        return;
    }
    if(rga_inst->get_flmt_setted() != rga_inst->get_flmt_idx()) {
        http_cli->cmd_exec(rga_inst->gen_rga_action(RgaUtility::CloseFlmt));
        RgaUtility::RgaActions flmt_set = rga_inst->get_flmt_setted() == "1" ? RgaUtility::SetFlmt1st : RgaUtility::SetFlmt2nd ;
        http_cli->cmd_exec(rga_inst->gen_rga_action(flmt_set));
        return;
    }
    if(!rga_inst->get_status(RgaUtility::EmissState)) {
        http_cli->cmd_exec(rga_inst->gen_rga_action(RgaUtility::OpenFlmt));
        return;
    }
    if(rga_inst->get_acquire_state()) {
        http_cli->cmd_enqueue(rga_inst->gen_rga_action(RgaUtility::GetLastScan));
    }
    if(rga_inst->get_em_auto() && !rga_inst->get_status(RgaUtility::EMState)) {
        http_cli->cmd_enqueue(rga_inst->gen_rga_action(RgaUtility::OpenEm));
        rga_inst->set_em_auto(0);
    }
//    prog_bar->setValue(rga_inst->get_prog_val());
}
///
/// \brief MyRga::initDataTbl
///
void MyRga::init_data_tbl(bool is_misc_info) {
    if(!is_misc_info) {
        disconnect(ui->tw_data, &QTableWidget::cellClicked, this, &MyRga::tbl_click);
    }
    QTableWidget* tbl = is_misc_info ? ui->tw_info : ui->tw_data;
    tbl->clear();
    tbl->setRowCount(0);
    tbl->setColumnCount(2);
    QStringList tblHeader_main = {u8"项目", u8"值"};
    tbl->setHorizontalHeaderLabels(tblHeader_main);
    tbl->verticalHeader()->setVisible(false);
    tbl->horizontalHeader()->setStretchLastSection(true);
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setSelectionMode(QAbstractItemView::NoSelection);
    tbl->horizontalHeader()->resizeSection(0, 160);
    QStringList sl_tblCol = rga_inst->get_tbl_col(!is_misc_info);
    int i_colCnt = sl_tblCol.count();
    for (int var = 0; var < i_colCnt; ++var) {
        tbl->insertRow(var);
        QTableWidgetItem* item = new QTableWidgetItem(sl_tblCol.at(var));
        if(!is_misc_info) {
            item->setCheckState(var ? Qt::Unchecked : Qt::Checked);
        }
        tbl->setItem(var, 0, item);
        tbl->setItem(var, 1, new QTableWidgetItem("na"));
    }
    if(!is_misc_info) {
        connect(ui->tw_data, &QTableWidget::cellClicked, this, &MyRga::tbl_click, Qt::UniqueConnection);
    }
}
void MyRga::tbl_click(int row, int) {
    bool b_rowChecked = ui->tw_data->item(row, 0)->checkState();
    ui->qcp_line->graph(row)->setVisible(b_rowChecked);
    ui->qcp_line->replot(QCustomPlot::rpQueuedReplot);
    QColor line_color = ui->qcp_line->graph(row)->pen().color();
    ui->tw_data->item(row, 0)->setBackground(b_rowChecked ? line_color : Qt::white);
    ui->tw_data->item(row, 0)->setForeground(b_rowChecked ? Qt::white : Qt::black);
    StaticContainer::STC_SELMASS.clear();
    int i_row_cnt = ui->tw_data->rowCount();
    for (int var = 1; var < i_row_cnt; ++var) {
        if(ui->tw_data->item(var, 0)->checkState() == Qt::Checked) {
            StaticContainer::STC_SELMASS.push_back(var);
        }
    }
    if(!StaticContainer::STC_SELMASS.count()) {
        ui->tb_misc->clear();
        return;
    }
    StaticContainer::STC_CELLCLICKED = true;
    ui->qcp_spec->xAxis->setTicker(calc_ticker());
    update_obs();
    StaticContainer::STC_CELLCLICKED = false;
}
///
/// \brief MyRga::initSpecChart
///
void MyRga::init_spec_chart() {
    QCustomPlot* spec_chart = ui->qcp_spec;
    spec_chart->setNoAntialiasingOnDrag(true);
    spec_chart->xAxis->grid()->setVisible(false);
    spec_chart->xAxis->setTickLabelRotation(15);
    spec_chart->xAxis->setSubTicks(false);
    spec_chart->xAxis->setTickLength(2, 0);
//    spec_chart->xAxis->setLabelFont(QFont(QLatin1String("sans serif"), 8));
//    spec_chart->yAxis->setLabelFont(QFont(QLatin1String("sans serif"), 8));
    spec_chart->yAxis->setNumberFormat("e"); // e = exponential, b = beautiful decimal powers
    spec_chart->yAxis->setNumberPrecision(0);
    spec_chart->yAxis->setRange(1e-14, 5e-2);
}
///
/// \brief MyRga::initLineChart
///
void MyRga::init_line_chart() {
    QStringList sl_col = rga_inst->get_tbl_col(true);
    QCustomPlot* line_chart = ui->qcp_line;
    int i_lineCnt = sl_col.count();
    line_chart->setNoAntialiasingOnDrag(true);
    line_chart->clearPlottables();
    line_chart->legend->setBrush(QColor(230, 230, 230, 40));
    line_chart->legend->setBorderPen(Qt::NoPen);
    line_chart->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    line_chart->xAxis->setTicker(timeTicker);
    line_chart->xAxis->grid()->setVisible(false);
    line_chart->xAxis->setTickLabelRotation(15);
    line_chart->xAxis->setTickLength(2, 0);
    line_chart->yAxis->setScaleType(QCPAxis::stLogarithmic);
    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    line_chart->yAxis->setTicker(logTicker);
    line_chart->yAxis->setNumberFormat("e"); // e = exponential, b = beautiful decimal powers
    line_chart->yAxis->setNumberPrecision(0);
    line_chart->yAxis->setRange(1e-14, 5e-2);
    line_chart->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);
//    line_chart->xAxis->setLabelFont(QFont(QLatin1String("sans serif"), 8));
//    line_chart->yAxis->setLabelFont(QFont(QLatin1String("sans serif"), 8));
    for(int line_index = 0; line_index < i_lineCnt; line_index++) {
        line_chart->addGraph();
        QPen qp_linePen = DataHelper::get_pen(line_index * 13);
        line_chart->graph(line_index)->setPen(qp_linePen);
        line_chart->graph(line_index)->setName(sl_col.at(line_index));
        line_chart->graph(line_index)->setVisible(!line_index);
    }
}

///
/// \brief MyRga::set_spec_xAxis
///
void MyRga::set_spec_xAxis() {
    if(!rga_inst) {
        return;
    }
    auto spec_chart = ui->qcp_spec;
    QSharedPointer<QCPAxisTickerText> textTicker = calc_ticker();
    int i_data_cnt = rga_inst->get_vs_labels().count();
    spec_chart->xAxis->setTicker(textTicker);
    spec_chart->xAxis->setRange(0, i_data_cnt + 1);
}

///
/// \brief MyRga::calc_ticker
/// \return
///
QSharedPointer<QCPAxisTickerText> MyRga::calc_ticker() {
    rga_inst->gen_ticker();
    QVector<QString> x_labels = rga_inst->get_vs_labels();
    int label_cnt = x_labels.count();
    QVector<double> x_ticks = rga_inst->get_vd_ticks();
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    if(label_cnt > 9) {
        for (int var = 0; var < label_cnt; ++var) {
            if((var % 5) && !StaticContainer::STC_SELMASS.contains(var + 1)) {
                x_labels[var] = "";
            }
        }
    }
    textTicker->addTicks(x_ticks, x_labels);
    return textTicker;
}
///
/// \brief MyRga::init_scan
///
void MyRga::init_scan() {
    read_current_config(true);
    idle_tmr->setInterval(StaticContainer::STC_LONGINTVL);
    http_cli->cmd_enqueue(rga_inst->get_scan_set(), true);
    qDebug() << rga_inst->get_scan_set();
    rga_inst->reset_scan_data();
    rga_inst->gen_ticker();
    if(rga_inst->get_is_save_data()) {
        rga_inst->init_data_file(true);
    }
    StaticContainer::STC_SELMASS.clear();
    init_data_tbl();
    init_line_chart();
    init_spec_chart();
    set_spec_xAxis();
}

///
/// \brief MyRga::run_from_recipe
/// \param dur
///
void MyRga::run_from_recipe(int c_run) {
    set_last_rcpt();
    start_scan();
    rga_inst->set_continuous_run(c_run);
}
///
/// \brief MyRga::read_current_config
/// \param only_rcpt
///
void MyRga::read_current_config(bool only_rcpt) {
    if(rga_inst == nullptr) {
        rga_inst = new RgaUtility;
    }
    QString file_name = "lastrun.ini";
    QString file_folder = DataHelper::get_file_folder("");
    QMap<QString, QString> qm_rcp, qm_rga_conn;
    qm_rcp = DataHelper::read_config(file_name, file_folder, "Recipe");
    RecipeSet recpt;
    recpt.s_rcpName     = "myRGA";
    recpt.i_period      = qm_rcp.value("Peroid").toInt() * 1000;
    recpt.s_method      = qm_rcp.value("Method").toInt() ? "Points" : "Sweep";
    recpt.s_dwell       = DataHelper::dwell_convert(qm_rcp.value("Dwell").toInt());
    recpt.s_rUnit       = qm_rcp.value("ReportUnit").toInt() ? "PP" : "Current";
    recpt.s_pUnit       = qm_rcp.value("PressureUnit").toInt();
    qDebug() << qm_rcp.value("PressureUnit");
    recpt.s_emOpt       = qm_rcp.value("EmOpt").toInt() ? "on" : "off";
    recpt.s_ppamu       = DataHelper::ppamu_convert(qm_rcp.value("PPAmu").toInt());
    recpt.s_flmtIdx     = QString::number(qm_rcp.value("Flmt").toInt() + 1);
    recpt.s_startMass   = qm_rcp.value("StartMass").toStdString().c_str();
    recpt.s_stopMass    = qm_rcp.value("StopMass").toStdString().c_str();
    QString s_points    = qm_rcp.value("Points").toStdString().c_str();
    recpt.sl_points     = s_points.split("/");
    rga_inst->set_scan_rcpt(recpt);
    if(only_rcpt) { // don't reset the connection and status when only read recipe
        return;
    }
    qm_rga_conn = DataHelper::read_config(file_name, file_folder, "Rga");
    QString s_ip = "";
    QString s_tag = "";
    QString s_port = "";
    if(!qm_rga_conn.contains("IP")) { //ip
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
    if(rga_inst->get_rga_ip() != s_ip) {
        rga_disconn();
    }
    QString rga_addr = "http://" + s_ip + ":" + s_port;
    rga_inst->reset_all();
    rga_inst->set_rga_addr(rga_addr);
    rga_inst->set_rga_tag(s_tag);
    idle_tmr->start(StaticContainer::STC_IDLINTVL);
}
///
/// \brief MyRga::setup_obs
///
void MyRga::setup_obs() {
    //******************************************************************//
    //** link button
    DataObserver* link_btn_obs = new TbObserver(ui->tb_link);
    link_btn_obs->setObjectName("link_btn_obs");
    obs_subj->add_obs(link_btn_obs);
    //******************************************************************//
    //** flmt button
    DataObserver* flmt_btn_obs = new TbObserver(ui->tb_flmt);
    flmt_btn_obs->setObjectName("flmt_btn_obs");
    obs_subj->add_obs(flmt_btn_obs);
    //******************************************************************//
    //** em button
    DataObserver* em_btn_obs = new TbObserver(ui->tb_em);
    em_btn_obs->setObjectName("em_btn_obs");
    obs_subj->add_obs(em_btn_obs);
    //******************************************************************//
    //** info button
    DataObserver* info_btn_obs = new TbObserver(ui->tb_info);
    info_btn_obs->setObjectName("info_btn_obs");
    obs_subj->add_obs(info_btn_obs);
    //******************************************************************//
    //** ctrl button
    DataObserver* ctrl_btn_obs = new TbObserver(ui->tb_ctrl);
    ctrl_btn_obs->setObjectName("ctrl_btn_obs");
    obs_subj->add_obs(ctrl_btn_obs);
    //******************************************************************//
    //** tw_data
    DataObserver* tw_data_obs = new TableObserver(ui->tw_data);
    tw_data_obs->setObjectName("tw_data_obs");
    obs_subj->add_obs(tw_data_obs);
    //******************************************************************//
    //** tw_info
    DataObserver* tw_info_obs = new TableObserver(ui->tw_info);
    tw_info_obs->setObjectName("tw_info_obs");
    obs_subj->add_obs(tw_info_obs);
    //******************************************************************//
    //** qcp_line
    DataObserver* qcp_line_obs = new LineChartObserver(ui->qcp_line);
    qcp_line_obs->setObjectName("qcp_line_obs");
    obs_subj->add_obs(qcp_line_obs);
    //******************************************************************//
    //** qcp_spec
    DataObserver* qcp_spec_obs = new SpecChartObserver(ui->qcp_spec);
    qcp_spec_obs->setObjectName("qcp_spec_obs");
    obs_subj->add_obs(qcp_spec_obs);
    //******************************************************************//
    //** text browser misc
    DataObserver* tb_misc = new TextInfoObserver(ui->tb_misc);
    tb_misc->setObjectName("tb_misc_obs");
    obs_subj->add_obs(tb_misc);
//    //******************************************************************//
//    //** text browser misc
//    DataObserver* pro_bar = new ProgressbarObserver(prog_bar);
//    pro_bar->setObjectName("pro_bar_obs");
//    obs_subj->add_obs(pro_bar);
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
    QMessageBox::StandardButton result = QMessageBox::question(this, u8"退出", u8"是否确认退出?",
                                         QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::No);
    if (result == QMessageBox::Yes) {
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        rga_disconn();
        event->accept();
    } else {
        event->ignore();
    }
}

///
/// \brief MyRga::set_last_rcpt
///
void MyRga::set_last_rcpt() {
    QMap<QString, QString> qm_values = DataHelper::read_config("lastrun.ini", DataHelper::get_file_folder(""), "Recipe");
    if(!qm_values.count()) {
        QMessageBox::warning(nullptr, u8"错误", u8"未读取到任何数据");
        return;
    }
    //read Method
    if(!qm_values.contains("Method")) {
        return;
    }
    int i_method = qm_values.find("Method")->toInt();
    //read StartMass
    if(!qm_values.contains("StartMass")) {
        return;
    }
    int i_startMass = qm_values.find("StartMass")->toInt();
    //read StopMass
    if(!qm_values.contains("StopMass")) {
        return;
    }
    int i_stopMass = qm_values.find("StopMass")->toInt();
    //read Points
    if(!qm_values.contains("Points")) {
        return;
    }
    QString s_points = qm_values.find("Points")->toStdString().c_str();
    //read Dwell
    if(!qm_values.contains("Dwell")) {
        return;
    }
    int i_dwell = qm_values.find("Dwell")->toInt();
    //read PPAmu
    if(!qm_values.contains("PPAmu")) {
        return;
    }
    int i_ppamu = qm_values.find("PPAmu")->toInt();
    //read Pressure unit
    if(!qm_values.contains("PressureUnit")) {
        return;
    }
    int i_pressureUnit = qm_values.find("PressureUnit")->toInt();
    //read Data Type
    if(!qm_values.contains("ReportUnit")) {
        return;
    }
    int i_reportUnit = qm_values.find("ReportUnit")->toInt();
    if(!qm_values.contains("Flmt")) {
        return;
    }
    int i_flmt = qm_values.find("Flmt")->toInt() + 1;
    on_cb_method_currentIndexChanged(i_method);
    ui->cb_method->setCurrentIndex(i_method);
    ui->le_points->setText(s_points);
    ui->sb_start->setValue(i_startMass);
    ui->sb_end->setValue(i_stopMass);
    ui->cb_dwell->setCurrentIndex(i_dwell);
    ui->cb_flmt->setCurrentIndex(i_flmt);
    ui->cb_ppamu->setCurrentIndex(i_ppamu);
    ui->cb_unitpressure->setCurrentIndex(i_pressureUnit);
    ui->cb_unitreport->setCurrentIndex(i_reportUnit);
}
///
/// \brief MyRga::rga_disconn. actions: scan stop, close em, close emission, release control
///
void MyRga::rga_disconn() {
    if(!rga_inst) {
        return;
    }
    rga_inst->write_scan_data(true);
    if(acq_tmr->isActive()) {
        acq_tmr->stop();
    }
    if(idle_tmr->isActive()) {
        idle_tmr->stop();
    }
    QStringList exit_sets = StaticContainer::STC_ISDEBUG ? rga_inst->get_stop_set() : rga_inst->get_close_set();
    foreach (auto cmd, exit_sets) {
        http_cli->cmd_exec(cmd);
        QThread::msleep(200);
    }
}

///
/// \brief MyRga::on_tw_info_cellDoubleClicked
/// \param row
///
void MyRga::on_tw_info_cellDoubleClicked(int row, int) {
    if(!rga_inst) {
        return;
    }
    if(row == 0) {
        ++rga_addr_click_cnt;
    } else {
        rga_addr_click_cnt = 0;
    }
    if(rga_addr_click_cnt == 5 && rga_inst->get_in_ctrl()) {
        QMessageBox::StandardButton btn = QMessageBox::question(this,  u8"重启",  u8"是否确定重启?", QMessageBox::Yes | QMessageBox::No);
        if(btn == QMessageBox::Yes) {
            http_cli->cmd_exec(rga_inst->gen_rga_action(RgaUtility::Reboot));
            acq_tmr->stop();
            idle_tmr->start(StaticContainer::STC_IDLINTVL);
            rga_inst->set_acquire_state(false);
            rga_inst->reset_all();
            update_obs();
        }
        rga_addr_click_cnt = 0;
        return;
    }
    if((row == 8) && (rga_inst->get_err_cnt() > 0)) {
        QMessageBox::warning(this, "Error", rga_inst->get_err_list().join("\n"));
        return;
    }
    if(row == 7 && !rga_inst->get_acquire_state()) {
        bool save_data = rga_inst->get_is_save_data();
        QString is_save =  u8"设置数据保存 %1?";
        QMessageBox::StandardButton btn = QMessageBox::question(this,  u8"数据记录", is_save.arg(save_data ? u8"关" : u8"开"), QMessageBox::Yes | QMessageBox::No);
        save_data = !save_data;
        if(btn == QMessageBox::Yes) {
            rga_inst->set_is_save_data(save_data);
        }
        return;
    }
}
///
/// \brief MyRga::chart_right_click
/// \param event
///
void MyRga::chart_right_click(QMouseEvent* event) {
//    QCustomPlot* plot = qobject_cast<QCustomPlot*>(sender());
    bool right_btn_click = event->button() == Qt::RightButton,
         shift_btn_press = QApplication::keyboardModifiers() == Qt::ShiftModifier;
    if(!right_btn_click) {
        return;
    }
    // right click to pop out action menu
    if(right_btn_click && !shift_btn_press) {
        chart_right_b_menu->exec(QCursor::pos());
        return;
    }
    if(! rga_inst->get_acquire_state()) {
        return;
    }
    // virtual line for line chart, shown values at the mouse position
    auto qcp_line = ui->qcp_line;
    QList<int> sel_mass = StaticContainer::STC_SELMASS;
    sel_mass.append(0);
    double x_pos = qcp_line->xAxis->pixelToCoord(event->pos().x());
    int data_tbl_checked_count = sel_mass.count(),
        line_graph_count = qcp_line->graphCount();
    if (line_graph_count < 1 || data_tbl_checked_count  < 1) {
        return;
    }
    // re initial virtual cursor
    if(qcp_line->hasItem(v_curs)) {
        qcp_line->removeItem(v_curs);
        v_curs = nullptr;
    }
    v_curs = new QCPItemLine(qcp_line);
    v_curs->setPen(QColor(51, 153, 204, 160));
    v_curs->start->setCoords(x_pos, QCPRange::minRange);
    v_curs->end->setCoords(x_pos, QCPRange::maxRange * 0.9);
    qcp_line->legend->setVisible(true);
    qcp_line->legend->clearItems();
    QList<double> dl_specData = {};
    QCPTextElement* title = static_cast<QCPTextElement*>(qcp_line->legend->element(0, 0));
    title->setText("<" + y_scale + "> " + QTime::fromMSecsSinceStartOfDay(int(x_pos * 1000)).toString("HH:mm:ss"));
    for(int i = 0; i < line_graph_count ; i ++) {
        double _value = qcp_line->graph(i)->data()->findBegin(x_pos)->value;
        dl_specData << _value;
        QString _valueString = QString::number(_value, 'e', 3);
        if(sel_mass.contains(i)) {
            QString _numString = ui->tw_data->item(i, 0)->text();
            qcp_line->plottable(i)->setName(_numString + ": " + _valueString );
            qcp_line->plottable(i)->addToLegend();
        }
        qcp_line->replot(QCustomPlot::rpQueuedReplot);
    }
}
///
/// \brief MyRga::chart_actions
/// \param action
///
void MyRga::chart_actions(QAction* action) {
    QString action_name = action->objectName();
    if(action_name == "action_print") {
        QPrinter printer;
        QPrintPreviewDialog preview_dialog(&printer, this);
        preview_dialog.setGeometry(50, 50, 1200, 700);
        connect(&preview_dialog, &QPrintPreviewDialog::paintRequested, this, &MyRga::print_chart);
        preview_dialog.exec();
        return;
    }
    // setup line chart ticker and scale type
    QCustomPlot* qcp_line = ui->qcp_line;
    if(action_name == "action_y_type") {
        int line_scale_type = qcp_line->yAxis->scaleType();
        QSharedPointer<QCPAxisTicker> linear_ticker(new QCPAxisTicker);
        QSharedPointer<QCPAxisTickerLog> log_ticker(new QCPAxisTickerLog);
        qcp_line->yAxis->setScaleType(line_scale_type ? QCPAxis::stLinear : QCPAxis::stLogarithmic);
        qcp_line->yAxis->setTicker(line_scale_type ? linear_ticker : log_ticker);
        y_scale = line_scale_type ? u8"线性" : u8"对数";
        qcp_line->rescaleAxes();
        qcp_line->replot();
        return;
    }
    // rescale line chart
    if(action_name == "action_rescale") {
        qcp_line->rescaleAxes();
        qcp_line->replot();
    }
}

///
/// \brief MyRga::print_chart. print line and spec chart to pdf.
/// \param printer
///
void MyRga::print_chart(QPrinter* printer) {
    QCustomPlot* qcp_line = ui->qcp_line;
    QCustomPlot* qcp_spec = ui->qcp_spec;
    printer->setPageOrientation(QPageLayout::Landscape);
    printer->setColorMode(QPrinter::Color);
    printer->setFullPage(false);
    QCPPainter painter(printer);
    int plotWidth = 1600;
    int plotHeight = 1000;
    double scale = 0.75;
    painter.setMode(QCPPainter::pmVectorized);
    painter.setMode(QCPPainter::pmNoCaching);
    painter.setMode(QCPPainter::pmNonCosmetic); // comment this out if you want cosmetic thin lines (always 1 pixel thick independent of pdf zoom level)
    painter.scale(scale, scale);
    qcp_line->toPainter(&painter, plotWidth - 140, plotHeight);
    printer->newPage();
    qcp_spec->toPainter(&painter, plotWidth - 140, plotHeight);
}

///
/// \brief MyRga::start_scan
///
void MyRga::start_scan() {
    if(!rga_inst) {
        return;
    }
    acq_tmr->stop();
    ui->frame_settings->setHidden(true);
    init_scan();
    acq_tmr->start();
    idle_tmr->setInterval(StaticContainer::STC_LONGINTVL);
}

///
/// \brief MyRga::stop_scan
///
void MyRga::stop_scan() {
    if(!rga_inst) {
        return;
    }
    acq_tmr->stop();
    ui->frame_settings->setHidden(false);
    http_cli->cmd_enqueue(rga_inst->get_stop_set(), true);
    rga_inst->write_scan_data(true);
    idle_tmr->setInterval(StaticContainer::STC_IDLINTVL);
//    prog_bar->setValue(100);
}

///
/// \brief MyRga::on_actionManual_triggered
///
void MyRga::on_actionManual_triggered() {
    QString qtManulFile = "./LINXON_Operating_Manual CHS 074-723-P1A.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(qtManulFile));
}


void MyRga::on_actionAbout_triggered() {
    QString s_about = "";
    QMap<QString, QString>::const_iterator it = StaticContainer::STC_RVER.constBegin();
    while (it != StaticContainer::STC_RVER.constEnd()) {
        s_about.append(it.key() + "\n  " + it.value() + "\n");
        ++it;
    }
    QMessageBox::information(this, u8"关于", s_about);
}

