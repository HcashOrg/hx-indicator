#ifndef MINERPAGE_H
#define MINERPAGE_H

#include <QWidget>

namespace Ui {
class MinerPage;
}

class PageScrollWidget;
class QTableWidget;
class BlankDefaultWidget;
class MinerPage : public QWidget
{
    Q_OBJECT

public:
    explicit MinerPage(QWidget *parent = 0);
    ~MinerPage();

    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_registerBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_lockToMinerBtn_clicked();

    void on_lockBalancesTableWidget_cellPressed(int row, int column);

    void on_incomeTableWidget_cellPressed(int row, int column);

    void on_incomeInfoBtn_clicked();

    void on_forecloseInfoBtn_clicked();

    void on_incomeRecordBtn_clicked();

    void on_citizenInfoBtn_clicked();

    void pageChangeSlot(unsigned int page);

    void on_incomeRecordTableWidget_cellPressed(int row, int column);

    void on_obtainAllBtn_clicked();

    void on_forecloseAllBtn_clicked();

    void on_sortTypeComboBox_currentIndexChanged(const QString &arg1);


private:
    QString getCitizenTooltip(const QString &citizenName);
    void modifyStringLength(QString & first,QString &second,bool fillAppend = true);//fillAppend=true表示在后面补全空格
private:
    Ui::MinerPage *ui;

    void paintEvent(QPaintEvent*);
    void init();
    void updateAccounts();
    bool accountsUpdating = false;

    void fetchLockBalance();
    void fetchAccountIncome();
    void showIncomeRecord();
    void showCitizenInfo();

    void InitStyle();
    void updateCheckState(int number);//0 1 2

    void checkBtnVisible();

    int count = 0;
    void autoLockToCitizen();
    void autoObtain();

    unsigned int calPage(const QTableWidget *const table)const;

    BlankDefaultWidget *blankWidget_income = NULL;
    BlankDefaultWidget *blankWidget_fore = NULL;
    BlankDefaultWidget *blankWidget_record = NULL;
    BlankDefaultWidget *blankWidget_citizen = NULL;
    PageScrollWidget *pageWidget_income = NULL;
    PageScrollWidget *pageWidget_fore = NULL;
    PageScrollWidget *pageWidget_record = NULL;
    PageScrollWidget *pageWidget_citizen = NULL;
};

#endif // MINERPAGE_H
