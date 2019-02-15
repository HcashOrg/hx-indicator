#include "ExchangeMyOrdersWidget.h"
#include "ui_ExchangeMyOrdersWidget.h"

#include <QScrollBar>

#include "ExchangePairSelectDialog.h"
#include "AddMyExchangePairsDialog.h"
#include "ToolButtonWidget.h"
#include "commondialog.h"
#include "control/BottomLine.h"
#include "ExchangeContractFeeDialog.h"
#include "poundage/PageScrollWidget.h"

ExchangeMyOrdersWidget::ExchangeMyOrdersWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangeMyOrdersWidget)
{
    ui->setupUi(this);
    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->myOrdersTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->myOrdersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->myOrdersTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->myOrdersTableWidget->setFrameShape(QFrame::NoFrame);
    ui->myOrdersTableWidget->setMouseTracking(true);
    ui->myOrdersTableWidget->setShowGrid(false);//隐藏表格线
    ui->myOrdersTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->myOrdersTableWidget->horizontalHeader()->setVisible(true);
    ui->myOrdersTableWidget->verticalHeader()->setVisible(false);
    ui->myOrdersTableWidget->setColumnWidth(0,140);
    ui->myOrdersTableWidget->setColumnWidth(1,80);
    ui->myOrdersTableWidget->setColumnWidth(2,170);
    ui->myOrdersTableWidget->setColumnWidth(3,170);
    ui->myOrdersTableWidget->setColumnWidth(4,80);

    ui->historyOrdersTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->historyOrdersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->historyOrdersTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->historyOrdersTableWidget->setFrameShape(QFrame::NoFrame);
    ui->historyOrdersTableWidget->setMouseTracking(true);
    ui->historyOrdersTableWidget->setShowGrid(false);//隐藏表格线
    ui->historyOrdersTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->historyOrdersTableWidget->horizontalHeader()->setVisible(true);
    ui->historyOrdersTableWidget->verticalHeader()->setVisible(false);
    ui->historyOrdersTableWidget->setColumnWidth(0,140);
    ui->historyOrdersTableWidget->setColumnWidth(1,80);
    ui->historyOrdersTableWidget->setColumnWidth(2,170);
    ui->historyOrdersTableWidget->setColumnWidth(3,170);
    ui->historyOrdersTableWidget->setColumnWidth(4,80);

    bottomLine = new BottomLine(this);
    on_currentOrdersBtn_clicked();

    pageWidget = new PageScrollWidget();
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&ExchangeMyOrdersWidget::pageChangeSlot);

    init();
}

ExchangeMyOrdersWidget::~ExchangeMyOrdersWidget()
{
    delete ui;
}

void ExchangeMyOrdersWidget::init()
{
    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));
    connect(ui->historyOrdersTableWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));

    onPairSelected(HXChain::getInstance()->currentExchangePair);

//    ui->historyBtn->setEnabled(false);


    queryOrders(1, 14);

}

