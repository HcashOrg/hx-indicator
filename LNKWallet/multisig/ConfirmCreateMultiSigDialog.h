#ifndef CONFIRMCREATEMULTISIGDIALOG_H
#define CONFIRMCREATEMULTISIGDIALOG_H

#include <QDialog>

namespace Ui {
class ConfirmCreateMultiSigDialog;
}

class FeeChooseWidget;

class ConfirmCreateMultiSigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfirmCreateMultiSigDialog( QString account, QWidget *parent = 0);
    ~ConfirmCreateMultiSigDialog();

    void pop();

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_pwdLineEdit_textChanged(const QString &arg1);

    void onAccountComboBoxCurrentIndexChanged(const QString &arg1);

private:
    Ui::ConfirmCreateMultiSigDialog *ui;
    FeeChooseWidget* feeWidget = NULL;
};

#endif // CONFIRMCREATEMULTISIGDIALOG_H
