#ifndef CITIZENACCOUNTPAGE_H
#define CITIZENACCOUNTPAGE_H

#include <QWidget>
class PageScrollWidget;
namespace Ui {
class CitizenAccountPage;
}

class CitizenAccountPage : public QWidget
{
    Q_OBJECT

public:
    explicit CitizenAccountPage(QWidget *parent = 0);
    ~CitizenAccountPage();

    void init();

    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_newCitizenBtn_clicked();

    void on_startMineBtn_clicked();

    void on_changeFeeBtn_clicked();

    void pageChangeSlot(unsigned int page);
private:
    Ui::CitizenAccountPage *ui;
    bool inited = false;

    void paintEvent(QPaintEvent*);
    void showLockBalance();
    PageScrollWidget *pageWidget;
};

#endif // CITIZENACCOUNTPAGE_H
