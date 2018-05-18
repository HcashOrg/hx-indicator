#ifndef MINERPAGE_H
#define MINERPAGE_H

#include <QWidget>

namespace Ui {
class MinerPage;
}

class PageScrollWidget;
class QTableWidget;
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

    void pageChangeSlot(unsigned int page);
    void on_incomeRecordTableWidget_cellPressed(int row, int column);

private:
    Ui::MinerPage *ui;

    void paintEvent(QPaintEvent*);
    void init();

    void fetchLockBalance();
    void fetchAccountIncome();
    void showIncomeRecord();

    void InitStyle();
    void updateCheckState(int number);//0 1 2

    unsigned int calPage(const QTableWidget *const table)const;
    void setTextCenter(QTableWidget *const table);

    PageScrollWidget *pageWidget_income;
    PageScrollWidget *pageWidget_fore;
    PageScrollWidget *pageWidget_record;
};

#endif // MINERPAGE_H
