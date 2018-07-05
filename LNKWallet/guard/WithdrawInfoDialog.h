#ifndef WITHDRAWINFODIALOG_H
#define WITHDRAWINFODIALOG_H

#include <QDialog>

namespace Ui {
class WithdrawInfoDialog;
}

class WithdrawConfirmPage;

class WithdrawInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WithdrawInfoDialog( WithdrawConfirmPage* _page, QWidget *parent = 0);
    ~WithdrawInfoDialog();

    void pop();

    void setTrxId(QString _trxId);


private slots:
    void on_closeBtn_clicked();

private:
    Ui::WithdrawInfoDialog *ui;

    WithdrawConfirmPage* page = NULL;
};

#endif // WITHDRAWINFODIALOG_H
