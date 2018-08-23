#ifndef CITIZENPROPOSALPAGE_H
#define CITIZENPROPOSALPAGE_H

#include <QWidget>

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

private:
    Ui::CitizenProposalPage *ui;
    bool inited = false;

    void showProposals();

    void paintEvent(QPaintEvent*);
};

#endif // CITIZENPROPOSALPAGE_H
