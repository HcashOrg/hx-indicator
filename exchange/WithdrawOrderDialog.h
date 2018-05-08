#ifndef WITHDRAWORDERDIALOG_H
#define WITHDRAWORDERDIALOG_H

#include <QDialog>

namespace Ui {
class WithdrawOrderDialog;
}

class WithdrawOrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WithdrawOrderDialog(QWidget *parent = 0);
    ~WithdrawOrderDialog();

    void pop();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::WithdrawOrderDialog *ui;
};

#endif // WITHDRAWORDERDIALOG_H
