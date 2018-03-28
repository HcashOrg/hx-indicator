#ifndef CHOOSETOKENOPERATIONDIALOG_H
#define CHOOSETOKENOPERATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ChooseTokenOperationDialog;
}

class ChooseTokenOperationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseTokenOperationDialog(QWidget *parent = 0);
    ~ChooseTokenOperationDialog();

    int  pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_depositBtn_clicked();

    void on_destroyBtn_clicked();

    void on_cancelBtn_clicked();

    void on_mintBtn_clicked();

    void on_withdrawBtn_clicked();

private:
    Ui::ChooseTokenOperationDialog *ui;
    int choice;         // 返回选择的值  0:取消  1:充值  2:销毁  3:增发   4:提取合约余额
};

#endif // CHOOSETOKENOPERATIONDIALOG_H
