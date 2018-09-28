#ifndef CREATECITIZENDIALOG_H
#define CREATECITIZENDIALOG_H

#include <QDialog>

namespace Ui {
class CreateCitizenDialog;
}

class CreateCitizenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateCitizenDialog(QWidget *parent = 0);
    ~CreateCitizenDialog();

    void  pop();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void on_closeBtn_clicked();

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_registerLabel_linkActivated(const QString &link);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_okBtn2_clicked();

    void on_pwdLineEdit_textEdited(const QString &arg1);

    void on_cancelBtn2_clicked();

private:
    Ui::CreateCitizenDialog *ui;
};

#endif // CREATECITIZENDIALOG_H
