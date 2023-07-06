#include "recipe_dlg.h"
#include "ui_recipe_dlg.h"

///
/// \brief RecipeDlg::RecipeDlg
/// \param parent
///
RecipeDlg::RecipeDlg(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::RecipeDlg) {
    ui->setupUi(this);
    ui->lb_points->setHidden(true);
    ui->le_points->setHidden(true);
    m_s_recipePath = DataHelper::getFileFolder("recipes");
    tblInit();
}
///
/// \brief RecipeDlg::~RecipeDlg
///
RecipeDlg::~RecipeDlg() {
    delete ui;
}

///
/// \brief RecipeDlg::on_cb_method_currentIndexChanged
/// \param index
///
void RecipeDlg::on_cb_method_currentIndexChanged(int index) {
    ui->lb_range->setHidden(index);
    ui->lb_arrow->setHidden(index);
    ui->sb_end->setHidden(index);
    ui->sb_start->setHidden(index);
    ui->lb_points->setHidden(!index);
    ui->le_points->setHidden(!index);
    ui->cb_ppamu->setDisabled(index);
    ui->cb_ppamu->setCurrentIndex(0);
    ui->cb_unitreport->setDisabled(index);
    ui->cb_unitreport->setCurrentIndex(0);
}

///
/// \brief RecipeDlg::on_btn_del_clicked
///
void RecipeDlg::on_btn_del_clicked() {
    if(m_iRow < 0) {
        return;
    }
    QString s_fileName = m_slRecipes.at(m_iRow) + ".ini";
    DataHelper::delFile(s_fileName, m_s_recipePath);
    tblInit();
    m_iRow = -1;
}

///
/// \brief RecipeDlg::on_btn_refresh_clicked
///
//void RecipeDlg::on_btn_refresh_clicked() {
//    tblInit();
//}

///
/// \brief RecipeDlg::on_btn_save_clicked. Save recipe to config file.
///
void RecipeDlg::on_btn_save_clicked() {
    QString s_recipeName        = ui->le_name->text();
    if(s_recipeName.length() < 1) {
        QMessageBox::warning(nullptr, "Recipe Name", "Please input recipe name.");
        return;
    } else {
        s_recipeName += ".ini";
    }
    QMap<QString, QString> recipe;
    int     i_acquirePeriod     = DataHelper::tmToSec(ui->te_time->time());
    QString s_emOpt             = ui->cb_emauto->isChecked() ? "On" : "Off";
    recipe.insert("EmOpt",      s_emOpt);
    recipe.insert("PressureUnit", QString::number(ui->cb_unitpressure->currentIndex()));
    recipe.insert("Peroid",     QString::number(i_acquirePeriod));
    recipe.insert("StartMass",  QString::number(ui->sb_start->value()));
    recipe.insert("StopMass",   QString::number(ui->sb_end->value()));
    recipe.insert("Points",     ui->le_points->text());
    recipe.insert("Method",     ui->cb_method->currentText());
    recipe.insert("Dwell",      ui->cb_dwell->currentText());
    recipe.insert("Flmt",       ui->cb_flmt->currentText());
    recipe.insert("PPAmu",      ui->cb_ppamu->currentText());
    recipe.insert("ReportUnit", ui->cb_unitreport->currentText());
    if(DataHelper::saveConf(recipe, s_recipeName, m_s_recipePath, "Recipe")) {
        QMessageBox::information(nullptr, "Success", "Recipe config saved.");
    } else {
        QMessageBox::warning(nullptr, "Save Failed", "Please check the settings.");
    }
    tblInit();
}

///
/// \brief RecipeDlg::tblInit
///
void RecipeDlg::tblInit() {
    ui->tbl_recipe->clear();
    m_slRecipes = DataHelper::listConfFile(m_s_recipePath);
    int i_recipeCount = m_slRecipes.length();
    QStringList tblHeader;
    tblHeader << tr("No.") << tr("Name");
    ui->tbl_recipe->setColumnCount(2);
    ui->tbl_recipe->setHorizontalHeaderLabels(tblHeader);
    ui->tbl_recipe->verticalHeader()->setVisible(false);
    ui->tbl_recipe->horizontalHeader()->resizeSection(0, 155);
//    ui->tbl_recipe->horizontalHeader()->resizeSection(1, 75);
    ui->tbl_recipe->horizontalHeader()->setStretchLastSection(true);
    ui->tbl_recipe->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbl_recipe->setRowCount(i_recipeCount);
    if(!i_recipeCount) {
        return;
    }
    for(int i = 0; i < i_recipeCount; ++i) {
        ui->tbl_recipe->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tbl_recipe->setItem(i, 1, new QTableWidgetItem(m_slRecipes.at(i)));
    }
}

