#include "KLineWidget.h"
#include "ui_KLineWidget.h"

#include "wallet.h"

KLineWidget::KLineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KLineWidget)
{
    ui->setupUi(this);

    customPlot = new QCustomPlot(this);
    customPlot->setGeometry(QRect(60, 110, 480, 354));

    init();
}

KLineWidget::~KLineWidget()
{
    delete ui;
}

void KLineWidget::init()
{
    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    queryKLineData(4,100);
}

void KLineWidget::queryKLineData(int type, int count)
{
    AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value( HXChain::getInstance()->currentAccount);
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id","886");
    object.insert("method","hx.fdxqs.exchange.kline.query");
    QJsonObject paramObject;
    paramObject.insert("pair", QString("%1/%2").arg(HXChain::getInstance()->currentExchangePair.first).arg(HXChain::getInstance()->currentExchangePair.second));
    paramObject.insert("type", type);
    paramObject.insert("count", count);
    object.insert("params",paramObject);
    httpManager.post(MIDDLE_EXCHANGE_URL,QJsonDocument(object).toJson());

}

void KLineWidget::drawKLine()
{
    int num = kPoints.size();
    if(num < 1)     return;
    QVector<double> time(num);
    QVector<double> value(num);
    double binSize = 3600; // bin data in 1 day intervals

    for(int i = 0; i < num; i++)
    {
        time[i] = QDateTime::fromString( kPoints.at(i).dateTime, "yyyy-MM-dd hh:mm:ss").toTime_t();
        value[i] = kPoints.at(i).kOpen;
    }
    qDebug() << time;
    qDebug() << value;
    // create candlestick chart:
    QCPFinancial *candlesticks = new QCPFinancial(customPlot->xAxis, customPlot->yAxis);
    candlesticks->setName("Candlestick");
    candlesticks->setChartStyle(QCPFinancial::csCandlestick);
    candlesticks->data()->set(QCPFinancial::timeSeriesToOhlc(time, value, binSize, time.at(0)));
    candlesticks->setWidth(binSize*0.9);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(183, 8, 8));
    candlesticks->setBrushNegative(QColor(38, 118, 24));
    candlesticks->setPenPositive(QPen(QColor(183, 8, 8)));
    candlesticks->setPenNegative(QPen(QColor(38, 118, 24)));

    // create bottom axis rect for volume bar chart:
    QCPAxisRect *volumeAxisRect = new QCPAxisRect(customPlot);
    customPlot->plotLayout()->addElement(1, 0, volumeAxisRect);
    volumeAxisRect->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
    volumeAxisRect->axis(QCPAxis::atBottom)->setLayer("axes");
    volumeAxisRect->axis(QCPAxis::atBottom)->grid()->setLayer("grid");
    // bring bottom and main axis rect closer together:
    customPlot->plotLayout()->setRowSpacing(0);
    volumeAxisRect->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
    volumeAxisRect->setMargins(QMargins(0, 0, 0, 0));
    // create two bar plottables, for positive (green) and negative (red) volume bars:
    customPlot->setAutoAddPlottableToLegend(false);
    QCPBars *volumePos = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    QCPBars *volumeNeg = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    for(int i = 0; i < num; i++)
    {
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->currentExchangePair.first);
        double v = getBigNumberString( kPoints.at(i).baseAmount, assetInfo.precision).toDouble();
        (kPoints.at(i).kClose < kPoints.at(i).kOpen ? volumeNeg : volumePos)->addData(time.at(i), v); // add data to either volumeNeg or volumePos, depending on sign of v
    }


    volumePos->setWidth(3600*4);
    volumePos->setPen(Qt::NoPen);
    volumePos->setBrush(QColor(100, 180, 110));
    volumeNeg->setWidth(3600*4);
    volumeNeg->setPen(Qt::NoPen);
    volumeNeg->setBrush(QColor(180, 90, 90));

    // interconnect x axis ranges of main and bottom axis rects:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), volumeAxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis, SLOT(setRange(QCPRange)));
    // configure axes of both main and bottom axis rect:
    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeSpec(Qt::UTC);
    dateTimeTicker->setDateTimeFormat("dd. MMMM");
    volumeAxisRect->axis(QCPAxis::atBottom)->setTicker(dateTimeTicker);
    volumeAxisRect->axis(QCPAxis::atBottom)->setTickLabelRotation(0);
    customPlot->xAxis->setBasePen(Qt::NoPen);
    customPlot->xAxis->setTickLabels(false);
    customPlot->xAxis->setTicks(false); // only want vertical grid in main axis rect, so hide xAxis backbone, ticks, and labels
    customPlot->xAxis->setTicker(dateTimeTicker);
    customPlot->rescaleAxes();
    customPlot->xAxis->scaleRange(1.025, customPlot->xAxis->range().center());
    customPlot->yAxis->scaleRange(1.1, customPlot->yAxis->range().center());

    // 设置两个坐标轴矩形左右对齐
    QCPMarginGroup *group = new QCPMarginGroup(customPlot);
    customPlot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    volumeAxisRect->setMarginGroup(QCP::msLeft|QCP::msRight, group);
}

void KLineWidget::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    QJsonObject resultObject = object.value("result").toObject();
    QJsonArray  array   = resultObject.value("data").toArray();

    kPoints.clear();
    for( QJsonValue v : array)
    {
        QJsonObject kObject = v.toObject();
        KPointInfo info;
        info.baseAmount = jsonValueToULL( kObject.value("base_amount"));
        info.quoteAmount = jsonValueToULL( kObject.value("quote_amount"));
        info.blockNum = kObject.value("block_num").toInt();
        info.pairStr = kObject.value("ex_pair").toString();
        info.kOpen = kObject.value("k_open").toDouble();
        info.kClose = kObject.value("k_close").toDouble();
        info.kHigh = kObject.value("k_high").toDouble();
        info.kLow = kObject.value("k_low").toDouble();
        info.dateTime = kObject.value("timestamp").toString();

        kPoints << info;
    }

    drawKLine();
}

void KLineWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}
