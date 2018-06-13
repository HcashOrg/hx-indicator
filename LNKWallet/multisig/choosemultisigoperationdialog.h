#ifndef CHOOSETOKENOPERATIONDIALOG_H
#define CHOOSETOKENOPERATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ChooseMultiSigOperationDialog;
}

class ChooseMultiSigOperationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseMultiSigOperationDialog(QWidget *parent = 0);
    ~ChooseMultiSigOperationDialog();

    int  pop();

private slots:

    void on_detailBtn_clicked();

    void on_depositBtn_clicked();

    void on_sendOutBtn_clicked();

    void on_transactionBtn_clicked();

private:
    Ui::ChooseMultiSigOperationDialog *ui;
    int choice;         // 返回选择的值   1:查看详情  2:充值  3:转出   4:查看交易记录
};

#endif // CHOOSETOKENOPERATIONDIALOG_H
