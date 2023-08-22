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
    recipe_config_path = DataHelper::get_file_folder("recipes");
    recipe_tbl_init();
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
    if(row_count < 0) {
        return;
    }
    QString s_fileName = recipe_list.at(row_count) + ".ini";
    DataHelper::del_config_file(s_fileName, recipe_config_path);
    recipe_tbl_init();
    row_count = -1;
}

///
/// \brief RecipeDlg::on_btn_save_clicked. Save recipe to config file.
///
void RecipeDlg::on_btn_save_clicked() {
    recipe_save();
    recipe_tbl_init();
}

///
/// \brief RecipeDlg::tblInit
///
void RecipeDlg::recipe_tbl_init() {
    ui->tbl_recipe->clear();
    recipe_list = DataHelper::list_config_file(recipe_config_path);
    int i_recipeCount = recipe_list.length();
    QStringList tblHeader;
    tblHeader << u8"序号" << u8"标签";
    ui->tbl_recipe->setColumnCount(2);
    ui->tbl_recipe->setHorizontalHeaderLabels(tblHeader);
    ui->tbl_recipe->verticalHeader()->setVisible(false);
    ui->tbl_recipe->horizontalHeader()->resizeSection(0, 155);
    ui->tbl_recipe->horizontalHeader()->setStretchLastSection(true);
    ui->tbl_recipe->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbl_recipe->setRowCount(i_recipeCount);
    if(!i_recipeCount) {
        return;
    }
    for(int i = 0; i < i_recipeCount; ++i) {
        ui->tbl_recipe->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tbl_recipe->setItem(i, 1, new QTableWidgetItem(recipe_list.at(i)));
    }
}

///
/// \brief RecipeDlg::on_tbl_recipe_cellClicked
/// \param row
///
void RecipeDlg::on_tbl_recipe_cellClicked(int row, int) {
    row_count = row;
    QStringList sl_recipes = DataHelper::list_config_file(recipe_config_path);
    QMap<QString, QString> qm_values = DataHelper::read_config(sl_recipes.at(row) + ".ini", recipe_config_path, "Recipe");
    if(!qm_values.count()) {
        QMessageBox::warning(nullptr, u8"读取错误", u8"未读取到数据");
        return;
    }
    //read Method
    if(!qm_values.contains("Method")) {
        return;
    }
    int i_method = qm_values.find("Method")->toInt();
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
    int i_emOpt = qm_values.find("EmOpt")->toInt();
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
    int i_flmt = qm_values.find("Flmt")->toInt();
    on_cb_method_currentIndexChanged(!i_method);
    ui->cb_emauto->setChecked(i_emOpt);
    ui->cb_method->setCurrentIndex(i_method);
    ui->te_time->setTime(DataHelper::sec_to_tm(i_period));
    ui->le_name->setText(sl_recipes.at(row));
    ui->le_points->setText(s_points);
    ui->sb_start->setValue(i_startMass);
    ui->sb_end->setValue(i_stopMass);
    ui->cb_dwell->setCurrentIndex(i_dwell);
    ui->cb_flmt->setCurrentIndex(i_flmt);
    ui->cb_ppamu->setCurrentIndex(i_ppamu);
    ui->cb_unitpressure->setCurrentIndex(i_pressureUnit);
    ui->cb_unitreport->setCurrentIndex(i_reportUnit);
}


void RecipeDlg::on_btn_run_clicked() {
    recipe_save(true);
    emit start_recipe(ui->cb_run->currentIndex());
    this->close();
}

void RecipeDlg::recipe_save(bool is_run) {
    QString s_recipeName        =  ui->le_name->text();
    if(s_recipeName.length() < 1) {
        QMessageBox::warning(nullptr, u8"保存错误", u8"请输入配方标签");
        return;
    } else {
        s_recipeName += ".ini";
    }
    if(is_run) {
        s_recipeName = "lastrun.ini";
    }
    QString file_path = DataHelper::get_file_folder(is_run ? "" : "recipes");
    QMap<QString, QString> recipe;
    int     i_acquirePeriod     = DataHelper::tm_to_sec(ui->te_time->time());
    QString s_emOpt             = ui->cb_emauto->isChecked() ? "1" : "0";
    recipe = DataHelper::gen_recipe_config(
                 s_emOpt,
                 QString::number(ui->cb_unitpressure->currentIndex()),
                 QString::number(ui->sb_start->value()),
                 QString::number(ui->sb_end->value()),
                 ui->le_points->text(),
                 QString::number(ui->cb_method->currentIndex()),
                 QString::number(ui->cb_dwell->currentIndex()),
                 QString::number(ui->cb_flmt->currentIndex()),
                 QString::number(ui->cb_ppamu->currentIndex()),
                 QString::number(ui->cb_unitreport->currentIndex()),
                 QString::number(i_acquirePeriod));
    if(DataHelper::save_config(recipe, s_recipeName, file_path, "Recipe")) {
        if(is_run) {
            return;
        }
        QMessageBox::information(nullptr, u8"成功", u8"配方已保存");
    } else {
        QMessageBox::warning(nullptr, u8"失败", u8"保存失败, 请检查设置.");
    }
}

