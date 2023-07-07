#ifndef RECIPE_DLG_H
#define RECIPE_DLG_H

#include <QDialog>

namespace Ui {
    class RecipeDlg;
}

class RecipeDlg : public QDialog {
    Q_OBJECT

  public:
    explicit RecipeDlg(QWidget* parent = nullptr);
    ~RecipeDlg();

  private slots:

    void on_cb_method_currentIndexChanged(int index);

    void on_btn_del_clicked();

    void on_btn_save_clicked();

    void recipe_tbl_init();

    void on_tbl_recipe_cellClicked(int row, int);

    void on_btn_run_clicked();

    void recipe_save(bool is_run = false);

  private:
    Ui::RecipeDlg* ui;

    bool scan_method = false;
    QString recipe_to_run = "";
    int row_count = 0;
    QStringList recipe_list;
    QString recipe_config_path;

  signals:
    void start_recipe(int dur);
};

#endif // RECIPE_DLG_H
