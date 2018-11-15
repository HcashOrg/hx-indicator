#ifndef CITIZENPROPOSALPAGE_H
#define CITIZENPROPOSALPAGE_H

#include <QWidget>
class PageScrollWidget;
struct ProposalInfo;
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

    QString calProposalWeight(const ProposalInfo &info)const;
private:
    Ui::CitizenProposalPage *ui;
    bool inited = false;

    void showProposals();

    void paintEvent(QPaintEvent*);
    PageScrollWidget *pageWidget;
};

#endif // CITIZENPROPOSALPAGE_H
