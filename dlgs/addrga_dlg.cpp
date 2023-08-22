#include "addrga_dlg.h"
#include "ui_addrga_dlg.h"

AddRgaDlg::AddRgaDlg(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddRgaDlg) {
    ui->setupUi(this);
    ui->le_rga_tag->setText("myRGA");
    ui->ip_widget->setIP("192.168.1.100");
    ui->le_port->setText("80");
    rga_config_path = DataHelper::get_file_folder("rgas");
    recipe_config_path = DataHelper::get_file_folder("recipes");
//    initRecipeList();
    tbl_init();
}

///
/// \brief AddRgaDlg::~AddRgaDlg
///
AddRgaDlg::~AddRgaDlg() {
    delete ui;
}

///
/// \brief AddRgaDlg::setRgaTag
/// \param tag
///
void AddRgaDlg::set_rga_tag(const QString& tag) {
    is_from_edit = true;
    QStringList sl_recipes = DataHelper::list_config_file(rga_config_path);
    QMap<QString, QString> qm_values = DataHelper::read_config(tag + ".ini", rga_config_path, "Rga");
    if(!qm_values.count()) {
        QMessageBox::warning(nullptr, "Read Failed", "No value readed.");
        return;
    }
    //rga ip
    if(!qm_values.contains("IP")) {
        return;
    }
    QString s_ip = qm_values.find("IP")->toStdString().c_str();
    //rga port
    if(!qm_values.contains("Port")) {
        return;
    }
    QString s_port = qm_values.find("Port")->toStdString().c_str();
    //read Tag
    if(!qm_values.contains("Tag")) {
        return;
    }
    QString s_tag = qm_values.find("Tag")->toStdString().c_str();
    ui->ip_widget->setIP(s_ip);
    ui->le_port->setText(s_port);
    ui->le_rga_tag->setText(s_tag);
}


///
/// \brief AddRgaDlg::checkSettingConflict
/// \param tag
/// \param ip
/// \return
///
int AddRgaDlg::check_setting_conflict(QString tag, QString ip) {
    QStringList sl_rgas = DataHelper::list_config_file(rga_config_path);
    foreach (QString _rga, sl_rgas) {
        if(_rga == tag) {
            QMessageBox::StandardButton btn = QMessageBox::question(this, u8"错误", u8"重复的 RGA 标签.\n覆盖?", QMessageBox::Yes | QMessageBox::No);
            return btn == QMessageBox::Yes;
        }
    }
    return 1;
}
///
/// \brief RecipeDlg::tblInit
///
void AddRgaDlg::tbl_init() {
    ui->tbl_rgas->clear();
    rga_list = DataHelper::list_config_file(rga_config_path);
    int i_rgaLen = rga_list.length();
    QStringList tblHeader;
    tblHeader << u8"序号" << u8"标签";
    ui->tbl_rgas->setColumnCount(2);
    ui->tbl_rgas->setHorizontalHeaderLabels(tblHeader);
    ui->tbl_rgas->verticalHeader()->setVisible(false);
    ui->tbl_rgas->horizontalHeader()->resizeSection(0, 155);
    ui->tbl_rgas->horizontalHeader()->setStretchLastSection(true);
    ui->tbl_rgas->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbl_rgas->setRowCount(i_rgaLen);
    if(!i_rgaLen) {
        return;
    }
    for(int i = 0; i < i_rgaLen; ++i) {
        ui->tbl_rgas->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tbl_rgas->setItem(i, 1, new QTableWidgetItem(rga_list.at(i)));
    }
}

///
/// \brief AddRgaDlg::on_btn_save_clicked
///
void AddRgaDlg::on_btn_save_clicked() {
    QString rga_ip = ui->ip_widget->getIP();
    QString rga_port = ui->le_port->text();
    QString s_rgaAddr = rga_ip + ":" + rga_port;
    if(!DataHelper::check_rga_conn(rga_ip, rga_port.toUInt())) {
        QMessageBox::warning(nullptr, u8"连接错误", u8"无法连接到此地址: \n" + s_rgaAddr);
        return;
    }
    QString s_tag = ui->le_rga_tag->text();
    if(s_tag.length() < 1) {
        QMessageBox::warning(nullptr, u8"错误", u8"请输入唯一的RGA标签");
        return;
    }
    QString s_ip = ui->ip_widget->getIP();
    if(!check_setting_conflict(s_tag, s_ip)) {
        return;
    }
    QString s_port = ui->le_port->text();
    QMap<QString, QString> qm_rga;
    qm_rga.insert("Tag", s_tag);
    qm_rga.insert("IP", s_ip);
    qm_rga.insert("Port", s_port);
    if(DataHelper::save_config(qm_rga, s_tag + ".ini", rga_config_path, "Rga")) {
        QMessageBox::information(nullptr, u8"成功", u8"RGA 连接已保存");
    } else {
        QMessageBox::warning(nullptr, u8"保存失败", u8"请检查设置");
    }
    if(is_from_edit) {
        this->close();
    }
    tbl_init();
}

void AddRgaDlg::on_tbl_rgas_cellClicked(int row, int) {
    row_count = row;
    QStringList sl_rgas = DataHelper::list_config_file(rga_config_path);
    QMap<QString, QString> qm_values = DataHelper::read_config(sl_rgas.at(row) + ".ini", rga_config_path, "Rga");
    if(!qm_values.count()) {
        QMessageBox::warning(nullptr, u8"读取失败", u8"未读取到数据");
        return;
    }
    //read tag
    if(!qm_values.contains("Tag")) {
        return;
    }
    QString s_tag = qm_values.find("Tag")->toStdString().c_str();
    //read ip
    if(!qm_values.contains("IP")) {
        return;
    }
    QString s_ip = qm_values.find("IP")->toStdString().c_str();
    //read port
    if(!qm_values.contains("Port")) {
        return;
    }
    QString s_port = qm_values.find("Port")->toStdString().c_str();
    ui->ip_widget->setIP(s_ip);
    ui->le_rga_tag->setText(s_tag);
    ui->le_port->setText(s_port);
}


void AddRgaDlg::on_btn_del_clicked() {
    if(row_count < 0) {
        return;
    }
    QString s_fileName = rga_list.at(row_count) + ".ini";
    DataHelper::del_config_file(s_fileName, rga_config_path);
    tbl_init();
    row_count = -1;
}



void AddRgaDlg::on_btn_sel_clicked() {
    QMap<QString, QString> qm_rga;
    qm_rga.insert("Tag", ui->le_rga_tag->text());
    qm_rga.insert("IP", ui->ip_widget->getIP());
    qm_rga.insert("Port", ui->le_port->text());
    DataHelper::save_config(qm_rga, "lastrun.ini", DataHelper::get_file_folder(""), "Rga");
    this->close();
}

