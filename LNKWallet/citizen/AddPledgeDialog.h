#ifndef ADDPLEDGEDIALOG_H
#define ADDPLEDGEDIALOG_H

#include <QDialog>
#include "wallet.h"

namespace Ui {
class AddPledgeDialog;
}

class AddPledgeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPledgeDialog(const QString &accountName,const ProposalInfo &proposalInfo,QWidget *parent = 0);
    ~AddPledgeDialog();
private slots:
    void AddPledgeSlots();
    void jsonDataUpdated(QString id);
private:
    void InitWidget();
    void InitData();
private:
    Ui::AddPledgeDialog *ui;
    QString accountName;
    ProposalInfo proposalInfo;
    void installDoubleValidator(QLineEdit *line, double mi, double ma, int pre);
};

#endif // ADDPLEDGEDIALOG_H