void ExchangeMyOrdersWidget::jsonDataUpdated(QString id)
{
    if( id == "ExchangeMyOrdersWidget+invoke_contract_offline+getUserOrders+sell+" +
            HXChain::getInstance()->currentExchangePair.first + "+" + HXChain::getInstance()->currentExchangePair.second)
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QString resultStr = parse_doucment.object().value("result").toString();
            QJsonObject resultObject = QJsonDocument::fromJson(resultStr.toLatin1()).object();

            mySellOrdersMap.clear();
            QStringList keys = resultObject.keys();
            foreach (QString key, keys)
            {
                QString str = resultObject.value(key).toString();
                QStringList strList = str.split(",");
                OrderInfo orderInfo;
                orderInfo.baseAmount = strList.at(0).toULongLong();
                orderInfo.quoteAmount = strList.at(1).toULongLong();
                orderInfo.address = strList.at(2);
                orderInfo.trxId = strList.at(3);

                if(strList.size() == 8)
                {
                    orderInfo.completedBaseAmount = strList.at(4).toULongLong();
                    orderInfo.completedQuoteAmount = strList.at(5).toULongLong();
                    orderInfo.lastDealBaseAmount = strList.at(6).toULongLong();
                    orderInfo.lastDealQuoteAmount = strList.at(7).toULongLong();
                }
                mySellOrdersMap.insert(orderInfo.trxId, orderInfo);
            }
        }

        return;
    }

    if( id == "ExchangeMyOrdersWidget+invoke_contract_offline+getUserOrders+buy+" +
            HXChain::getInstance()->currentExchangePair.first + "+" + HXChain::getInstance()->currentExchangePair.second)
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QString resultStr = parse_doucment.object().value("result").toString();
            QJsonObject resultObject = QJsonDocument::fromJson(resultStr.toLatin1()).object();

            myBuyOrdersMap.clear();
            QStringList keys = resultObject.keys();
            foreach (QString key, keys)
            {
                QString str = resultObject.value(key).toString();
                QStringList strList = str.split(",");
                OrderInfo orderInfo;
                orderInfo.baseAmount = strList.at(0).toULongLong();
                orderInfo.quoteAmount = strList.at(1).toULongLong();
                orderInfo.address = strList.at(2);
                orderInfo.trxId = strList.at(3);

                if(strList.size() == 8)
                {
                    orderInfo.completedBaseAmount = strList.at(4).toULongLong();
                    orderInfo.completedQuoteAmount = strList.at(5).toULongLong();
                    orderInfo.lastDealBaseAmount = strList.at(6).toULongLong();
                    orderInfo.lastDealQuoteAmount = strList.at(7).toULongLong();
                }
                myBuyOrdersMap.insert(orderInfo.trxId, orderInfo);
            }

            showMyOrders();
        }

        return;
    }

    if( id.startsWith("ExchangeMyOrdersWidget+invoke_contract_testing+cancelOrder+"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;


        HXChain::TotalContractFee totalFee = HXChain::getInstance()->parseTotalContractFee(result);
        unsigned long long totalAmount = totalFee.baseAmount + ceil(totalFee.step * 1.2 * HXChain::getInstance()->contractFee / 100.0);

        ExchangeContractFeeDialog exchangeContractFeeDialog(totalAmount, HXChain::getInstance()->currentAccount);
        if(exchangeContractFeeDialog.pop())
        {
            QString params = id.mid( QString("ExchangeMyOrdersWidget+invoke_contract_testing+cancelOrder+").size());
            exchangeContractFeeDialog.updatePoundageID();
            HXChain::getInstance()->postRPC( "ExchangeMyOrdersWidget+invoke_contract+cancelOrder",
                                             toJsonFormat( "invoke_contract",
                                                           QJsonArray() << HXChain::getInstance()->currentAccount << HXChain::getInstance()->currentContractFee()
                                                           << ceil(totalFee.step * 1.2) << EXCHANGE_MODE_CONTRACT_ADDRESS
                                                           << "cancelOrder"  << params));
        }

        return;
    }

    if( id == "ExchangeMyOrdersWidget+invoke_contract+cancelOrder")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        getUserOrders(HXChain::getInstance()->currentExchangePair);

        return;
    }
}

void ExchangeMyOrdersWidget::on_favoriteMarketBtn_clicked()
{
    ExchangePairSelectDialog dialog("");
    connect(&dialog, &ExchangePairSelectDialog::pairSelected, this, &ExchangeMyOrdersWidget::onPairSelected);
    connect(&dialog, &ExchangePairSelectDialog::addFavoriteClicked, this, &ExchangeMyOrdersWidget::onAddFavoriteClicked);
    dialog.move(ui->favoriteMarketBtn->mapToGlobal( QPoint(ui->favoriteMarketBtn->width() / 2 - dialog.width() / 2,ui->favoriteMarketBtn->height())));

    dialog.exec();
}

void ExchangeMyOrdersWidget::on_marketBtn1_clicked()
{
    ExchangePairSelectDialog dialog("HX");
    connect(&dialog, &ExchangePairSelectDialog::pairSelected, this, &ExchangeMyOrdersWidget::onPairSelected);
    dialog.move(ui->marketBtn1->mapToGlobal( QPoint(ui->marketBtn1->width() / 2 - dialog.width() / 2,ui->marketBtn1->height())));

    dialog.exec();
}

//void ExchangeMyOrdersWidget::on_marketBtn2_clicked()
//{
//    ExchangePairSelectDialog dialog("BTC");
//    connect(&dialog, &ExchangePairSelectDialog::pairSelected, this, &ExchangeMyOrdersWidget::onPairSelected);
//    dialog.move(ui->marketBtn2->mapToGlobal( QPoint(ui->marketBtn2->width() / 2 - dialog.width() / 2,ui->marketBtn2->height())));