///
/// \brief RecipeDlg::on_tbl_recipe_cellClicked
/// \param row
///
void RecipeDlg::on_tbl_recipe_cellClicked(int row, int) {
    m_iRow = row;
    QStringList sl_recipes = DataHelper::listConfFile(m_s_recipePath);
    QMap<QString, QString> qm_values = DataHelper::readConf(sl_recipes.at(row) + ".ini", m_s_recipePath, "Recipe");
    if(!qm_values.count()) {
        QMessageBox::warning(nullptr, "Read Failed", "No value readed.");
        return;
    }
    //read Method
    if(!qm_values.contains("Method")) {
        return;
    }
    QString s_method = qm_values.find("Method")->toStdString().c_str();
    //read Peroid
    if(!qm_values.contains("Peroid")) {
        return;
    }
    int i_period = qm_values.find("Peroid")->toInt();
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
    //read EmOpt
    if(!qm_values.contains("EmOpt")) {
        return;
    }
    QString s_emOpt = qm_values.find("EmOpt")->toStdString().c_str();
    //read Dwell
    if(!qm_values.contains("Dwell")) {
        return;
    }
    QString s_dwell = qm_values.find("Dwell")->toStdString().c_str();
    //read PPAmu
    if(!qm_values.contains("PPAmu")) {
        return;
    }
    QString s_ppamu = qm_values.find("PPAmu")->toStdString().c_str();
    //read Pressure unit
    if(!qm_values.contains("PressureUnit")) {
        return;
    }
    int i_pressureUnit = qm_values.find("PressureUnit")->toInt();
    //read Data Type
    if(!qm_values.contains("ReportUnit")) {
        return;
    }
    QString s_reportUnit = qm_values.find("ReportUnit")->toStdString().c_str();
    if(!qm_values.contains("Flmt")) {
        return;
    }
    QString s_flmt = qm_values.find("Flmt")->toStdString().c_str();
    bool b_isSweep  = s_method  == "Sweep";
    bool b_isEmOn   = s_emOpt   == "On";
    on_cb_method_currentIndexChanged(!b_isSweep);
    ui->cb_emauto->setChecked(b_isEmOn);
    ui->cb_method->setCurrentIndex(!b_isSweep);
    ui->te_time->setTime(DataHelper::secToTm(i_period));
    ui->le_name->setText(sl_recipes.at(row));
    ui->le_points->setText(s_points);
    ui->sb_start->setValue(i_startMass);
    ui->sb_end->setValue(i_stopMass);
    ui->cb_dwell->setCurrentText(s_dwell);
    ui->cb_flmt->setCurrentText(s_flmt);
    ui->cb_ppamu->setCurrentText(s_ppamu);
    ui->cb_unitpressure->setCurrentIndex(i_pressureUnit);
    ui->cb_unitreport->setCurrentText(s_reportUnit);
}

///
/// \brief RecipeDlg::on_btn_clear_clicked
///
//void RecipeDlg::on_btn_clear_clicked() {
//    QTime tm_init;
//    tm_init.setHMS(0, 10, 0);
//    ui->cb_emauto->setChecked(false);
//    ui->cb_method->setCurrentIndex(0);
//    ui->te_time->setTime(tm_init);
//    ui->le_name->setText("");
//    ui->le_points->setText("2/4/16/18/28/32/44");
//    ui->sb_start->setValue(0);
//    ui->sb_end->setValue(100);
//}


void RecipeDlg::on_btn_select_clicked() {
    DataHelper::save_last_config("", ui->le_name->text(), "");
    this->close();
}


void RecipeDlg::on_cb_run_currentIndexChanged(int index)
{

}

