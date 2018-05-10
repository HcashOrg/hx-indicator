#ifndef DEPOSITEXCHANGECONTRACTDIALOG_H
#define DEPOSITEXCHANGECONTRACTDIALOG_H

#include <QDialog>

namespace Ui {
class DepositExchangeContractDialog;
}

class DepositExchangeContractDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DepositExchangeContractDialog(QWidget *parent = 0);
    ~DepositExchangeContractDialog();

    void  pop();

    void init();

    void setCurrentAsset(QString _assetSymbol);

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_closeBtn_clicked();

    void on_amountLineEdit_textChanged(const QString &arg1);

private:
    Ui::DepositExchangeContractDialog *ui;
    int stepCount = 0;      // 合约执行步数

    void estimateContractFee();
};

#endif // DEPOSITEXCHANGECONTRACTDIALOG_H
