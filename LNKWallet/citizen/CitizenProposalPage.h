#ifndef CITIZENPROPOSALPAGE_H
#define CITIZENPROPOSALPAGE_H

#include <QWidget>
#include "wallet.h"
class PageScrollWidget;
namespace Ui {
class CitizenProposalPage;
}

class CitizenProposalPage : public QWidget
{
    Q_OBJECT

public:
    explicit CitizenProposalPage(QWidget *parent = 0);
    ~CitizenProposalPage();

    void init();

    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_proposalTableWidget_cellClicked(int row, int column);

    void on_changeSenatorBtn_clicked();

    void pageChangeSlot(unsigned int page);

private:
    Ui::CitizenProposalPage *ui;
    bool inited = false;

    void showProposals();

    void paintEvent(QPaintEvent*);
    PageScrollWidget *pageWidget;
    QString calProposalWeight(const ProposalInfo &info) const;
};

#endif // CITIZENPROPOSALPAGE_H
