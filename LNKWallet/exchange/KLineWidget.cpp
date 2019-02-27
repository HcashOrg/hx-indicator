#include "KLineWidget.h"
#include "ui_KLineWidget.h"

#include "wallet.h"

KLineWidget::KLineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KLineWidget)
{
    ui->setupUi(this);

    ui->recentDealsTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->recentDealsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->recentDealsTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->recentDealsTableWidget->setFrameShape(QFrame::NoFrame);
    ui->recentDealsTableWidget->setMouseTracking(true);
    ui->recentDealsTableWidget->setShowGrid(false);//隐藏表格线
    ui->recentDealsTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->recentDealsTableWidget->horizontalHeader()->setVisible(false);
    ui->recentDealsTableWidget->verticalHeader()->setVisible(false);
    ui->recentDealsTableWidget->setColumnWidth(0,50);
    ui->recentDealsTableWidget->setColumnWidth(1,50);
    ui->recentDealsTableWidget->setColumnWidth(2,40);

    customPlot = new QCustomPlot(this);
//    customPlot->setGeometry(QRect(60, 110, 480, 354));
    customPlot->setGeometry(QRect(10, 10, 480, 354));
    customPlot->legend->setVisible(true);
    customPlot->setInteractions( QCP::iRangeZoom);

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
    queryRecentDeals(20);
}

void KLineWidget::queryRecentDeals(int count)
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id","deal");
    object.insert("method","hx.fdxqs.exchange.deal.query");
    QJsonObject paramObject;
    paramObject.insert("pair", QString("%1/%2").arg(HXChain::getInstance()->currentExchangePair.first).arg(HXChain::getInstance()->currentExchangePair.second));
    paramObject.insert("count", count);
    object.insert("params",paramObject);
    httpManager.post(MIDDLE_EXCHANGE_URL,QJsonDocument(object).toJson());
}

void KLineWidget::queryKLineData(int type, int count)
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id","kline");
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

//    int num = 500;
//    QVector<double> time(num), value(num), value2(num);
//    QDateTime start = QDateTime(QDate(2014, 6, 11));
//    start.setTimeSpec(Qt::UTC);
//    double startTime = start.toTime_t();
//    double binSize = 3600*24; // bin data in 1 day intervals
//    time[0] = startTime;
//    value[0] = 60;
//    qsrand(9);
//    for (int i=1; i<num; ++i)
//    {
//      time[i] = startTime + 3600*i;
//      value[i] = value[i-1] + (qrand()/(double)RAND_MAX-0.5)*10;
//    }

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

    customPlot->replot();
}

void KLineWidget::showRecentDeals()
{
    ui->recentDealsTableWidget->setRowCount(0);
    ui->recentDealsTableWidget->setRowCount(deals.size());
    for(int i = 0; i < deals.size(); i++)
    {
        ExchangeDeal deal = deals.at(i);

        QString str = QDateTime::fromString(deal.dateTime, "yyyy-MM-dd hh:mm:ss").toString("hh:mm:ss");
        ui->recentDealsTableWidget->setItem( i, 0, new QTableWidgetItem(str));

        const AssetInfo& baseAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.first));
        const AssetInfo& quoteAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.second));
        unsigned long long baseAmount = deal.baseAmount;
        unsigned long long quoteAmount = deal.quoteAmount;
        double price = double(quoteAmount) / qPow(10,quoteAssetInfo.precision) / baseAmount * qPow(10,baseAssetInfo.precision);
        QString priceStr =  QString::number(price,'f', HXChain::getInstance()->getExchangePairPrecision(HXChain::getInstance()->currentExchangePair));
        ui->recentDealsTableWidget->setItem( i, 1, new QTableWidgetItem(priceStr));
        ui->recentDealsTableWidget->item(i,1)->setTextColor( (deal.type == "buy") ? QColor(1,215,26) : QColor(215,1,1));

        ui->recentDealsTableWidget->setItem( i, 2, new QTableWidgetItem( getBigNumberString( baseAmount, baseAssetInfo.precision)));

        for(int j : {0,1,2})
        {
            ui->recentDealsTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void KLineWidget::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    QString id = object.value("id").toString();
    qDebug() << "ooooooooooooo " << _data << _status;

    if(id == "deal")
    {
        QJsonObject resultObject = object.value("result").toObject();
        QJsonArray  array   = resultObject.value("data").toArray();

        deals.clear();
        for( QJsonValue v : array)
        {
            QJsonObject dealObject = v.toObject();
            ExchangeDeal deal;
            deal.address = dealObject.value("addr").toString();
            deal.baseAmount = jsonValueToULL( dealObject.value("base_amount"));
            deal.quoteAmount = jsonValueToULL( dealObject.value("quote_amount"));
            deal.pairStr = dealObject.value("pair").toString();
            deal.blockNum = dealObject.value("block_num").toInt();
            deal.type = dealObject.value("ex_type").toString();
            deal.dateTime = dealObject.value("timestamp").toString();

            deals << deal;
        }

        showRecentDeals();
    }
    else if(id == "kline")
    {
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

}

void KLineWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}
