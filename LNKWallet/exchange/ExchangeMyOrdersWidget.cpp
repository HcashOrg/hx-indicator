#include "ExchangeMyOrdersWidget.h"
#include "ui_ExchangeMyOrdersWidget.h"

#include "ExchangePairSelectDialog.h"
#include "AddMyExchangePairsDialog.h"
#include "ToolButtonWidget.h"
#include "commondialog.h"
#include "control/BottomLine.h"

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

    ui->currentOrdersBtn->setCheckable(true);
    ui->currentOrdersBtn->setChecked(true);

    bottomLine = new BottomLine(this);
    bottomLine->attachToWidget(ui->currentOrdersBtn);

    init();
}

ExchangeMyOrdersWidget::~ExchangeMyOrdersWidget()
{
    delete ui;
}

void ExchangeMyOrdersWidget::init()
{
    onPairSelected(HXChain::getInstance()->currentExchangePair);
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

void ExchangeMyOrdersWidget::on_marketBtn2_clicked()
{
    ExchangePairSelectDialog dialog("BTC");
    connect(&dialog, &ExchangePairSelectDialog::pairSelected, this, &ExchangeMyOrdersWidget::onPairSelected);
    dialog.move(ui->marketBtn2->mapToGlobal( QPoint(ui->marketBtn2->width() / 2 - dialog.width() / 2,ui->marketBtn2->height())));

    dialog.exec();
}

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
                HXChain::getInstance()->postRPC( "ExchangeMyOrdersWidget+invoke_contract+cancelOrder",
                                                 toJsonFormat( "invoke_contract",
                                                 QJsonArray() << HXChain::getInstance()->currentAccount << "0.00001" << 100000 << EXCHANGE_MODE_CONTRACT_ADDRESS
                                                               << "cancelOrder"  << params));
            }
        }

        return;
    }
}

void ExchangeMyOrdersWidget::on_currentOrdersBtn_clicked()
{
    ui->currentOrdersBtn->setChecked(true);
}
