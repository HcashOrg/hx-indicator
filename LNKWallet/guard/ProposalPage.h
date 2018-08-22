#ifndef PROPOSALPAGE_H
#define PROPOSALPAGE_H

#include <QWidget>
#include <QLabel>

#include "poundage/PageScrollWidget.h"
#include "control/BlankDefaultWidget.h"
namespace Ui {
class ProposalPage;
}

class ProposalPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProposalPage(QWidget *parent = 0);
    ~ProposalPage();

    void init();

    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_proposalTableWidget_cellClicked(int row, int column);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_proposalTableWidget_cellPressed(int row, int column);

    void pageChangeSlot(unsigned int page);
private:
    Ui::ProposalPage *ui;

    void showProposals();

    void paintEvent(QPaintEvent*);
    PageScrollWidget *pageWidget;
    BlankDefaultWidget *blankWidget;
};

class VoteStateLabel : public QLabel
{
    Q_OBJECT

public:
    VoteStateLabel(QWidget *parent = 0);
    ~VoteStateLabel();

    void setVoteNum(int approve, int disapprove, int total);
};

#endif // PROPOSALPAGE_H
