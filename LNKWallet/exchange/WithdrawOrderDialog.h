#ifndef WITHDRAWORDERDIALOG_H
#define WITHDRAWORDERDIALOG_H

#include <QDialog>

namespace Ui {
class WithdrawOrderDialog;
}
class FeeChooseWidget;
class WithdrawOrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WithdrawOrderDialog(QWidget *parent = 0);
    ~WithdrawOrderDialog();

    bool pop();

    void setText(QString _text);
    void setContractFee(double _fee);
    void setAccountName(const QString &accountName);
    void updatePoundageID();
private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::WithdrawOrderDialog *ui;
    bool yesOrNo = false;
    FeeChooseWidget *feeChoose;
};

#endif // WITHDRAWORDERDIALOG_H
