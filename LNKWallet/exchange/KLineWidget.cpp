#include "KLineWidget.h"
#include "ui_KLineWidget.h"

#include "ExchangePairSelectDialog.h"
#include "AddMyExchangePairsDialog.h"

static const QMap<int,int>  kLineTypeMap = { {0,60}, {1,300}, {2,900}, {3,1800}, {4,3600}, {5,7200}, {6,21600}, {7,43200}, {8,86400}, {9,604800}, {10,2592000}};
static const QMap<int,int>  kLineQueryCountMap = { {0, 3600 * 36 / 60}, {1,3600 * 24 * 7 / 300}, {2,3600 * 24 * 7 / 900}, {3,3600 * 24 * 7 / 1800}, {4,3600 * 24 * 30 / 3600},
                                                   {5,1000}, {6,1000}, {7,1000}, {8,1000}, {9,1000}, {10,1000}};
static const QMap<int,double>  kLineScaleFactorMap = { {0,0.1}, {1,0.1}, {2,0.2}, {3,0.2}, {4,0.4}, {5,0.4}, {6,0.4}, {7,0.4}, {8,0.4}, {9,0.4}, {10,1}};


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
    ui->recentDealsTableWidget->horizontalHeader()->setSectionsClickable(false);
    ui->recentDealsTableWidget->horizontalHeader()->setVisible(true);
    ui->recentDealsTableWidget->verticalHeader()->setVisible(false);
    ui->recentDealsTableWidget->setColumnWidth(0,55);
    ui->recentDealsTableWidget->setColumnWidth(1,60);
    ui->recentDealsTableWidget->setColumnWidth(2,60);

    customPlot = new QCustomPlot(ui->widget);
//    customPlot->setGeometry(QRect(60, 110, 480, 354));
    customPlot->setGeometry(QRect(0, 34, 480, 310));
    customPlot->legend->setVisible(false);
    customPlot->setInteractions( QCP::iRangeDrag | QCP::iRangeZoom);
    customPlot->axisRects().at(0)->setRangeDrag(Qt::Horizontal);
    customPlot->axisRects().at(0)->setRangeZoom(Qt::Horizontal);
    customPlot->axisRects().at(0)->setRangeZoomFactor(1.1,1);

    // create bottom axis rect for volume bar chart:
    volumeAxisRect = new QCPAxisRect(customPlot);
    customPlot->plotLayout()->addElement(1, 0, volumeAxisRect);
    volumeAxisRect->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
    volumeAxisRect->axis(QCPAxis::atBottom)->setLayer("axes");
    volumeAxisRect->axis(QCPAxis::atBottom)->grid()->setLayer("grid");
    // bring bottom and main axis rect closer together:
    customPlot->plotLayout()->setRowSpacing(0);
    volumeAxisRect->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
    volumeAxisRect->setMargins(QMargins(0, 0, 0, 0));
    volumeAxisRect->setRangeDrag(Qt::Horizontal);
    volumeAxisRect->setRangeZoom(Qt::Horizontal);

    QBrush backRole;
    backRole.setColor(QColor(243,241,250));
    backRole.setStyle(Qt::SolidPattern);
    customPlot->setBackground(backRole);

    connect(customPlot,&QCustomPlot::mouseMove,this, &KLineWidget::mouseMoveEvent);

    customPlot->installEventFilter(this);

    tipLabel = new QLabel(this);
    tipLabel->raise();

    init();
}

KLineWidget::~KLineWidget()
{
    delete ui;
}

void KLineWidget::init()
{
    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    ui->periodComboBox->setCurrentIndex(4);
    connect(ui->periodComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(onPeriodComboBoxCurrentIndexChanged(QString)));
    queryKLineData(ui->periodComboBox->currentIndex(),kLineQueryCountMap.value(ui->periodComboBox->currentIndex()));
    queryRecentDeals(20);

    ui->currentPairLabel->setText(QString("%1 / %2").arg(revertERCSymbol(HXChain::getInstance()->currentExchangePair.first))
                                  .arg(revertERCSymbol(HXChain::getInstance()->currentExchangePair.second)));
}

