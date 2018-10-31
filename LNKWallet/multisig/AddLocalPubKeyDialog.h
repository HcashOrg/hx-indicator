#ifndef ADDLOCALPUBKEYDIALOG_H
#define ADDLOCALPUBKEYDIALOG_H

#include <QDialog>
#include "CreateMultisigWidget.h"

namespace Ui {
class AddLocalPubKeyDialog;
}

class AddLocalPubKeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLocalPubKeyDialog(QWidget *parent = 0);
    ~AddLocalPubKeyDialog();

    bool pop();
    QStringList addedAccounts;
    void setAddedPubKeys(QVector<AccountPubKey>& accountPubKeys);

private:
    void showLocalAccountPubKeys();

private slots:
    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_cancelBtn_clicked();

    void on_tableWidget_cellClicked(int row, int column);

private:
    Ui::AddLocalPubKeyDialog *ui;
    bool yesOrNo = false;
};

#endif // ADDLOCALPUBKEYDIALOG_H
