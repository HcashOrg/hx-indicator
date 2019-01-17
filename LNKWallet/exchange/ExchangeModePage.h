#ifndef EXCHANGEMODEPAGE_H
#define EXCHANGEMODEPAGE_H

#include <QWidget>
#include "wallet.h"

namespace Ui {
class ExchangeModePage;
}

struct OrderInfo
{
    unsigned long long baseAmount = 0;
    unsigned long long quoteAmount = 0;
    QString address;
    QString trxId;
    unsigned long long completedBaseAmount = 0;
    unsigned long long completedQuoteAmount = 0;
    unsigned long long lastDealBaseAmount = 0;
    unsigned long long lastDealQuoteAmount = 0;
};

class ExchangeModePage : public QWidget
{
    Q_OBJECT

public:
    explicit ExchangeModePage(QWidget *parent = nullptr);
    ~ExchangeModePage();

    void init();

signals:
    void backBtnVisible(bool isShow);
    void showOnchainOrderPage();

private slots:
    void jsonDataUpdated(QString id);

    void on_orderModeBtn_clicked();

    void on_favoriteMarketBtn_clicked();

    void on_marketBtn1_clicked();

    void on_marketBtn2_clicked();

    void on_marketBtn3_clicked();

    void onAccountComboBoxCurrentIndexChanged(const QString &arg1);

    void on_buyBtn_clicked();

    void on_sellBtn_clicked();


    void on_sellPositionTableWidget_cellPressed(int row, int column);

    void on_buyPositionTableWidget_cellPressed(int row, int column);

    void on_buyPriceLineEdit_textEdited(const QString &arg1);

    void on_buyAmountLineEdit_textEdited(const QString &arg1);

    void on_sellPriceLineEdit_textEdited(const QString &arg1);

    void on_sellAmountLineEdit_textEdited(const QString &arg1);

    void on_balanceBtn_clicked();

    void on_myOrdersBtn_clicked();

public:
    void getUserBalances();
    void getSellOrders(const ExchangePair& _pair);
    unsigned long long getMaxOrderAmount();
    QVector<OrderInfo>  sellOrdersVector;
    void getBuyOrders(const ExchangePair& _pair);
    QVector<OrderInfo>  buyOrdersVector;
    void mergeDepth(QVector<OrderInfo>& orders, const ExchangePair& pair);        // 合并深度

    void calculateAbleToBuy();
    void calculateAbleToSell();

    void estimateBuyValue();
    void estimateSellValue();
public slots:
    void onPairSelected(const ExchangePair& _pair);
    void onAddFavoriteClicked();

private:
    void showDepth();

private:
    Ui::ExchangeModePage *ui;

    void paintEvent(QPaintEvent*);

};

#endif // EXCHANGEMODEPAGE_H
