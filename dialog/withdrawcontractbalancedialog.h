#ifndef WITHDRAWCONTRACTBALANCEDIALOG_H
#define WITHDRAWCONTRACTBALANCEDIALOG_H

#include <QDialog>

namespace Ui {
class WithdrawContractBalanceDialog;
}

class WithdrawContractBalanceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WithdrawContractBalanceDialog( QString address, QWidget *parent = 0);
    ~WithdrawContractBalanceDialog();

    void pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_amountLineEdit_textEdited(const QString &arg1);

    void on_closeBtn_clicked();

private:
    Ui::WithdrawContractBalanceDialog *ui;
    QString contractAddress;

    void init();
};

#endif // WITHDRAWCONTRACTBALANCEDIALOG_H