//    dialog.exec();
//}

void ExchangeMyOrdersWidget::on_marketBtn3_clicked()
{
    ExchangePairSelectDialog dialog("ERCPAX");
    connect(&dialog, &ExchangePairSelectDialog::pairSelected, this, &ExchangeMyOrdersWidget::onPairSelected);
    dialog.move(ui->marketBtn3->mapToGlobal( QPoint(ui->marketBtn3->width() / 2 - dialog.width() / 2,ui->marketBtn3->height())));

    dialog.exec();
}

void ExchangeMyOrdersWidget::getUserOrders(const ExchangePair &_pair)
{
    AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value( HXChain::getInstance()->currentAccount);

    QString params = QString("%1,%2,%3,%4").arg(_pair.first).arg(_pair.second).arg(accountInfo.address).arg( "sell");
    HXChain::getInstance()->postRPC( "ExchangeMyOrdersWidget+invoke_contract_offline+getUserOrders+sell+" + _pair.first + "+" + _pair.second,
                                     toJsonFormat( "invoke_contract_offline",
                                     QJsonArray() << HXChain::getInstance()->currentAccount << EXCHANGE_MODE_CONTRACT_ADDRESS
                                                   << "getUserOrders"  << params));

    params = QString("%1,%2,%3,%4").arg(_pair.first).arg(_pair.second).arg(accountInfo.address).arg( "buy");
    HXChain::getInstance()->postRPC( "ExchangeMyOrdersWidget+invoke_contract_offline+getUserOrders+buy+" + _pair.first + "+" + _pair.second,
                                     toJsonFormat( "invoke_contract_offline",
                                     QJsonArray() << HXChain::getInstance()->currentAccount << EXCHANGE_MODE_CONTRACT_ADDRESS
                                                   << "getUserOrders"  << params));
}

