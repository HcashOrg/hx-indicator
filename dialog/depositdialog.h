#ifndef DEPOSITDIALOG_H
#define DEPOSITDIALOG_H

#include <QDialog>

namespace Ui {
class DepositDialog;
}

class DepositDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DepositDialog( QString address, QWidget *parent = 0);
    ~DepositDialog();

    void pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_amountLineEdit_textEdited(const QString &arg1);

    void on_closeBtn_clicked();

private:
    Ui::DepositDialog *ui;
    QString contractAddress;

    void init();
    void checkIsFeeEnough();
};

#endif // DEPOSITDIALOG_H
