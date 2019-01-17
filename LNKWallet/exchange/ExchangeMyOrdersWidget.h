#ifndef EXCHANGEMYORDERSWIDGET_H
#define EXCHANGEMYORDERSWIDGET_H

#include <QWidget>
#include "ExchangeModePage.h"

namespace Ui {
class ExchangeMyOrdersWidget;
}


class ExchangeMyOrdersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExchangeMyOrdersWidget(QWidget *parent = nullptr);
    ~ExchangeMyOrdersWidget();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void on_favoriteMarketBtn_clicked();

    void on_marketBtn1_clicked();

    void on_marketBtn2_clicked();

    void on_marketBtn3_clicked();

    void on_myOrdersTableWidget_cellClicked(int row, int column);

public:
    void getUserOrders(const ExchangePair& _pair);   // 0: sell  1: buy
    QMap<QString,OrderInfo>     mySellOrdersMap;    // key是交易id
    QMap<QString,OrderInfo>     myBuyOrdersMap;

    void showMyOrders();

public slots:
    void onPairSelected(const ExchangePair& _pair);
    void onAddFavoriteClicked();

private:
    Ui::ExchangeMyOrdersWidget *ui;

    void paintEvent(QPaintEvent*);

};

#endif // EXCHANGEMYORDERSWIDGET_H
