#ifndef LOCKCONTRACTWITHDRAWDIALOG_H
#define LOCKCONTRACTWITHDRAWDIALOG_H

#include <QDialog>

namespace Ui {
class LockContractWithdrawDialog;
}

class FeeChooseWidget;
class LockContractWithdrawDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LockContractWithdrawDialog( QString _accountName, QString _asset, QWidget *parent = nullptr);
    ~LockContractWithdrawDialog();

    QString m_accountName;
    QString m_asset;
    unsigned long long m_amount = 0;
    void  pop();

    void init();

    void setMaxAmount(unsigned long long _amount);

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void on_withdrawAllBtn_clicked();

    void on_amountLineEdit_textChanged(const QString &arg1);

    void on_pwdLineEdit_textChanged(const QString &arg1);

private:
    Ui::LockContractWithdrawDialog *ui;
    FeeChooseWidget *feeChoose = NULL;
    int stepCount = 0;      // 合约执行步数

    void checkOkBtnEnabled();
};

#endif // LOCKCONTRACTWITHDRAWDIALOG_H
