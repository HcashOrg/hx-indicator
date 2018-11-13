#ifndef AUTOWITHDRAWSETDIALOG_H
#define AUTOWITHDRAWSETDIALOG_H

#include <QDialog>

namespace Ui {
class AutoWithdrawSetDialog;
}

class AutoWithdrawSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoWithdrawSetDialog(QWidget *parent = 0);
    ~AutoWithdrawSetDialog();

    void pop();

    void showLimitAmount();
    void save();
private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::AutoWithdrawSetDialog *ui;
};

#endif // AUTOWITHDRAWSETDIALOG_H
