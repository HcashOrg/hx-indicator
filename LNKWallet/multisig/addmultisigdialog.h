#ifndef ADDMULTISIGDIALOG_H
#define ADDMULTISIGDIALOG_H

#include <QDialog>

namespace Ui {
class AddMultiSigDialog;
}

class AddMultiSigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddMultiSigDialog(QWidget *parent = 0);
    ~AddMultiSigDialog();

    void  pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void  on_multiSigAddressLineEdit_textEdited(const QString &arg1);

    void on_closeBtn_clicked();

private:
    Ui::AddMultiSigDialog *ui;
};

#endif // ADDMULTISIGDIALOG_H
