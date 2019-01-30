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
class BlankDefaultWidget;
class PageScrollWidget;
class MyExchangeContractPage : public QWidget
{
    Q_OBJECT

public:
    explicit MyExchangeContractPage(QWidget *parent = 0);
    ~MyExchangeContractPage();

    void init();

    void refresh();

    void onBack();

signals:
    void backBtnVisible(bool isShow);
    void showOnchainOrderPage();

private:
    QMap<QString,SellOrders> accountSellOrdersMap;
    void showOrders();
    void updateTableHeaders();

    void fetchAccountExchangeContractBalances();

private slots:
    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_assetComboBox2_currentIndexChanged(const QString &arg1);

    void on_sellBtn_clicked();

    void on_registerBtn_clicked();

    void on_balanceBtn_clicked();

    void on_withdrawAllBtn_clicked();

    void onItemClicked(int _row, int _column);

    void pageChangeSlot(unsigned int page);
    void on_swapBtn_clicked();

    void on_onchainOrderBtn_clicked();

private:
    Ui::MyExchangeContractPage *ui;
    bool inited = false;
    QWidget* currentWidget;

    void paintEvent(QPaintEvent*);
    void registerContract();

    BlankDefaultWidget *blankWidget;
    PageScrollWidget *pageWidget;
};

#endif // MYEXCHANGECONTRACTPAGE_H