void KLineWidget::refresh()
{
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
    QList<uint>   keys = kPointMap.keys();
    int num = keys.size();
    if(num < 1)     return;
    QVector<double> time(num);
    QVector<double> open(num);
    QVector<double> high(num);
    QVector<double> low(num);
    QVector<double> close(num);
//    double binSize = 3600 * 12;
    double binSize = kLineTypeMap.value( ui->periodComboBox->currentIndex());

    const AssetInfo& baseAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.first));
    const AssetInfo& quoteAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.second));
    int precisionCompensation = qPow(10,baseAssetInfo.precision - quoteAssetInfo.precision);

    int offset = QTimeZone::systemTimeZone().standardTimeOffset(QDateTime::currentDateTime());
    for(int i = 0; i < num; i++)
    {
        KPointInfo kPoint = kPointMap.value( keys.at(i));
        QDateTime dateTime =  QDateTime::fromString( kPoint.dateTime, "yyyy-MM-dd hh:mm:ss");
        dateTime.setTimeSpec(Qt::UTC);
        time[i] = dateTime.toTime_t() + offset;
        open[i] = kPoint.kOpen * precisionCompensation;
        high[i] = kPoint.kHigh * precisionCompensation;
        low[i] = kPoint.kLow * precisionCompensation;
        close[i] = kPoint.kClose * precisionCompensation;
    }
    lower = time[0];
    upper = time[num - 1];

//    disconnect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onXRangeChanged(QCPRange)));




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
    candlesticks = new QCPFinancial(customPlot->xAxis, customPlot->yAxis);
    candlesticks->setName("Candlestick");
    candlesticks->setChartStyle(QCPFinancial::csCandlestick);
//    candlesticks->data()->set(QCPFinancial::timeSeriesToOhlc(time, value, binSize, time.at(0)));
    candlesticks->setData(time,open,high,low,close);
    candlesticks->setWidth(binSize*0.9);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(1, 215, 26));
    candlesticks->setBrushNegative(QColor(215, 1, 1));
    candlesticks->setPenPositive(QPen(QColor(1, 215, 26)));
    candlesticks->setPenNegative(QPen(QColor(215, 1, 1)));



    customPlot->axisRects().at(0)->axis(QCPAxis::atBottom)->setRange(time[0],time[num - 1]);
    volumeAxisRect->axis(QCPAxis::atBottom)->setRange(time[0],time[num - 1]);

    // create two bar plottables, for positive (green) and negative (red) volume bars:
    customPlot->setAutoAddPlottableToLegend(true);
    QCPBars *volumePos = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    QCPBars *volumeNeg = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    for(int i = 0; i < num; i++)
    {
        KPointInfo kPoint = kPointMap.value( keys.at(i));
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value( HXChain::getInstance()->getAssetId( HXChain::getInstance()->currentExchangePair.first));
        double v = getBigNumberString( kPoint.baseAmount, assetInfo.precision).toDouble();
        (kPoint.kClose < kPoint.kOpen ? volumeNeg : volumePos)->addData(time.at(i), v); // add data to either volumeNeg or volumePos, depending on sign of v
    }


    volumePos->setWidth(binSize*0.9);
    volumePos->setPen(Qt::NoPen);
    volumePos->setBrush(QColor(1, 215, 26));
    volumeNeg->setWidth(binSize*0.9);
    volumeNeg->setPen(Qt::NoPen);
    volumeNeg->setBrush(QColor(215, 1, 1));

    // configure axes of both main and bottom axis rect:
    QSharedPointer<MyQCPAxisTickerDateTime> dateTimeTicker(new MyQCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeSpec(Qt::UTC);
//    dateTimeTicker->setDateTimeFormat("dd.MMMM");
//    switch (ui->periodComboBox->currentIndex())
//    {
//    case 0:
//    case 1:
//    case 2:
//    }
    dateTimeTicker->setTickTimePointType(MyQCPAxisTickerDateTime::tpDay);
    dateTimeTicker->setDateTimeFormat("hh:mm");
    volumeAxisRect->axis(QCPAxis::atBottom)->setTicker(dateTimeTicker);
    volumeAxisRect->axis(QCPAxis::atBottom)->setTickLabelRotation(0);
    customPlot->xAxis->setBasePen(Qt::NoPen);
    customPlot->xAxis->setTickLabels(false);
    customPlot->xAxis->setTicks(false); // only want vertical grid in main axis rect, so hide xAxis backbone, ticks, and labels
//    customPlot->xAxis->setTicker(dateTimeTicker);

    customPlot->rescaleAxes();
    double factor = kLineScaleFactorMap.value(ui->periodComboBox->currentIndex()) / 10.0;
//    double factor = 0.5;
    customPlot->xAxis->scaleRange(factor, customPlot->xAxis->range().lower * factor * 0.5 + customPlot->xAxis->range().upper * (1 - factor * 0.5));
    qDebug() << "Xxxxxxxxxxxxx " << uint(customPlot->xAxis->range().lower)
             << uint(customPlot->xAxis->range().upper)
             ;

    double span = kLineTypeMap.value( ui->periodComboBox->currentIndex()) * 40;
//    double span = kLineTypeMap.value( ui->periodComboBox->currentIndex()) * kLineQueryCountMap.value( ui->periodComboBox->currentIndex())
//            * kLineScaleFactorMap.value( ui->periodComboBox->currentIndex());
    customPlot->xAxis->setRangeUpper(upper + span * 0.05);
    customPlot->xAxis->setRangeLower(upper - span * 1.05);

    if(customPlot->yAxis->range().lower < 0)
    {
        customPlot->yAxis->setRangeLower(0);
    }
    customPlot->yAxis->scaleRange(1.1, customPlot->yAxis->range().center());

    if(volumeAxisRect->axis(QCPAxis::atLeft)->range().lower < 0)
    {
        volumeAxisRect->axis(QCPAxis::atLeft)->setRangeLower(0);
    }

    // interconnect x axis ranges of main and bottom axis rects:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onXRangeChanged(QCPRange)));
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), volumeAxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis, SLOT(setRange(QCPRange)));

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
        ui->recentDealsTableWidget->setRowHeight(i, 22);

        ExchangeDeal deal = deals.at(i);

        QDateTime utcDateTime = QDateTime::fromString(deal.dateTime, "yyyy-MM-dd hh:mm:ss");
        utcDateTime.setTimeSpec(Qt::UTC);
        QString str = utcDateTime.toLocalTime().toString("hh:mm:ss");
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

        // 显示最新价格
        if(i == 0)
        {
            ui->priceLabel->setText(priceStr);
            ui->priceLabel->adjustSize();
            ui->priceLabel->setGeometry(ui->priceLabel->x(), ui->priceLabel->y(), ui->priceLabel->width(), 20);
            ui->infoLabel->move(ui->priceLabel->x() + ui->priceLabel->width() + 10, ui->infoLabel->y());
        }
    }


}

