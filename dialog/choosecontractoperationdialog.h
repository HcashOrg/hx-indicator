#ifndef CHOOSECONTRACTOPERATIONDIALOG_H
#define CHOOSECONTRACTOPERATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ChooseContractOperationDialog;
}

class ChooseContractOperationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseContractOperationDialog(QWidget *parent = 0);
    ~ChooseContractOperationDialog();

    int choice;         // 返回选择的值  0:取消  1:充值  2:销毁  3:增发   4:提取合约余额

private slots:
    void on_depositBtn_clicked();

    void on_mintBtn_clicked();

    void on_destroyBtn_clicked();

    void on_withdrawBtn_clicked();

private:
    Ui::ChooseContractOperationDialog *ui;

    void paintEvent(QPaintEvent*e);
};

#endif // CHOOSECONTRACTOPERATIONDIALOG_H
