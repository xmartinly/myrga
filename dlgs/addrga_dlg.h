#ifndef ADDRGA_DLG_H
#define ADDRGA_DLG_H

#include <QDialog>

namespace Ui {
    class AddRgaDlg;
}

class AddRgaDlg : public QDialog {
    Q_OBJECT

  public:
    explicit AddRgaDlg(QWidget* parent = nullptr);
    ~AddRgaDlg();

    void set_rga_tag(const QString& tag);

  private slots:
    void on_btn_save_clicked();

    void on_tbl_rgas_cellClicked(int row, int);

    void on_btn_del_clicked();

    void on_btn_sel_clicked();

  private:
    Ui::AddRgaDlg* ui;
    int         m_iRow = -1;
    //rga list path
    QString     m_sRgaPath;
    //recipe list path
    QString     m_sRecipePath;
    QStringList m_slRgas;
    bool        m_bIsFromEdit = false;
    void        tbl_init();
//    void        initRecipeList();
    bool        check_conn();

    int         check_setting_conflict(QString tag, QString ip);
};

#endif // ADDRGA_DLG_H