void KLineWidget::rescaleYAxis()
{
    //get visible min and max y values
    if (candlesticks)
    {
        QSharedPointer<QCPFinancialDataContainer> data = candlesticks->data();

        double dHigh = 0;
        double dLow = 99999999;

        QCPRange range = customPlot->xAxis->range();
        int count = 0;
        for (int i = 0; i < data->size(); ++i)
        {
            QCPFinancialDataContainer::const_iterator it = data->at(i);
            if(range.contains(it->key))
            {
                count++;
                if (it->high > dHigh)
                    dHigh = it->high;
                if (it->low < dLow && it->low > 0)
                    dLow = it->low;
            }
        }

        customPlot->yAxis->setRange(dLow * 0.5, dHigh * 2);
    }
}

KPointInfo KLineWidget::getKPointInfoByTime(uint time_t, uint interval)
{
    QVector<uint> keys = QVector<uint>::fromList( kPointMap.keys());
    uint left = 0;
    if(time_t % interval > interval / 2)
    {
        left = time_t - time_t % interval + interval;
    }
    else
    {
        left = time_t - time_t % interval;
    }

    if(keys.contains(left))
    {
        return kPointMap.value(left);
    }
    else
    {
        return KPointInfo();
    }

}

void KLineWidget::onXRangeChanged(const QCPRange &range)
{
//    if(lower < 1.5e+9 || upper < 1.5e+9)    return;
//    QCPRange boundedRange = range;

//    if(range.lower < lower)
//    {
//        boundedRange.lower = lower;
//    }

//    if(range.upper > upper)
//    {
//        boundedRange.upper = upper;
//    }
//    customPlot->xAxis->setRange(boundedRange);
    rescaleYAxis();
}

void KLineWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(customPlot->graphCount() < 2)    return;
    QVector<double> vx,vy;
    uint x = uint( customPlot->xAxis->pixelToCoord(event->pos().x()));
    double y = customPlot->yAxis->pixelToCoord(event->pos().y());

    int offset = QTimeZone::systemTimeZone().standardTimeOffset(QDateTime::currentDateTime());
    const KPointInfo kpInfo = getKPointInfoByTime(x - offset, uint(kLineTypeMap.value(ui->periodComboBox->currentIndex())));
    const AssetInfo& baseAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.first));
    const AssetInfo& quoteAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.second));
    double precisionCompensation = qPow(10,baseAssetInfo.precision - quoteAssetInfo.precision);
    int showPrecision = HXChain::getInstance()->getExchangePairPrecision(HXChain::getInstance()->currentExchangePair);
    double changeRate = (kpInfo.kOpen < 1e-9) ? 0 : (kpInfo.kClose - kpInfo.kOpen) / kpInfo.kOpen;
    QString str = tr("<html><head/><body><p><span style=\" font-size:10px; color:%7;\">CHANGE: %1%    </span><span style=\" font-size:10px; color:#261932;\">O:%2 H:%3 L:%4 C:%5 V:%6</span></p></body></html>").arg( QString::number(changeRate * 100, 'f', 2))
            .arg( QString::number(kpInfo.kOpen * precisionCompensation,'f',showPrecision)).arg(QString::number(kpInfo.kHigh * precisionCompensation,'f',showPrecision))
            .arg(QString::number(kpInfo.kLow * precisionCompensation,'f',showPrecision)).arg(QString::number(kpInfo.kClose * precisionCompensation,'f',showPrecision))
            .arg( getBigNumberString(kpInfo.baseAmount, baseAssetInfo.precision))
            .arg( (changeRate > 0) ? "rgb(1,215,26)" : "rgb(215,1,1)" );

    ui->infoLabel->setText(str);

    vx << 0 << x << customPlot->xAxis->range().maxRange;
    vy << y << y << y;
    customPlot->graph(0)->setData(vx,vy);
    customPlot->graph(0)->setPen(QPen(Qt::red));
    vx.clear();
    vy.clear();
    vx << x << x << x;
    vy << 0 << y << customPlot->yAxis->range().maxRange;
    customPlot->graph(1)->setData(vx,vy);
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->replot();

    tipLabel->move( customPlot->mapTo(this, event->pos() + QPoint(12,5)));
    QString timeStr = QDateTime::fromTime_t(x - offset).toString("hh:mm");
    tipLabel->setText(QString("(%1 , %2)").arg(timeStr).arg(y));
    tipLabel->adjustSize();
}