void ExchangeMyOrdersWidget::showMyOrders()
{
    const AssetInfo& baseAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.first));
    const AssetInfo& quoteAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.second));

    int sellCount = mySellOrdersMap.size();
    int buyCount = myBuyOrdersMap.size();

    ui->myOrdersTableWidget->setRowCount(0);
    ui->myOrdersTableWidget->setRowCount(sellCount + buyCount);

    for(int i = 0; i < sellCount; i++)
    {
        ui->myOrdersTableWidget->setRowHeight(i,40);

        const OrderInfo& orderInfo = mySellOrdersMap.value( mySellOrdersMap.keys().at(i));

        TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct( orderInfo.trxId);
        QString trxTime;
        if(ts.expirationTime.isEmpty())
        {
            trxTime = "--";
        }
        else
        {
            QDateTime dateTime = QDateTime::fromString(ts.expirationTime,"yyyy-MM-ddThh:mm:ss");
            dateTime = dateTime.addSecs( -600);
            dateTime.setTimeSpec(Qt::UTC);
            dateTime = dateTime.toLocalTime();
            trxTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
        }
        ui->myOrdersTableWidget->setItem( i, 0, new QTableWidgetItem( trxTime));

        ui->myOrdersTableWidget->setItem( i, 1, new QTableWidgetItem( tr("sell")));
        ui->myOrdersTableWidget->item(i,1)->setTextColor(QColor(44,202,148));
        ui->myOrdersTableWidget->item(i,1)->setData(Qt::UserRole, orderInfo.trxId);

        unsigned long long totalBaseAmount = orderInfo.baseAmount + orderInfo.completedBaseAmount;
        ui->myOrdersTableWidget->setItem( i, 2, new QTableWidgetItem( getBigNumberString( totalBaseAmount, baseAssetInfo.precision)
                                                                      + " / "
                                                                      + getBigNumberString( orderInfo.completedBaseAmount, baseAssetInfo.precision)));

        double price = double(orderInfo.quoteAmount) / qPow(10,quoteAssetInfo.precision) / orderInfo.baseAmount * qPow(10,baseAssetInfo.precision);
        double completedPrice = 0;
        if(orderInfo.completedBaseAmount != 0)
        {
            completedPrice = double(orderInfo.completedQuoteAmount) / qPow(10,quoteAssetInfo.precision) / orderInfo.completedBaseAmount * qPow(10,baseAssetInfo.precision);
        }
        QTableWidgetItem* item = new QTableWidgetItem( QString::number(price,'f', HXChain::getInstance()->getExchangePairPrecision(HXChain::getInstance()->currentExchangePair))
                                                       + " / "
                                                       + ((orderInfo.completedBaseAmount == 0) ? "-" : QString::number(completedPrice,'f', HXChain::getInstance()->getExchangePairPrecision(HXChain::getInstance()->currentExchangePair)))
                                                       );
        item->setTextColor(QColor(83,61,138));
        ui->myOrdersTableWidget->setItem( i,3,item);

        ui->myOrdersTableWidget->setItem( i, 4, new QTableWidgetItem( tr("cancel")));

        ToolButtonWidget *toolButton = new ToolButtonWidget();
        toolButton->setText(ui->myOrdersTableWidget->item(i,4)->text());
        ui->myOrdersTableWidget->setCellWidget(i,4,toolButton);
        connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ExchangeMyOrdersWidget::on_myOrdersTableWidget_cellClicked,this,i,4));
    }

    for(int i = sellCount; i < sellCount + buyCount; i++)
    {
        ui->myOrdersTableWidget->setRowHeight(i,40);

        const OrderInfo& orderInfo = myBuyOrdersMap.value( myBuyOrdersMap.keys().at(i - sellCount));

        TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct( orderInfo.trxId);
        QString trxTime;
        if(ts.expirationTime.isEmpty())
        {
            trxTime = "--";
        }
        else
        {
            QDateTime dateTime = QDateTime::fromString(ts.expirationTime,"yyyy-MM-ddThh:mm:ss");
            dateTime = dateTime.addSecs( -600);
            dateTime.setTimeSpec(Qt::UTC);
            dateTime = dateTime.toLocalTime();
            trxTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
        }
        ui->myOrdersTableWidget->setItem( i, 0, new QTableWidgetItem( trxTime));

        ui->myOrdersTableWidget->setItem( i, 1, new QTableWidgetItem( tr("buy")));
        ui->myOrdersTableWidget->item(i,1)->setTextColor(QColor(235,0,94));
        ui->myOrdersTableWidget->item(i,1)->setData(Qt::UserRole, orderInfo.trxId);

        unsigned long long totalBaseAmount = orderInfo.baseAmount + orderInfo.completedBaseAmount;
        ui->myOrdersTableWidget->setItem( i, 2, new QTableWidgetItem( getBigNumberString( totalBaseAmount, baseAssetInfo.precision)
                                                                      + " / "
                                                                      + getBigNumberString( orderInfo.completedBaseAmount, baseAssetInfo.precision)));

        double price = double(orderInfo.quoteAmount) / qPow(10,quoteAssetInfo.precision) / orderInfo.baseAmount * qPow(10,baseAssetInfo.precision);
        double completedPrice = 0;
        if(orderInfo.completedBaseAmount != 0)
        {
            completedPrice = double(orderInfo.completedQuoteAmount) / qPow(10,quoteAssetInfo.precision) / orderInfo.completedBaseAmount * qPow(10,baseAssetInfo.precision);
        }
        QTableWidgetItem* item = new QTableWidgetItem( QString::number(price,'f', HXChain::getInstance()->getExchangePairPrecision(HXChain::getInstance()->currentExchangePair))
                                                       + " / "
                                                       + ((orderInfo.completedBaseAmount == 0) ? "-" : QString::number(completedPrice,'f', HXChain::getInstance()->getExchangePairPrecision(HXChain::getInstance()->currentExchangePair)))
                                                       );
        item->setTextColor(QColor(83,61,138));
        ui->myOrdersTableWidget->setItem( i,3,item);

        ui->myOrdersTableWidget->setItem( i, 4, new QTableWidgetItem( tr("cancel")));

        ToolButtonWidget *toolButton = new ToolButtonWidget();
        toolButton->setText(ui->myOrdersTableWidget->item(i,4)->text());
        ui->myOrdersTableWidget->setCellWidget(i,4,toolButton);
        connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ExchangeMyOrdersWidget::on_myOrdersTableWidget_cellClicked,this,i,4));
    }

    tableWidgetSetItemZebraColor(ui->myOrdersTableWidget);

}

