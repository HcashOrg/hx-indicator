#ifndef MYEXCHANGECONTRACTPAGE_H
#define MYEXCHANGECONTRACTPAGE_H

#include <QWidget>
#include <QMap>

namespace Ui {
class MyExchangeContractPage;
}


struct OrderAmount
{
    unsigned long long sellAmount;
    unsigned long long buyAmount;
};
typedef QMap<QString,QVector<OrderAmount>>   SellOrders;        // key 是 "资产1,资产2" 格式的字符串

class MyExchangeContractPage : public QWidget
{
    Q_OBJECT

public:
    explicit MyExchangeContractPage(QWidget *parent = 0);
    ~MyExchangeContractPage();

    void init();

    void refresh();

private:
    QMap<QString,SellOrders> accountSellOrdersMap;
    void showOrders();
    void showContractBalances();

private slots:
    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_assetComboBox2_currentIndexChanged(const QString &arg1);

    void on_sellBtn_clicked();

    void on_balanceBtn_clicked();

    void on_myOrdersBtn_clicked();

    void on_balancesTableWidget_cellPressed(int row, int column);

    void on_ordersTableWidget_cellPressed(int row, int column);

    void on_withdrawAllBtn_clicked();

private:
    Ui::MyExchangeContractPage *ui;
    bool inited = false;

    void paintEvent(QPaintEvent*);
};

#endif // MYEXCHANGECONTRACTPAGE_H
