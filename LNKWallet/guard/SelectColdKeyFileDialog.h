#ifndef SELECTCOLDKEYFILEDIALOG_H
#define SELECTCOLDKEYFILEDIALOG_H

#include <QDialog>

namespace Ui {
class SelectColdKeyFileDialog;
}

class SelectColdKeyFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectColdKeyFileDialog(QWidget *parent = 0);
    ~SelectColdKeyFileDialog();

    void pop();
    QString filePath;
    QString pwd;
private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void on_pathBtn_clicked();

    void on_pkFileLineEdit_textChanged(const QString &arg1);

    void on_pwdLineEdit_textChanged(const QString &arg1);

private:
    Ui::SelectColdKeyFileDialog *ui;

    void checkOkBtnEnabled();
};

#endif // SELECTCOLDKEYFILEDIALOG_H
