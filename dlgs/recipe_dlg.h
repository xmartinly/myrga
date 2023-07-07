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

    void tblInit();

    void on_tbl_recipe_cellClicked(int row, int);

    void on_btn_run_clicked();

  private:
    Ui::RecipeDlg* ui;

    bool m_method = false;

    int m_iRow = 0;
    QStringList m_slRecipes;
    QString m_s_recipePath;

  signals:
    void start_recipe(int dur);
};

#endif // RECIPE_DLG_H
