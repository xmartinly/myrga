#include "addrga_dlg.h"
#include "ui_addrga_dlg.h"

AddRgaDlg::AddRgaDlg(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddRgaDlg) {
    ui->setupUi(this);
    ui->le_rga_tag->setText("myRGA");
    ui->ip_widget->setIP("192.168.1.100");
    ui->le_port->setText("80");
    m_sRgaPath = DataHelper::getFileFolder("rgas");
    m_sRecipePath = DataHelper::getFileFolder("recipes");
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
    m_bIsFromEdit = true;
    QStringList sl_recipes = DataHelper::listConfFile(m_sRgaPath);
    QMap<QString, QString> qm_values = DataHelper::readConf(tag + ".ini", m_sRgaPath, "Rga");
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
/// \brief AddRgaDlg::checkConnection
/// \return
///
bool AddRgaDlg::check_conn() {
    QString s_ip = ui->ip_widget->getIP();
    if(s_ip.length() < 1) {
        ui->ip_widget->setFocus();
        return false;
    }
    uint    i_port = ui->le_port->text().toUInt();
    if((i_port < 1) || (i_port > 65535)) {
        ui->le_port->setFocus();
        return false;
    }
    return DataHelper::checkConnect(s_ip, i_port);
}

///
/// \brief AddRgaDlg::checkSettingConflict
/// \param tag
/// \param ip
/// \return
///
int AddRgaDlg::check_setting_conflict(QString tag, QString ip) {
    QStringList sl_rgas = DataHelper::listConfFile(m_sRgaPath);
    foreach (QString _rga, sl_rgas) {
        if(_rga == tag) {
            QMessageBox::StandardButton btn = QMessageBox::question(this, u8"Tag Error", u8"Duplicate RGA Tag found.\nOverwrite?", QMessageBox::Yes | QMessageBox::No);
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
    m_slRgas = DataHelper::listConfFile(m_sRgaPath);
    int i_rgaLen = m_slRgas.length();
    QStringList tblHeader;
    tblHeader << u8"No." << u8"Name";
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
        ui->tbl_rgas->setItem(i, 1, new QTableWidgetItem(m_slRgas.at(i)));
    }
}

///
/// \brief AddRgaDlg::on_btn_save_clicked
///
void AddRgaDlg::on_btn_save_clicked() {
    QString s_rgaAddr =  ui->ip_widget->getIP() + ":" + ui->le_port->text();
    if(!check_conn()) {
        QMessageBox::warning(nullptr, "Connection error", "Can't connect to the address: \n" + s_rgaAddr);
        return;
    }
    QString s_tag = ui->le_rga_tag->text();
    if(s_tag.length() < 1) {
        QMessageBox::warning(nullptr, "Tag error", "Please input RGA tag for indentify.");
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
    if(DataHelper::saveConf(qm_rga, s_tag + ".ini", m_sRgaPath, "Rga")) {
        QMessageBox::information(nullptr, "Success", "Rga connection saved.");
    } else {
        QMessageBox::warning(nullptr, "Save Failed", "Please check the settings.");
    }
    if(m_bIsFromEdit) {
        this->close();
    }
    tbl_init();
}

void AddRgaDlg::on_tbl_rgas_cellClicked(int row, int) {
    m_iRow = row;
    QStringList sl_rgas = DataHelper::listConfFile(m_sRgaPath);
    QMap<QString, QString> qm_values = DataHelper::readConf(sl_rgas.at(row) + ".ini", m_sRgaPath, "Rga");
    if(!qm_values.count()) {
        QMessageBox::warning(nullptr, "Read Failed", "No value readed.");
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
    if(m_iRow < 0) {
        return;
    }
    QString s_fileName = m_slRgas.at(m_iRow) + ".ini";
    DataHelper::delFile(s_fileName, m_sRgaPath);
    tbl_init();
    m_iRow = -1;
}



void AddRgaDlg::on_btn_sel_clicked() {
    DataHelper::save_last_config(ui->le_rga_tag->text());
    this->close();
}