void ExchangeMyOrdersWidget::onPairSelected(const ExchangePair &_pair)
{
    HXChain::getInstance()->currentExchangePair = _pair;
    ui->currentPairLabel->setText( QString("%1 / %2").arg(revertERCSymbol(_pair.first)).arg(revertERCSymbol(_pair.second)));

    getUserOrders(_pair);
}

void ExchangeMyOrdersWidget::onAddFavoriteClicked()
{
    AddMyExchangePairsDialog dialog;
    dialog.pop();
}

void ExchangeMyOrdersWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void ExchangeMyOrdersWidget::on_myOrdersTableWidget_cellClicked(int row, int column)
{
    if(column == 4)
    {
        if( ui->myOrdersTableWidget->item(row,1))
        {
            QString trxId = ui->myOrdersTableWidget->item(row,1)->data(Qt::UserRole).toString();
            OrderInfo orderInfo;
            QString type;
            if(mySellOrdersMap.contains(trxId))
            {
                orderInfo = mySellOrdersMap.value(trxId);
                type = "sell";
            }
            else if(myBuyOrdersMap.contains(trxId))
            {
                orderInfo = myBuyOrdersMap.value(trxId);
                type = "buy";
            }
            else
            {
                return;
            }

            CommonDialog commonDialog(CommonDialog::OkAndCancel);
            commonDialog.setText(tr("Sure to cancel this order?"));
            if(commonDialog.pop())
            {
                QString params = QString("%1,%2,%3,%4").arg(HXChain::getInstance()->currentExchangePair.first)
                        .arg(HXChain::getInstance()->currentExchangePair.second)
                        .arg(orderInfo.trxId)
                        .arg(type);
//                HXChain::getInstance()->postRPC( "ExchangeMyOrdersWidget+invoke_contract+cancelOrder",
//                                                 toJsonFormat( "invoke_contract",
//                                                 QJsonArray() << HXChain::getInstance()->currentAccount << "0.00001" << 100000 << EXCHANGE_MODE_CONTRACT_ADDRESS
//                                                               << "cancelOrder"  << params));
                HXChain::getInstance()->postRPC( "ExchangeMyOrdersWidget+invoke_contract_testing+cancelOrder+" + params,
                                                 toJsonFormat( "invoke_contract_testing",
                                                               QJsonArray() << HXChain::getInstance()->currentAccount << EXCHANGE_MODE_CONTRACT_ADDRESS
                                                               << "cancelOrder"  << params));
            }
        }

        return;
    }
}

void ExchangeMyOrdersWidget::on_currentOrdersBtn_clicked()
{
    ui->currentOrdersBtn->setChecked(true);
    ui->historyBtn->setChecked(false);

    bottomLine->attachToWidget(ui->currentOrdersBtn);

    ui->myOrdersTableWidget->show();
    ui->historyOrdersTableWidget->hide();
}

void ExchangeMyOrdersWidget::on_historyBtn_clicked()
{
    ui->currentOrdersBtn->setChecked(false);
    ui->historyBtn->setChecked(true);

    bottomLine->attachToWidget(ui->historyBtn);

    ui->myOrdersTableWidget->hide();
    ui->historyOrdersTableWidget->show();
}

void ExchangeMyOrdersWidget::queryOrders(int page, int pageCount)
{
    AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value( HXChain::getInstance()->currentAccount);
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","hx.fdxqs.exchange.deal.query");
    QJsonObject paramObject;
    paramObject.insert("addr", accountInfo.address);
    paramObject.insert("pair", QString("%1/%2").arg(HXChain::getInstance()->currentExchangePair.first).arg(HXChain::getInstance()->currentExchangePair.second));
    paramObject.insert("page", page);
    paramObject.insert("page_count", pageCount);
    object.insert("params",paramObject);
    httpManager.post(MIDDLE_EXCHANGE_URL,QJsonDocument(object).toJson());
}

