#ifndef PROPOSALDETAILDIALOG_H
#define PROPOSALDETAILDIALOG_H

#include <QDialog>
struct ProposalInfo;

namespace Ui {
class ProposalDetailDialog;
}

class ProposalDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProposalDetailDialog(QWidget *parent = 0);
    ~ProposalDetailDialog();

    void pop();

    void setProposal(QString _proposalId);
private slots:
    void on_infoBtn_clicked();

    void on_voteStateBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::ProposalDetailDialog *ui;
    QString calProposalWeight(const ProposalInfo &info)const;
};

#endif // PROPOSALDETAILDIALOG_H
