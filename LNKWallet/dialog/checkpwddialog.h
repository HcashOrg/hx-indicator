#ifndef CHECKPWDDIALOG_H
#define CHECKPWDDIALOG_H

#include <QDialog>

namespace Ui {
class CheckPwdDialog;
}

class CheckPwdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckPwdDialog(QWidget *parent = 0);
    ~CheckPwdDialog();

    bool  pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pwdLineEdit_textEdited(const QString &arg1);

    void on_pwdLineEdit_returnPressed();

    void on_closeBtn_clicked();

private:
    Ui::CheckPwdDialog *ui;

    bool yesOrNo;
};

#endif // CHECKPWDDIALOG_H