void ExchangeMyOrdersWidget::showHistoryOrders()
{
    const AssetInfo& baseAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.first));
    const AssetInfo& quoteAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.second));

    QStringList keys = historyOrdersInfoMap.keys();
    int size = keys.size();
    ui->historyOrdersTableWidget->setRowCount(0);
    ui->historyOrdersTableWidget->setRowCount(size);
    for(int i = 0; i < size; i++)
    {
        ui->historyOrdersTableWidget->setRowHeight(i,40);

        const HistoryOrderInfo& info = historyOrdersInfoMap.value( keys.at(i));

        QDateTime dateTime = info.dateTime;
        dateTime = dateTime.addSecs( -600);
        dateTime.setTimeSpec(Qt::UTC);
        dateTime = dateTime.toLocalTime();
        ui->historyOrdersTableWidget->setItem( i, 0, new QTableWidgetItem( dateTime.toString("yyyy-MM-dd hh:mm:ss")));

        QString typeStr;
        QColor color;
        if(info.type == "buy")  { typeStr = tr("buy"); color = QColor(235,0,94);}
        if(info.type == "sell")  { typeStr = tr("sell"); color = QColor(44,202,148);}
        ui->historyOrdersTableWidget->setItem( i, 1, new QTableWidgetItem( typeStr));
        ui->historyOrdersTableWidget->item(i,1)->setTextColor(color);
        ui->historyOrdersTableWidget->item(i,1)->setData(Qt::UserRole, info.trxId);

        unsigned long long completedBaseAmount = info.originBaseAmount - info.currentBaseAmount;    // currentBaseAmount 可能是负数  已成交肯定是正数
        unsigned long long completedQuoteAmount = info.originQuoteAmount - info.currentQuoteAmount;    // currentBaseAmount 可能是负数  已成交肯定是正数
        ui->historyOrdersTableWidget->setItem( i, 2, new QTableWidgetItem( getBigNumberString( info.originBaseAmount, baseAssetInfo.precision)
                                                                      + " / "
                                                                      + getBigNumberString( completedBaseAmount, baseAssetInfo.precision)));

        double price = double(info.originQuoteAmount) / qPow(10,quoteAssetInfo.precision) / info.originBaseAmount * qPow(10,baseAssetInfo.precision);
        double completedPrice = 0;
        if(completedBaseAmount != 0)
        {
            completedPrice = double(completedQuoteAmount) / qPow(10,quoteAssetInfo.precision) / completedBaseAmount * qPow(10,baseAssetInfo.precision);
        }
        QTableWidgetItem* item = new QTableWidgetItem( QString::number(price,'f', HXChain::getInstance()->getExchangePairPrecision(HXChain::getInstance()->currentExchangePair))
                                                       + " / "
                                                       + ((completedBaseAmount == 0) ? "-" : QString::number(completedPrice,'f', HXChain::getInstance()->getExchangePairPrecision(HXChain::getInstance()->currentExchangePair)))
                                                       );
        item->setTextColor(QColor(83,61,138));
        ui->historyOrdersTableWidget->setItem( i,3,item);

        QString stateStr;
        switch (info.state)
        {
        case 1:
            stateStr = tr("no deal");
            break;
        case 2:
            stateStr = tr("partial deal");
            break;
        case 3:
            stateStr = tr("total deal");
            break;
        case 4:
            stateStr = tr("withdrawed");
            break;
        }
        ui->historyOrdersTableWidget->setItem( i, 4, new QTableWidgetItem( stateStr));
    }

    tableWidgetSetItemZebraColor(ui->historyOrdersTableWidget);

    ui->historyOrdersTableWidget->horizontalScrollBar()->setSliderPosition(currentSliderPos);
}

void ExchangeMyOrdersWidget::pageChangeSlot(unsigned int page)
{

}

void ExchangeMyOrdersWidget::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();

    QJsonObject resultObject = object.value("result").toObject();
    recordCount = resultObject.value("total_records").toInt();
    currentPage = resultObject.value("current_page").toInt();

    QJsonArray  array   = resultObject.value("data").toArray();
    for( QJsonValue v : array)
    {
        QJsonObject orderObject = v.toObject();
        HistoryOrderInfo info;
        info.trxId = orderObject.value("tx_id").toString();
        info.currentBaseAmount = jsonValueToLL(  orderObject.value("current_base_amount"));
        info.currentQuoteAmount = jsonValueToLL( orderObject.value("current_quote_amount"));
        info.originBaseAmount = jsonValueToULL( orderObject.value("origin_base_amount"));
        info.originQuoteAmount = jsonValueToULL( orderObject.value("origin_quote_amount"));
        info.state = orderObject.value("stat").toInt();
        info.type = orderObject.value("ex_type").toString();
        info.dateTime = QDateTime::fromString(orderObject.value("timestamp").toString(),"yyyy-MM-dd hh:mm:ss");
        QStringList pairStrList = orderObject.value("ex_pair").toString().split("/");
        if(pairStrList.size() > 1)
        {
            info.pair.first = pairStrList.at(0);
            info.pair.second = pairStrList.at(1);
        }

        historyOrdersInfoMap.insert(info.trxId, info);
    }

    showHistoryOrders();

