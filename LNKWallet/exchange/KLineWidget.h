#ifndef KLINEWIDGET_H
#define KLINEWIDGET_H

#include <QWidget>

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

class KLineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KLineWidget(QWidget *parent = nullptr);
    ~KLineWidget();

    void init();

private:
    void queryKLineData(int type, int count);
    void drawKLine();
    HttpManager httpManager;
    QVector<KPointInfo>    kPoints;
private slots:
    void httpReplied(QByteArray _data, int _status);


private:
    Ui::KLineWidget *ui;
    QCustomPlot* customPlot = nullptr;
    QCPFinancial* candlesticks = nullptr;

    void paintEvent(QPaintEvent*);
};

#endif // KLINEWIDGET_H
