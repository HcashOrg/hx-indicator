#ifndef LOCKFUNDDIALOG_H
#define LOCKFUNDDIALOG_H

#include <QDialog>

namespace Ui {
class LockFundDialog;
}

class FeeChooseWidget;
class LockFundDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LockFundDialog(QWidget *parent = nullptr);
    ~LockFundDialog();

    void  pop();

    void init();

    void setAccount(QString accountName);
private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_assetComboBox_currentIndexChanged(const QString &arg1);


    void on_amountLineEdit_textEdited(const QString &arg1);

    void on_pwdLineEdit_textChanged(const QString &arg1);

private:
    Ui::LockFundDialog *ui;
    FeeChooseWidget *feeChoose = NULL;
    int stepCount = 0;      // 合约执行步数

    void resetAmountLineEdit();
    void checkOkBtnEnabled();
};

#endif // LOCKFUNDDIALOG_H
