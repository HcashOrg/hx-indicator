#ifndef CITIZENRESOLUTIONDIALOG_H
#define CITIZENRESOLUTIONDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include "CitizenPolicyPage.h"

namespace Ui {
class CitizenResolutionDialog;
}

class CitizenResolutionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CitizenResolutionDialog(const ResolutionInfo& _info, QString _account, QWidget *parent = nullptr);
    ~CitizenResolutionDialog();

    void pop();

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void on_bgGroup_toggled(int id, bool status) ;

private:
    Ui::CitizenResolutionDialog *ui;
    ResolutionInfo info;
    QString account;

    QButtonGroup* bgGroup = nullptr;

    QString calProposalWeight(const ResolutionInfo &info, int option) const;

};

#endif // CITIZENRESOLUTIONDIALOG_H
