#ifndef WITHDRAWMULTISIGDIALOG_H
#define WITHDRAWMULTISIGDIALOG_H

#include <QDialog>

namespace Ui {
class WithdrawMultiSigDialog;
}

class WithdrawMultiSigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WithdrawMultiSigDialog( QString _multiSigAddress, QWidget *parent = 0);
    ~WithdrawMultiSigDialog();

    void  pop();

private:
    QStringList contactsList;
    bool contactUpdating;
    void getContactsList();

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_multiSigAddressComboBox_currentIndexChanged(const QString &arg1);

    void on_selectPathBtn_clicked();

    void on_copyBtn_clicked();

    void on_closeBtn_clicked();

    void on_detailBtn_clicked();

    void on_sendtoLineEdit_textEdited(const QString &arg1);

    void on_closeBtn2_clicked();

    void on_contactComboBox_currentIndexChanged(int index);

private:
    Ui::WithdrawMultiSigDialog *ui;

    void init();
};

#endif // WITHDRAWMULTISIGDIALOG_H
