#ifndef KLINEWIDGET_H
#define KLINEWIDGET_H

#include <QWidget>

#include "wallet.h"
#include "extra/qcustomplot.h"
#include "extra/HttpManager.h"

namespace Ui {
class KLineWidget;
}

struct KPointInfo
{
    unsigned long long baseAmount = 0;
    unsigned long long quoteAmount = 0;
    int blockNum = 0;
    QString pairStr;
    double kOpen = 0;
    double kClose = 0;
    double kHigh = 0;
    double kLow = 0;
    QString dateTime;
};

struct ExchangeDeal
{
    QString address;
    unsigned long long baseAmount = 0;
    unsigned long long quoteAmount = 0;
    QString pairStr;
    int blockNum = 0;
    QString type;
    QString dateTime;
};

class KLineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KLineWidget(QWidget *parent = nullptr);
    ~KLineWidget();

    void init();

private:
    void queryRecentDeals(int count);
    void queryKLineData(int type, int count);
    void drawKLine();
    void showRecentDeals();
    const KPointInfo& getKPointInfoByTime(uint time_t, uint interval);   // 查找time_t属于哪个时间段
    HttpManager httpManager;
    QMap<uint,KPointInfo>   kPointMap;
    QVector<ExchangeDeal>   deals;
private slots:
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void httpReplied(QByteArray _data, int _status);


    void on_favoriteMarketBtn_clicked();

    void on_marketBtn1_clicked();

    void on_marketBtn3_clicked();

    void onPairSelected(const ExchangePair& _pair);
    void onAddFavoriteClicked();


    void on_periodComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::KLineWidget *ui;
    QCustomPlot* customPlot = nullptr;
    QCPFinancial* candlesticks = nullptr;

    void paintEvent(QPaintEvent*);
};

#endif // KLINEWIDGET_H
