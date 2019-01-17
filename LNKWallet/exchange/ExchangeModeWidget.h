#ifndef EXCHANGEMODEWIDGET_H
#define EXCHANGEMODEWIDGET_H

#include <QWidget>
#include "wallet.h"

namespace Ui {
class ExchangeModeWidget;
}

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

public:
    void getUserBalances();
    void getSellOrders(const ExchangePair& _pair);
    unsigned long long getMaxOrderAmount();
    QVector<OrderInfo>  sellOrdersVector;
    void getBuyOrders(const ExchangePair& _pair);
    QVector<OrderInfo>  buyOrdersVector;
    void mergeDepth(QVector<OrderInfo>& orders, const ExchangePair& pair);        // 合并深度
    int getExchangePairPrecision(const ExchangePair& pair);    // 返回交易对显示价格时的小数位数
    int getExchangeAmountPrecision(QString assetSymbol);

    void calculateAbleToBuy();
    void calculateAbleToSell();

    void estimateBuyValue();
    void estimateSellValue();
public slots:
    void onPairSelected(const ExchangePair& _pair);
    void onAddFavoriteClicked();

private:
    Ui::ExchangeModeWidget *ui;

    void paintEvent(QPaintEvent*);

};

#endif // EXCHANGEMODEWIDGET_H