//    int size = array.size();
//    ui->historyOrdersTableWidget->setRowCount(size);
//    for(int i = 0; i < size; i++)
//    {
//        ui->historyOrdersTableWidget->setRowHeight(i,40);

//        QJsonObject dataObject = array.at(i).toObject();
//        QString contractAddress = dataObject.take("contract_address").toString();
//        unsigned long long sellAmount = jsonValueToULL(dataObject.take("from_supply"));
//        QString sellSymbol = dataObject.take("from_asset").toString();
//        unsigned long long buyAmount = jsonValueToULL(dataObject.take("to_supply"));
//        QString buySymbol = dataObject.take("to_asset").toString();
//        int state = dataObject.take("state").toInt();

//        AssetInfo sellAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(sellSymbol));
//        AssetInfo buyAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(buySymbol));

//        ui->ordersTableWidget->setItem(i,0, new QTableWidgetItem(getBigNumberString(sellAmount,sellAssetInfo.precision)));
//        ui->ordersTableWidget->item(i,0)->setData(Qt::UserRole,sellAmount);

//        ui->ordersTableWidget->setItem(i,1, new QTableWidgetItem(getBigNumberString(buyAmount,buyAssetInfo.precision)));
//        ui->ordersTableWidget->item(i,1)->setData(Qt::UserRole,buyAmount);

//        double price = (double)sellAmount / qPow(10,sellAssetInfo.precision) / buyAmount * qPow(10,buyAssetInfo.precision);
//        QTableWidgetItem* item = new QTableWidgetItem(QString::number(price,'g',8));
//        item->setData(Qt::UserRole,contractAddress);
//        ui->ordersTableWidget->setItem(i,2,item);

//        ui->ordersTableWidget->setItem(i,3, new QTableWidgetItem(tr("buy")));

//        for(int j = 3;j < 4;++j)
//        {
//            ToolButtonWidgetItem *toolButtonItem = new ToolButtonWidgetItem(i,j);

//            if(HXChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText()) == contractAddress)
//            {
//                toolButtonItem->setEnabled(false);
//                toolButtonItem->setText(tr("my order"));
//                toolButtonItem->setButtonFixSize(80,20);
//            }
//            else
//            {
//                toolButtonItem->setText(ui->ordersTableWidget->item(i,j)->text());
//            }

//            ui->ordersTableWidget->setCellWidget(i,j,toolButtonItem);
//            connect(toolButtonItem,SIGNAL(itemClicked(int,int)),this,SLOT(onItemClicked(int,int)));
//        }
//    }
//    tableWidgetSetItemZebraColor(ui->ordersTableWidget);

//    int page = (ui->ordersTableWidget->rowCount()%ROWNUMBER==0 && ui->ordersTableWidget->rowCount() != 0) ?
//                ui->ordersTableWidget->rowCount()/ROWNUMBER : ui->ordersTableWidget->rowCount()/ROWNUMBER+1;
//    pageWidget->SetTotalPage(page);
//    pageWidget->setShowTip(ui->ordersTableWidget->rowCount(),ROWNUMBER);
//    pageChangeSlot(0);
//    pageWidget->setVisible(0 != size);

    //    blankWidget->setVisible(0 == size);
}

void ExchangeMyOrdersWidget::onSliderValueChanged(int _value)
{
qDebug() << "ssssssssssssss " << _value;
    if( ui->historyOrdersTableWidget->rowCount() >= recordCount)    return;
    if(ui->historyOrdersTableWidget->horizontalScrollBar()->sliderPosition() + 7 >= ui->historyOrdersTableWidget->rowCount())
    {
        currentSliderPos = ui->historyOrdersTableWidget->horizontalScrollBar()->sliderPosition();
        queryOrders(currentPage++, 14);
    }
}
