#ifndef KLINEWIDGET_H
#define KLINEWIDGET_H

#include <QWidget>

#include "wallet.h"
#include "extra/mycustomplot.h"
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

signals:
    void pairChanged(ExchangePair);
    void onClose();
public slots:
    void onPairChanged(const ExchangePair& _pair);


public:
    void refresh();
private:
    int refreshCount = 0;


private:
    void queryRecentDeals(int count);
    void queryKLineData(int type, int count);
    void drawKLine();
    void showRecentDeals();
    void rescaleYAxis();
    uint getTimeLeftValue(uint time_t, uint interval);   // 查找time_t属于哪个时间段
    void showLatestInfo();
    HttpManager httpManager;
    QMap<uint,KPointInfo>   kPointMap;
    QVector<ExchangeDeal>   deals;
    double lower = 0;
    double upper = 0;
private slots:
    void onXRangeChanged(const QCPRange &range);
private slots:
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *e);

private slots:
    void httpReplied(QByteArray _data, int _status);


    void on_favoriteMarketBtn_clicked();

    void on_marketBtn1_clicked();

    void on_marketBtn3_clicked();

    void onPairSelected(const ExchangePair& _pair);

    void onAddFavoriteClicked();

    void onPeriodComboBoxCurrentIndexChanged(const QString &arg1);

    void on_closeBtn_clicked();

private:
    Ui::KLineWidget *ui;
    QCustomPlot* customPlot = nullptr;
    QCPFinancial* candlesticks = nullptr;
    QCPAxisRect *volumeAxisRect = nullptr;
    QLabel* tipLabel = nullptr; // 显示鼠标所在坐标

    void paintEvent(QPaintEvent*);
};

#endif // KLINEWIDGET_H
