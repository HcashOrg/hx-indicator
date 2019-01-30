#ifndef EXCHANGECONTRACTFEEDIALOG_H
#define EXCHANGECONTRACTFEEDIALOG_H

#include <QDialog>

namespace Ui {
class ExchangeContractFeeDialog;
}

class FeeChooseWidget;
class ExchangeContractFeeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExchangeContractFeeDialog( unsigned long long _feeAmount, QString _accountName, QWidget *parent = nullptr);
    ~ExchangeContractFeeDialog();

    bool  pop();

    void init();

    void updatePoundageID();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::ExchangeContractFeeDialog *ui;
    bool yesOrNo = false;

    unsigned long long feeAmount = 0;
    QString account;
    FeeChooseWidget *feeChoose = nullptr;
};

#endif // EXCHANGECONTRACTFEEDIALOG_H
