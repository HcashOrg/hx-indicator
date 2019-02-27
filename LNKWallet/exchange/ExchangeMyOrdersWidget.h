#ifndef EXCHANGEMYORDERSWIDGET_H
#define EXCHANGEMYORDERSWIDGET_H

#include <QWidget>
#include "ExchangeModePage.h"
#include "extra/HttpManager.h"

namespace Ui {
class ExchangeMyOrdersWidget;
}

class BottomLine;
class PageScrollWidget;

struct HistoryOrderInfo
{
    QString trxId;
    long long currentBaseAmount = 0;
    long long currentQuoteAmount = 0;
    unsigned long long originBaseAmount = 0;
    unsigned long long originQuoteAmount = 0;
    ExchangePair pair;
    QString type;
    int state = 0;
    QDateTime dateTime;
};

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

//    void on_marketBtn2_clicked();

    void on_marketBtn3_clicked();

    void on_myOrdersTableWidget_cellClicked(int row, int column);

    void on_currentOrdersBtn_clicked();

    void on_historyBtn_clicked();

private:
    void queryOrders(int page, int pageCount, QString id);
    void showHistoryOrders();
    int recordCount = 0;
    int currentSliderPos = 0;
    int currentPage = 0;
    HttpManager httpManager;
    QVector<HistoryOrderInfo> historyOrders;
private slots:
    void pageChangeSlot(unsigned int page);
    void httpReplied(QByteArray _data, int _status);
//    void onSliderValueChanged(int _value);

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
    BottomLine* bottomLine = nullptr;
    PageScrollWidget *pageWidget = nullptr;

    void paintEvent(QPaintEvent*);

};

#endif // EXCHANGEMYORDERSWIDGET_H
