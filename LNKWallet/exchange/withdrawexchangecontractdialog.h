#ifndef WITHDRAWEXCHANGECONTRACTDIALOG_H
#define WITHDRAWEXCHANGECONTRACTDIALOG_H

#include <QDialog>

namespace Ui {
class WithdrawExchangeContractDialog;
}
class FeeChooseWidget;
class WithdrawExchangeContractDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WithdrawExchangeContractDialog( bool _isExchangeMode = false, QWidget *parent = 0);
    ~WithdrawExchangeContractDialog();

    void pop();

    void init();

    void setCurrentAsset(QString _assetSymbol);

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_withdrawAllBtn_clicked();

    void on_closeBtn_clicked();

    void on_amountLineEdit_textChanged(const QString &arg1);

private:
    Ui::WithdrawExchangeContractDialog *ui;
    bool isExchangeMode = nullptr;
    int stepCount = 0;      // 合约执行步数

    void estimateContractFee();
    FeeChooseWidget *feeChoose;
};

#endif // WITHDRAWEXCHANGECONTRACTDIALOG_H