bool KLineWidget::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == customPlot)
    {
        if(e->type() == QEvent::Enter)
        {
            if(customPlot->graphCount() == 0)
            {
                customPlot->addGraph();
                customPlot->addGraph();

                tipLabel->show();
            }
        }
        else if(e->type() == QEvent::Leave)
        {
            if(customPlot->graphCount() >= 2)
            {
                customPlot->clearGraphs();
                customPlot->replot();

                tipLabel->hide();
            }
        }
    }

    return QWidget::eventFilter(watched,e);
}

void KLineWidget::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    QString id = object.value("id").toString();

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

        kPointMap.clear();
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

            QDateTime dateTime = QDateTime::fromString( kObject.value("timestamp").toString(), "yyyy-MM-dd hh:mm:ss");
            dateTime.setTimeSpec(Qt::UTC);
            uint time_t = dateTime.toTime_t();
            kPointMap.insert(time_t, info);
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

void KLineWidget::on_favoriteMarketBtn_clicked()
{
    ExchangePairSelectDialog dialog("");
    connect(&dialog, &ExchangePairSelectDialog::pairSelected, this, &KLineWidget::onPairSelected);
    connect(&dialog, &ExchangePairSelectDialog::addFavoriteClicked, this, &KLineWidget::onAddFavoriteClicked);
    dialog.move(ui->favoriteMarketBtn->mapToGlobal( QPoint(ui->favoriteMarketBtn->width() / 2 - dialog.width() / 2,ui->favoriteMarketBtn->height())));

    dialog.exec();
}

void KLineWidget::on_marketBtn1_clicked()
{
    ExchangePairSelectDialog dialog("HX");
    connect(&dialog, &ExchangePairSelectDialog::pairSelected, this, &KLineWidget::onPairSelected);
    dialog.move(ui->marketBtn1->mapToGlobal( QPoint(ui->marketBtn1->width() / 2 - dialog.width() / 2,ui->marketBtn1->height())));

    dialog.exec();
}

void KLineWidget::on_marketBtn3_clicked()
{
    ExchangePairSelectDialog dialog("ERCPAX");
    connect(&dialog, &ExchangePairSelectDialog::pairSelected, this, &KLineWidget::onPairSelected);
    dialog.move(ui->marketBtn3->mapToGlobal( QPoint(ui->marketBtn3->width() / 2 - dialog.width() / 2,ui->marketBtn3->height())));

    dialog.exec();
}

void KLineWidget::onPairSelected(const ExchangePair &_pair)
{
    HXChain::getInstance()->currentExchangePair = _pair;
    ui->currentPairLabel->setText( QString("%1 / %2").arg(revertERCSymbol(_pair.first)).arg(revertERCSymbol(_pair.second)));
    ui->priceLabel->clear();
    ui->infoLabel->clear();

    customPlot->clearPlottables();

    queryKLineData(ui->periodComboBox->currentIndex(), kLineQueryCountMap.value(ui->periodComboBox->currentIndex()));
    queryRecentDeals(20);
}

void KLineWidget::onAddFavoriteClicked()
{
    AddMyExchangePairsDialog dialog;
    dialog.pop();
}

void KLineWidget::onPeriodComboBoxCurrentIndexChanged(const QString &arg1)
{
    ui->infoLabel->clear();
    customPlot->clearPlottables();
    queryKLineData(ui->periodComboBox->currentIndex(), kLineQueryCountMap.value(ui->periodComboBox->currentIndex()));
}
