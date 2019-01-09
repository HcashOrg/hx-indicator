#ifndef EXCHANGEMODEWIDGET_H
#define EXCHANGEMODEWIDGET_H

#include <QWidget>
#include "wallet.h"


namespace Ui {
class ExchangeModeWidget;
}

struct ExchangeBalance
{
    unsigned long long locked = 0;
    unsigned long long available = 0;
};

struct OrderInfo
{
    unsigned long long baseAmount = 0;
    unsigned long long quoteAmount = 0;
    QString address;
    QString trxId;
};

class ExchangeModeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExchangeModeWidget(QWidget *parent = nullptr);
    ~ExchangeModeWidget();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void on_orderModeBtn_clicked();

    void on_favoriteMarketBtn_clicked();

    void on_marketBtn1_clicked();

    void on_marketBtn2_clicked();

    void on_marketBtn3_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_buyBtn_clicked();

    void on_sellBtn_clicked();


private:
    void getExchangePairs();

    void getUserBalances();
    QMap<QString,ExchangeBalance>   assetExchangeBalanceMap;
    void getSellOrders(const ExchangePair& _pair);
    QVector<OrderInfo>  sellOrdersVector;
    void getBuyOrders(const ExchangePair& _pair);
    QVector<OrderInfo>  buyOrdersVector;

public slots:
    void onPairSelected(const ExchangePair& _pair);

private:
    Ui::ExchangeModeWidget *ui;

    void paintEvent(QPaintEvent*);

};

#endif // EXCHANGEMODEWIDGET_H
