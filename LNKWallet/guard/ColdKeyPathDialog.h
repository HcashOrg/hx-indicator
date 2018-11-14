#ifndef COLDKEYPATHDIALOG_H
#define COLDKEYPATHDIALOG_H

#include <QDialog>

namespace Ui {
class ColdKeyPathDialog;
}

class ColdKeyPathDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColdKeyPathDialog( QString _accountName, QWidget *parent = 0);
    ~ColdKeyPathDialog();

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
    Ui::ColdKeyPathDialog *ui;

    QString accountName;

    void checkOkBtnEnabled();
};

#endif // COLDKEYPATHDIALOG_H
