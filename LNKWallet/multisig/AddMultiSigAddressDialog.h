#ifndef ADDMULTISIGADDRESSDIALOG_H
#define ADDMULTISIGADDRESSDIALOG_H

#include <QDialog>

namespace Ui {
class AddMultiSigAddressDialog;
}

class AddMultiSigAddressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddMultiSigAddressDialog(QWidget *parent = 0);
    ~AddMultiSigAddressDialog();

    void pop();
    QString multiSigAddress;

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void on_addressLineEdit_textEdited(const QString &arg1);

private:
    Ui::AddMultiSigAddressDialog *ui;
};

#endif // ADDMULTISIGADDRESSDIALOG_H
