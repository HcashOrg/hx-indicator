#include "marketpage.h"
#include "ui_marketpage.h"

#include "hsr.h"
#include "rpcthread.h"
#include "commondialog.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#define MARKETPAGE_BTN_SELECTED_STYLE     "QToolButton{background-color:rgb(0,138,254);color:white;border:none;}"
#define MARKETPAGE_BTN_UNSELECTED_STYLE   "QToolButton{background-color:rgb(30,168,254);color:white;border:none;}"


MarketPage::MarketPage(QString name, QWidget *parent) :
    QWidget(parent),
    inited(false),
    editing(false),
    ui(new Ui::MarketPage)
{
    ui->setupUi(this);

    connect( Hcash::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->bidOrderTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->bidOrderTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->bidOrderTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->bidOrderTableWidget->setColumnWidth(0,130);
    ui->bidOrderTableWidget->setColumnWidth(1,120);
    ui->bidOrderTableWidget->setColumnWidth(2,120);
    ui->bidOrderTableWidget->setShowGrid(false);
    ui->bidOrderTableWidget->setFrameShape(QFrame::NoFrame);
    ui->bidOrderTableWidget->setMouseTracking(true);

    ui->askOrderTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->askOrderTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->askOrderTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->askOrderTableWidget->setColumnWidth(0,130);
    ui->askOrderTableWidget->setColumnWidth(1,120);
    ui->askOrderTableWidget->setColumnWidth(2,120);
    ui->askOrderTableWidget->setShowGrid(false);
    ui->askOrderTableWidget->setFrameShape(QFrame::NoFrame);
    ui->askOrderTableWidget->setMouseTracking(true);

    ui->myOrdersTableWidget->setColumnWidth(0,120);
    ui->myOrdersTableWidget->setColumnWidth(1,130);
    ui->myOrdersTableWidget->setColumnWidth(2,60);
    ui->myOrdersTableWidget->setColumnWidth(3,130);
    ui->myOrdersTableWidget->setColumnWidth(4,100);
    ui->myOrdersTableWidget->setColumnWidth(5,100);
    ui->myOrdersTableWidget->setColumnWidth(6,80);

    ui->recentOrdersTableWidget->setColumnWidth(0,150);
    ui->recentOrdersTableWidget->setColumnWidth(1,150);
    ui->recentOrdersTableWidget->setColumnWidth(2,100);
    ui->recentOrdersTableWidget->setColumnWidth(3,100);
    ui->recentOrdersTableWidget->setColumnWidth(4,150);

    ui->bidOrderTableWidget->horizontalHeader()->setVisible(true);
    ui->askOrderTableWidget->horizontalHeader()->setVisible(true);
    ui->myOrdersTableWidget->horizontalHeader()->setVisible(true);
    ui->recentOrdersTableWidget->horizontalHeader()->setVisible(true);


    ui->accountComboBox->setStyleSheet("QComboBox{border: 1px solid gray;border-radius: 3px;padding: 1px 2px 1px 8px;min-width: 9em;}"
                  "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 20px;"
                  "border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;"
                  "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                  "QComboBox::down-arrow {image: url(:/pic/pic2/comboBoxArrow.png);}"
                  );


    ui->symbolComboBox1->setStyleSheet("QComboBox{border: 1px solid gray;border-radius: 3px;padding: 1px 2px 1px 8px;min-width: 9em;}"
                  "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 20px;"
                  "border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;"
                  "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                  "QComboBox::down-arrow {image: url(:/pic/pic2/comboBoxArrow.png);}"
                  );

    ui->symbolComboBox2->setStyleSheet("QComboBox{border: 1px solid gray;border-radius: 3px;padding: 1px 2px 1px 8px;min-width: 9em;}"
                  "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 20px;"
                  "border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;"
                  "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                  "QComboBox::down-arrow {image: url(:/pic/pic2/comboBoxArrow.png);}"
                  );

    ui->tipBtn->setIconSize(QSize(18,18));
    ui->tipBtn->setStyleSheet("QPushButton{background:transparent;border:none;}");
    ui->tipBtn->setIcon(QIcon(":/pic/cplpic/info.png"));

    ui->priceLineEdit->setTextMargins(70,0,0,0);
    ui->priceLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->numLineEdit->setTextMargins(70,0,0,0);
    ui->numLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->amountLineEdit->setTextMargins(70,0,0,0);
    ui->amountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->priceLineEdit2->setTextMargins(70,0,0,0);
    ui->priceLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->numLineEdit2->setTextMargins(70,0,0,0);
    ui->numLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->amountLineEdit2->setTextMargins(70,0,0,0);
    ui->amountLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->tipBtn->setToolTip(QString::fromLocal8Bit("所有成交均会以挂单价成交，优先匹配最高价买单与最低价卖单，多余的花费成为代理收入"));



    // 账户下拉框按字母顺序排序
    QStringList keys = Hcash::getInstance()->addressMap.keys();
    ui->accountComboBox->addItems( keys);
    if( name.isEmpty())
    {
        ui->accountComboBox->setCurrentIndex(0);
    }
    else
    {
        ui->accountComboBox->setCurrentText(name);
    }

    getAssets();
    on_marketBtn_clicked();

    inited = true;
}

MarketPage::~MarketPage()
{
    delete ui;
}

void MarketPage::getAssets()
{
    foreach (int key, Hcash::getInstance()->assetInfoMap.keys())
    {
        ui->symbolComboBox1->addItem( Hcash::getInstance()->assetInfoMap.value(key).symbol);
        ui->symbolComboBox2->addItem( Hcash::getInstance()->assetInfoMap.value(key).symbol);
    }

    ui->symbolComboBox1->setCurrentIndex(0);
    ui->symbolComboBox2->setCurrentIndex(0);


}

void MarketPage::updateOrders()
{
    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    QString baseSymbol  = getCurrentQuoteOrBaseAsset(false);

    if( quoteSymbol.isEmpty() || baseSymbol.isEmpty())    return;

    Hcash::getInstance()->postRPC( toJsonFormat( "id_blockchain_market_list_bids__" + quoteSymbol + "__" + baseSymbol, "blockchain_market_list_bids",
                                               QStringList() << getAssetName(quoteSymbol) << getAssetName(baseSymbol)  ));

    Hcash::getInstance()->postRPC( toJsonFormat( "id_blockchain_market_list_asks__" + quoteSymbol + "__" + baseSymbol, "blockchain_market_list_asks",
                                               QStringList() << getAssetName(quoteSymbol) << getAssetName(baseSymbol)  ));

    ui->priceLabel->setText(QString("%1/%2").arg(quoteSymbol).arg(baseSymbol));
    ui->numLabel->setText(baseSymbol);
    ui->amountLabel->setText(quoteSymbol);
    ui->priceLabel2->setText(QString("%1/%2").arg(quoteSymbol).arg(baseSymbol));
    ui->numLabel2->setText(baseSymbol);
    ui->amountLabel2->setText(quoteSymbol);

    ui->buySymbolLabel->setText(QString::fromLocal8Bit("买入 %1").arg(baseSymbol));
    ui->sellSymbolLabel->setText(QString::fromLocal8Bit("卖出 %1").arg(baseSymbol));

}

void MarketPage::updateMyOrders()
{
    if( getCurrentQuoteOrBaseAsset().isEmpty() )    return;

    Hcash::getInstance()->postRPC( toJsonFormat( "id_wallet_market_order_list__" + getCurrentQuoteOrBaseAsset(true) + "__" + getCurrentQuoteOrBaseAsset(false),
                                               "wallet_market_order_list", QStringList() << getAssetName( getCurrentQuoteOrBaseAsset(true)) << getAssetName( getCurrentQuoteOrBaseAsset(false))  ));

}

void MarketPage::updateRecentOrdersHistory()
{
    if( getCurrentQuoteOrBaseAsset().isEmpty() )    return;

    Hcash::getInstance()->postRPC( toJsonFormat( "id_blockchain_market_order_history__" + getCurrentQuoteOrBaseAsset(true) + "__" + getCurrentQuoteOrBaseAsset(false),
                                               "blockchain_market_order_history", QStringList() << getAssetName( getCurrentQuoteOrBaseAsset(true)) << getAssetName( getCurrentQuoteOrBaseAsset(false))  ));

}

void MarketPage::refresh()
{
    updateOrders();
    updateMyOrders();
    updateRecentOrdersHistory();
    showBalance();
}

void MarketPage::on_symbolComboBox1_currentIndexChanged(const QString &arg1)
{
    if(!inited) return;

    refresh();


    editing = true;
    ui->priceLineEdit->setText("0");
    ui->numLineEdit->setText("0");
    ui->amountLineEdit->setText("0");
    ui->priceLineEdit2->setText("0");
    ui->numLineEdit2->setText("0");
    ui->amountLineEdit2->setText("0");
    editing = false;
    setPrecision();
}

void MarketPage::on_symbolComboBox2_currentIndexChanged(const QString &arg1)
{
    if(!inited) return;

    refresh();


    editing = true;
    ui->priceLineEdit->setText("0");
    ui->numLineEdit->setText("0");
    ui->amountLineEdit->setText("0");
    ui->priceLineEdit2->setText("0");
    ui->numLineEdit2->setText("0");
    ui->amountLineEdit2->setText("0");
    editing = false;
    setPrecision();
}

void MarketPage::jsonDataUpdated(QString id)
{    
    if( id ==    "id_blockchain_market_list_bids__" + getCurrentQuoteOrBaseAsset(true) + "__" + getCurrentQuoteOrBaseAsset(false) )
    {
        QString result = Hcash::getInstance()->jsonDataValue(id);

        parseBidOrders(result);
        showBidOrderList();
        return;
    }

    if( id ==    "id_blockchain_market_list_asks__" + getCurrentQuoteOrBaseAsset(true) + "__" + getCurrentQuoteOrBaseAsset(false))
    {
        QString result = Hcash::getInstance()->jsonDataValue(id);

        parseAskOrders(result);
        showAskOrderList();

        return;
    }



    if(     id == "id_wallet_market_order_list__" + getCurrentQuoteOrBaseAsset(true) + "__" + getCurrentQuoteOrBaseAsset(false) )
    {
        QString result = Hcash::getInstance()->jsonDataValue(id);

        parseMyOrders(result);
        showMyOrders();

        return;
    }

    if( id == "id_blockchain_market_order_history__" + getCurrentQuoteOrBaseAsset(true) + "__" + getCurrentQuoteOrBaseAsset(false))
    {
        QString result = Hcash::getInstance()->jsonDataValue(id);

        parseOrderHistory(result);
        showRecentOrders();

        return;
    }

    if( id.startsWith("id_wallet_market_cancel_order__"))
    {
        QString result = Hcash::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\""))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(QString::fromLocal8Bit("撤单交易已发出"));
            commonDialog.pop();
        }

        return;
    }

    if( id == "id_wallet_market_submit_bid" || id == "id_wallet_market_submit_ask")
    {
        QString result = Hcash::getInstance()->jsonDataValue(id);
        qDebug() << result;

        if( result.startsWith("\"result\""))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(QString::fromLocal8Bit("挂单交易已发出"));
            commonDialog.pop();
        }
        else if( result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( QString::fromLocal8Bit("挂单失败: ") + errorMessage);
            commonDialog.pop();
        }

        return;
    }

}

void MarketPage::parseBidOrders(QString result)
{
    bidOrdersMap.remove(getCurrentQuoteOrBaseAsset(true) + "," + getCurrentQuoteOrBaseAsset(false));

    result.prepend("{");
    result.append("}");

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if( parse_doucment.isObject())
        {
            QJsonObject jsonObject = parse_doucment.object();
            if( jsonObject.contains("result"))
            {
                QJsonValue resultValue = jsonObject.take("result");
                if( resultValue.isArray())
                {
                    OrdersInfoVector ordersInfoVector;
                    QJsonArray resultArray = resultValue.toArray();
                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        OrderInfo orderInfo;

                        QJsonObject object          = resultArray.at(i).toObject();
                        orderInfo.type              = object.take("type").toString();

                        QJsonObject marketIndexObject = object.take("market_index").toObject();
                        orderInfo.owner             = marketIndexObject.take("owner").toString();

                        QJsonObject orderPriceObject = marketIndexObject.take("order_price").toObject();
                        orderInfo.ratio             = orderPriceObject.take("ratio").toString();
                        orderInfo.quoteAssetId      = orderPriceObject.take("quote_asset_id").toInt();
                        orderInfo.baseAssetId       = orderPriceObject.take("base_asset_id").toInt();

                        QJsonObject stateObject     = object.take("state").toObject();
                        QJsonValue balanceValue      = stateObject.take("balance");
                        if( balanceValue.isString())
                        {
                            orderInfo.orderBalance  = balanceValue.toString().toDouble();
                        }
                        else
                        {
                            orderInfo.orderBalance  = balanceValue.toDouble();
                        }

                        ordersInfoVector.append(orderInfo);
                    }

                    // TODOTOMORROW ask和bid混在了一起
                    bidOrdersMap.insert(getCurrentQuoteOrBaseAsset(true) + "," + getCurrentQuoteOrBaseAsset(false), ordersInfoVector);
                }
            }
        }
    }

}

void MarketPage::parseAskOrders(QString result)
{
    askOrdersMap.remove(getCurrentQuoteOrBaseAsset(true) + "," + getCurrentQuoteOrBaseAsset(false));

    result.prepend("{");
    result.append("}");

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if( parse_doucment.isObject())
        {
            QJsonObject jsonObject = parse_doucment.object();
            if( jsonObject.contains("result"))
            {
                QJsonValue resultValue = jsonObject.take("result");
                if( resultValue.isArray())
                {
                    OrdersInfoVector ordersInfoVector;
                    QJsonArray resultArray = resultValue.toArray();
                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        OrderInfo orderInfo;

                        QJsonObject object          = resultArray.at(i).toObject();
                        orderInfo.type              = object.take("type").toString();

                        QJsonObject marketIndexObject = object.take("market_index").toObject();
                        orderInfo.owner             = marketIndexObject.take("owner").toString();

                        QJsonObject orderPriceObject = marketIndexObject.take("order_price").toObject();
                        orderInfo.ratio             = orderPriceObject.take("ratio").toString();
                        orderInfo.quoteAssetId      = orderPriceObject.take("quote_asset_id").toInt();
                        orderInfo.baseAssetId       = orderPriceObject.take("base_asset_id").toInt();

                        QJsonObject stateObject     = object.take("state").toObject();
                        QJsonValue balanceValue      = stateObject.take("balance");
                        if( balanceValue.isString())
                        {
                            orderInfo.orderBalance  = balanceValue.toString().toDouble();
                        }
                        else
                        {
                            orderInfo.orderBalance  = balanceValue.toDouble();
                        }

                        ordersInfoVector.append(orderInfo);
                    }

                    // TODOTOMORROW ask和bid混在了一起
                    askOrdersMap.insert(getCurrentQuoteOrBaseAsset(true) + "," + getCurrentQuoteOrBaseAsset(false), ordersInfoVector);
                }
            }
        }
    }
}

void MarketPage::showBidOrderList()
{
    ui->bidOrderTableWidget->setRowCount(0);

    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    QString baseSymbol  = getCurrentQuoteOrBaseAsset(false);

    ui->bidOrderTableWidget->horizontalHeaderItem(0)->setText(QString::fromLocal8Bit("单价") + QString("(%1/%2)").arg(quoteSymbol).arg(baseSymbol));
    ui->bidOrderTableWidget->horizontalHeaderItem(1)->setText(baseSymbol);
    ui->bidOrderTableWidget->horizontalHeaderItem(2)->setText(quoteSymbol);

    if( !bidOrdersMap.contains(quoteSymbol + "," + baseSymbol))  return;
    OrdersInfoVector ordersInfoVector = bidOrdersMap.value(quoteSymbol + "," + baseSymbol);

    int size = ordersInfoVector.size();
    ui->bidOrderTableWidget->setRowCount(size);

    for( int i = 0; i < size; i++)
    {
        OrderInfo info = ordersInfoVector.at(i);

        AssetInfo quoteAssetInfo = Hcash::getInstance()->assetInfoMap.value(info.quoteAssetId);
        AssetInfo baseAssetInfo  = Hcash::getInstance()->assetInfoMap.value(info.baseAssetId);

        if( info.type == "bid_order")
        {
            ui->bidOrderTableWidget->setItem(i,0,new QTableWidgetItem( QString::number( info.ratio.toDouble() * baseAssetInfo.precision / quoteAssetInfo.precision,'g',18) ));
            ui->bidOrderTableWidget->setItem(i,1,new QTableWidgetItem(QString::number(info.orderBalance / quoteAssetInfo.precision / (info.ratio.toDouble() * baseAssetInfo.precision / quoteAssetInfo.precision),'g',18) ));
            ui->bidOrderTableWidget->setItem(i,2,new QTableWidgetItem(QString::number(info.orderBalance / quoteAssetInfo.precision,'g',18) ));

            for(int j = 0; j < 3; j++)
            {
                ui->bidOrderTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
            }

        }
    }
}

void MarketPage::showAskOrderList()
{
    ui->askOrderTableWidget->setRowCount(0);

    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    QString baseSymbol  = getCurrentQuoteOrBaseAsset(false);

    ui->askOrderTableWidget->horizontalHeaderItem(0)->setText(QString::fromLocal8Bit("单价") + QString("(%1/%2)").arg(quoteSymbol).arg(baseSymbol));
    ui->askOrderTableWidget->horizontalHeaderItem(1)->setText(baseSymbol);
    ui->askOrderTableWidget->horizontalHeaderItem(2)->setText(quoteSymbol);

    if( !askOrdersMap.contains(quoteSymbol + "," + baseSymbol))  return;
    OrdersInfoVector ordersInfoVector = askOrdersMap.value(quoteSymbol + "," + baseSymbol);

    int size = ordersInfoVector.size();
    ui->askOrderTableWidget->setRowCount(size);

    for( int i = 0; i < size; i++)
    {
        OrderInfo info = ordersInfoVector.at(i);

        AssetInfo quoteAssetInfo = Hcash::getInstance()->assetInfoMap.value(info.quoteAssetId);
        AssetInfo baseAssetInfo  = Hcash::getInstance()->assetInfoMap.value(info.baseAssetId);

        if( info.type == "ask_order")
        {
            ui->askOrderTableWidget->setItem(i,0,new QTableWidgetItem(QString::number( info.ratio.toDouble() * baseAssetInfo.precision / quoteAssetInfo.precision,'g',18) ));
            ui->askOrderTableWidget->setItem(i,1,new QTableWidgetItem(QString::number(info.orderBalance / baseAssetInfo.precision,'g',18) ));
            ui->askOrderTableWidget->setItem(i,2,new QTableWidgetItem(QString::number(info.orderBalance / baseAssetInfo.precision * (info.ratio.toDouble() * baseAssetInfo.precision / quoteAssetInfo.precision),'g',18) ));

            for(int j = 0; j < 3; j++)
            {
                ui->askOrderTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
}


void MarketPage::parseMyOrders(QString result)
{
    result.prepend("{");
    result.append("}");

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if( parse_doucment.isObject())
        {
            QJsonObject jsonObject = parse_doucment.object();
            if( jsonObject.contains("result"))
            {
                QJsonValue resultValue = jsonObject.take("result");
                if( resultValue.isArray())
                {
                    OrdersInfoVector ordersInfoVector;
                    QJsonArray resultArray = resultValue.toArray();

                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        OrderInfo orderInfo;

                        orderInfo.orderId           = resultArray.at(i).toArray().at(0).toString();

                        QJsonObject object          = resultArray.at(i).toArray().at(1).toObject();
                        orderInfo.type              = object.take("type").toString();

                        QJsonObject marketIndexObject = object.take("market_index").toObject();
                        orderInfo.owner             = marketIndexObject.take("owner").toString();

                        QJsonObject orderPriceObject = marketIndexObject.take("order_price").toObject();
                        orderInfo.ratio             = orderPriceObject.take("ratio").toString();
                        orderInfo.quoteAssetId      = orderPriceObject.take("quote_asset_id").toInt();
                        orderInfo.baseAssetId       = orderPriceObject.take("base_asset_id").toInt();

                        QJsonObject stateObject     = object.take("state").toObject();
                        QJsonValue balanceValue      = stateObject.take("balance");
                        if( balanceValue.isString())
                        {
                            orderInfo.orderBalance  = balanceValue.toString().toDouble();
                        }
                        else
                        {
                            orderInfo.orderBalance  = balanceValue.toDouble();
                        }

                        ordersInfoVector.append(orderInfo);
                    }

                    myOrdersMap.insert( getCurrentQuoteOrBaseAsset(true) + "," + getCurrentQuoteOrBaseAsset(false), ordersInfoVector);

                }
            }
        }
    }


}

void MarketPage::showMyOrders()
{
    ui->myOrdersTableWidget->setRowCount(0);

    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    QString baseSymbol  = getCurrentQuoteOrBaseAsset(false);

    ui->myOrdersTableWidget->horizontalHeaderItem(3)->setText(QString::fromLocal8Bit("单价 ") + quoteSymbol + "/" + baseSymbol);
    ui->myOrdersTableWidget->horizontalHeaderItem(4)->setText(QString::fromLocal8Bit("数量 ") + baseSymbol);
    ui->myOrdersTableWidget->horizontalHeaderItem(5)->setText(QString::fromLocal8Bit("金额 ") + quoteSymbol);

    if( !myOrdersMap.contains(quoteSymbol + "," + baseSymbol))  return;
    OrdersInfoVector ordersInfoVector = myOrdersMap.value(getCurrentQuoteOrBaseAsset(true) + "," + getCurrentQuoteOrBaseAsset(false));

    int size = ordersInfoVector.size();
    ui->myOrdersTableWidget->setRowCount(size);
    for( int i = 0; i < size; i++)
    {
        OrderInfo info = ordersInfoVector.at(i);

        AssetInfo quoteAssetInfo = Hcash::getInstance()->assetInfoMap.value(info.quoteAssetId);
        AssetInfo baseAssetInfo  = Hcash::getInstance()->assetInfoMap.value(info.baseAssetId);

        ui->myOrdersTableWidget->setItem(i,0,new QTableWidgetItem(info.orderId));
        ui->myOrdersTableWidget->setItem(i,1,new QTableWidgetItem(info.owner));


        ui->myOrdersTableWidget->setItem(i,3,new QTableWidgetItem(QString::number( info.ratio.toDouble() * baseAssetInfo.precision / quoteAssetInfo.precision,'g',18)));


        if( info.type == "ask_order")
        {
            ui->myOrdersTableWidget->setItem(i,2,new QTableWidgetItem(QString::fromLocal8Bit("卖") ));
            ui->myOrdersTableWidget->item(i,2)->setTextColor(QColor(239,104,60));
            ui->myOrdersTableWidget->setItem(i,4,new QTableWidgetItem(QString::number(info.orderBalance / baseAssetInfo.precision,'g',18) ));
            ui->myOrdersTableWidget->setItem(i,5,new QTableWidgetItem(QString::number(info.orderBalance / baseAssetInfo.precision * ( info.ratio.toDouble() * baseAssetInfo.precision / quoteAssetInfo.precision),'g',18) ));

        }
        else if( info.type == "bid_order")
        {
            ui->myOrdersTableWidget->setItem(i,2,new QTableWidgetItem(QString::fromLocal8Bit("买") ));
            ui->myOrdersTableWidget->item(i,2)->setTextColor(QColor(113,203,90));
            ui->myOrdersTableWidget->setItem(i,4,new QTableWidgetItem(QString::number(info.orderBalance / quoteAssetInfo.precision / ( info.ratio.toDouble() * baseAssetInfo.precision / quoteAssetInfo.precision),'g',18) ));
            ui->myOrdersTableWidget->setItem(i,5,new QTableWidgetItem(QString::number(info.orderBalance / quoteAssetInfo.precision,'g',18) ));
        }

        ui->myOrdersTableWidget->setItem(i,6,new QTableWidgetItem(QString::fromLocal8Bit("撤销挂单")));
        ui->myOrdersTableWidget->item(i,6)->setTextColor(QColor(154,99,86));

        for(int j = 0; j < 7; j++)
        {
            ui->myOrdersTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void MarketPage::setPrecision()
{
    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    QString baseSymbol  = getCurrentQuoteOrBaseAsset(false);

    if( quoteSymbol.isEmpty() || baseSymbol.isEmpty())    return;

    int quoteAsset      = Hcash::getInstance()->getAssetId(quoteSymbol);
    int baseAsset       = Hcash::getInstance()->getAssetId(baseSymbol);

    int quotePrecison   = QString::number( Hcash::getInstance()->assetInfoMap.value(quoteAsset).precision, 'g', 15).count() - 1;
    int basePrecison    = QString::number( Hcash::getInstance()->assetInfoMap.value(baseAsset).precision, 'g', 15).count() - 1;



    QRegExp rx1(QString( "^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,15})?$|(^\\t?$)"));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->priceLineEdit->setValidator(pReg1);
    ui->priceLineEdit2->setValidator(pReg1);

    QRegExp rx3(QString( "^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(quotePrecison));
    QRegExpValidator *pReg3 = new QRegExpValidator(rx3, this);
    ui->amountLineEdit->setValidator(pReg3);
    ui->amountLineEdit2->setValidator(pReg3);

    QRegExp rx2(QString( "^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(basePrecison));
//    QRegExp rx2(QString( "^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,15})?$|(^\\t?$)"));
    QRegExpValidator *pReg2 = new QRegExpValidator(rx2, this);
    ui->numLineEdit->setValidator(pReg2);
    ui->numLineEdit2->setValidator(pReg2);

}

void MarketPage::parseOrderHistory(QString result)
{
    recentOrdersMap.remove(getCurrentQuoteOrBaseAsset(true) + "," + getCurrentQuoteOrBaseAsset(false));

    result.prepend("{");
    result.append("}");

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if( parse_doucment.isObject())
        {
            QJsonObject jsonObject = parse_doucment.object();
            if( jsonObject.contains("result"))
            {
                QJsonValue resultValue = jsonObject.take("result");
                if( resultValue.isArray())
                {
                    OrdersHistoryVector ordersHistoryVector;
                    QJsonArray resultArray = resultValue.toArray();
                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        OrderHistory orderHistory;

                        QJsonObject object          = resultArray.at(i).toObject();
                        QJsonObject bidIndexObject  = object.take("bid_index").toObject();
                        orderHistory.bidOwner       = bidIndexObject.take("owner").toString();

                        QJsonObject bidOrderPriceObject   = bidIndexObject.take("order_price").toObject();
                        orderHistory.bidRatio       = bidOrderPriceObject.take("ratio").toString();
                        orderHistory.quoteAssetId   = bidOrderPriceObject.take("quote_asset_id").toInt();
                        orderHistory.baseAssetId    = bidOrderPriceObject.take("base_asset_id").toInt();

                        QJsonObject askIndexObject  = object.take("ask_index").toObject();
                        orderHistory.askOwner       = askIndexObject.take("owner").toString();
                        QJsonObject askOrderPriceObject   = askIndexObject.take("order_price").toObject();
                        orderHistory.askRatio       = askOrderPriceObject.take("ratio").toString();

                        QJsonObject bidPaidObject   = object.take("bid_paid").toObject();
                        QJsonValue bidPaidAmountValue  = bidPaidObject.take("amount");
                        if( bidPaidAmountValue.isString())
                        {
                            orderHistory.bidPaidAmount  = bidPaidAmountValue.toString().toDouble();
                        }
                        else
                        {
                            orderHistory.bidPaidAmount  = bidPaidAmountValue.toDouble();
                        }

                        QJsonObject askPaidObject   = object.take("ask_paid").toObject();
                        QJsonValue askPaidAmountValue  = askPaidObject.take("amount");
                        if( askPaidAmountValue.isString())
                        {
                            orderHistory.askPaidAmount  = askPaidAmountValue.toString().toDouble();
                        }
                        else
                        {
                            orderHistory.askPaidAmount  = askPaidAmountValue.toDouble();
                        }

                        orderHistory.timeStamp      = object.take("timestamp").toString();

                        ordersHistoryVector.append(orderHistory);
                    }

                    recentOrdersMap.insert(getCurrentQuoteOrBaseAsset(true) + "," + getCurrentQuoteOrBaseAsset(false), ordersHistoryVector);
                }
            }
        }
    }
}

void MarketPage::showRecentOrders()
{
    ui->recentOrdersTableWidget->setRowCount(0);

    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    QString baseSymbol  = getCurrentQuoteOrBaseAsset(false);

    ui->recentOrdersTableWidget->horizontalHeaderItem(0)->setText(QString::fromLocal8Bit("买一价 ") + quoteSymbol + "/" + baseSymbol);
    ui->recentOrdersTableWidget->horizontalHeaderItem(1)->setText(QString::fromLocal8Bit("卖一价 ") + quoteSymbol + "/" + baseSymbol);
    ui->recentOrdersTableWidget->horizontalHeaderItem(2)->setText( quoteSymbol);
    ui->recentOrdersTableWidget->horizontalHeaderItem(3)->setText( baseSymbol);

    if( !recentOrdersMap.contains(getCurrentQuoteOrBaseAsset(true) + "," + getCurrentQuoteOrBaseAsset(false)))  return;
    OrdersHistoryVector ordersHistoryVector = recentOrdersMap.value(getCurrentQuoteOrBaseAsset(true) + "," + getCurrentQuoteOrBaseAsset(false));

    int size = ordersHistoryVector.size();
    ui->recentOrdersTableWidget->setRowCount(size);
    for( int i = 0; i < size; i++)
    {
        OrderHistory history = ordersHistoryVector.at(i);

        AssetInfo quoteAssetInfo = Hcash::getInstance()->assetInfoMap.value(history.quoteAssetId);
        AssetInfo baseAssetInfo  = Hcash::getInstance()->assetInfoMap.value(history.baseAssetId);

        ui->recentOrdersTableWidget->setItem(i,0,new QTableWidgetItem( QString::number( history.bidRatio.toDouble() * baseAssetInfo.precision / quoteAssetInfo.precision,'g',18) ));
        ui->recentOrdersTableWidget->setItem(i,1,new QTableWidgetItem( QString::number( history.askRatio.toDouble() * baseAssetInfo.precision / quoteAssetInfo.precision,'g',18) ));
        ui->recentOrdersTableWidget->setItem(i,2,new QTableWidgetItem( QString::number( history.bidPaidAmount / quoteAssetInfo.precision,'g',18) ));
        ui->recentOrdersTableWidget->setItem(i,3,new QTableWidgetItem( QString::number( history.askPaidAmount / baseAssetInfo.precision,'g',18) ));

        QString date = history.timeStamp;
        date.replace(QString("T"),QString(" "));
        QDateTime time = QDateTime::fromString(date, "yyyy-MM-dd hh:mm:ss");
        time = time.addSecs(28800);       // 时间加8小时
        QString currentDateTime = time.toString("yyyy-MM-dd hh:mm:ss");
        ui->recentOrdersTableWidget->setItem(i,4,new QTableWidgetItem(currentDateTime));

        for(int j = 0; j < 5; j++)
        {
            ui->recentOrdersTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void MarketPage::showBalance()
{
    AssetBalanceMap assetBalanceMap = Hcash::getInstance()->accountBalanceMap.value(ui->accountComboBox->currentText());
    int quoteAssetId = Hcash::getInstance()->getAssetId(getCurrentQuoteOrBaseAsset(true));
    int baseAssetId  = Hcash::getInstance()->getAssetId(getCurrentQuoteOrBaseAsset(false));
    if( quoteAssetId < 0 || baseAssetId < 0 )    return;
    double quoteBalance = assetBalanceMap.value(quoteAssetId);
    double baseBalance  = assetBalanceMap.value(baseAssetId);

    ui->quoteBalanceLabel->setText( QString::number(quoteBalance / Hcash::getInstance()->assetInfoMap.value(quoteAssetId).precision,'g',18) + " " + getCurrentQuoteOrBaseAsset(true) );
    ui->baseBalanceLabel->setText( QString::number(baseBalance / Hcash::getInstance()->assetInfoMap.value(baseAssetId).precision,'g',18) + " " + getCurrentQuoteOrBaseAsset(false) );
}

void MarketPage::on_marketBtn_clicked()
{
    ui->bidOrderTableWidget->show();
    ui->askOrderTableWidget->show();
    ui->myOrdersTableWidget->hide();
    ui->recentOrdersTableWidget->hide();

    ui->buySymbolLabel->show();
    ui->sellSymbolLabel->show();
    ui->tipBtn->show();

    ui->marketBtn->setStyleSheet(MARKETPAGE_BTN_SELECTED_STYLE);
    ui->myOrdersBtn->setStyleSheet(MARKETPAGE_BTN_UNSELECTED_STYLE);
    ui->myHistoryBtn->setStyleSheet(MARKETPAGE_BTN_UNSELECTED_STYLE);

}

void MarketPage::on_myOrdersBtn_clicked()
{
    ui->bidOrderTableWidget->hide();
    ui->askOrderTableWidget->hide();
    ui->myOrdersTableWidget->show();
    ui->recentOrdersTableWidget->hide();

    ui->buySymbolLabel->hide();
    ui->sellSymbolLabel->hide();
    ui->tipBtn->hide();

    ui->marketBtn->setStyleSheet(MARKETPAGE_BTN_UNSELECTED_STYLE);
    ui->myOrdersBtn->setStyleSheet(MARKETPAGE_BTN_SELECTED_STYLE);
    ui->myHistoryBtn->setStyleSheet(MARKETPAGE_BTN_UNSELECTED_STYLE);

}

void MarketPage::on_myHistoryBtn_clicked()
{
    ui->bidOrderTableWidget->hide();
    ui->askOrderTableWidget->hide();
    ui->myOrdersTableWidget->hide();
    ui->recentOrdersTableWidget->show();

    ui->buySymbolLabel->hide();
    ui->sellSymbolLabel->hide();
    ui->tipBtn->hide();

    ui->marketBtn->setStyleSheet(MARKETPAGE_BTN_UNSELECTED_STYLE);
    ui->myOrdersBtn->setStyleSheet(MARKETPAGE_BTN_UNSELECTED_STYLE);
    ui->myHistoryBtn->setStyleSheet(MARKETPAGE_BTN_SELECTED_STYLE);
}

QString MarketPage::getCurrentQuoteOrBaseAsset(bool isQuote)
{
    int asset1 = Hcash::getInstance()->getAssetId(ui->symbolComboBox1->currentText());
    int asset2   = Hcash::getInstance()->getAssetId(ui->symbolComboBox2->currentText());

    if( asset1 == asset2)   return "";    // 如果symbolComboBox1 和 symbolComboBox2 选择的一样 返回空

    if( isQuote)
    {
        return  (asset1 > asset2)?(ui->symbolComboBox1->currentText()):(ui->symbolComboBox2->currentText());
    }
    else
    {
        return  (asset1 < asset2)?(ui->symbolComboBox1->currentText()):(ui->symbolComboBox2->currentText());
    }
}



void MarketPage::on_myOrdersTableWidget_cellClicked(int row, int column)
{
    if( column == 6)
    {
        QString id = ui->myOrdersTableWidget->item(row,0)->text();

        Hcash::getInstance()->postRPC( toJsonFormat( "id_wallet_market_cancel_order__" + id, "wallet_market_cancel_order", QStringList() << id ));

        return;
    }
}

void MarketPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if( !inited)    return;

    Hcash::getInstance()->currentAccount = arg1;
    showBalance();
}

void MarketPage::on_bidBtn_clicked()
{
    mutexForAddressMap.lock();
    int size = Hcash::getInstance()->addressMap.size();
    mutexForAddressMap.unlock();
    if( size == 0)   // 有至少一个账户
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(QString::fromLocal8Bit("您还没有创建账户!"));
        commonDialog.pop();

        return;
    }

    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    QString baseSymbol  = getCurrentQuoteOrBaseAsset(false);

    if( quoteSymbol.isEmpty() || baseSymbol.isEmpty())    return;


    Hcash::getInstance()->postRPC( toJsonFormat( "id_wallet_market_submit_bid", "wallet_market_submit_bid",
                                               QStringList() << ui->accountComboBox->currentText() << ui->numLineEdit->text() << getAssetName(baseSymbol)
                                               << ui->priceLineEdit->text() << getAssetName(quoteSymbol)  << "true"
                                               ));

}

void MarketPage::on_askBtn_clicked()
{
    mutexForAddressMap.lock();
    int size = Hcash::getInstance()->addressMap.size();
    mutexForAddressMap.unlock();
    if( size == 0)   // 有至少一个账户
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(QString::fromLocal8Bit("您还没有创建账户!"));
        commonDialog.pop();

        return;
    }

    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    QString baseSymbol  = getCurrentQuoteOrBaseAsset(false);

    if( quoteSymbol.isEmpty() || baseSymbol.isEmpty())    return;


    Hcash::getInstance()->postRPC( toJsonFormat( "id_wallet_market_submit_ask", "wallet_market_submit_ask",
                                               QStringList() << ui->accountComboBox->currentText() << ui->numLineEdit2->text() << getAssetName(baseSymbol)
                                               << ui->priceLineEdit2->text() << getAssetName(quoteSymbol)  << "true"
                                               ));
}

void MarketPage::on_priceLineEdit_textChanged(const QString &arg1)
{
    if( editing)    return;
    editing = true;
    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    if( quoteSymbol.isEmpty())    return;

    double amount = ui->priceLineEdit->text().toDouble() * ui->numLineEdit->text().toDouble();

    int quoteAsset      = Hcash::getInstance()->getAssetId(quoteSymbol);
//    int quotePrecison   = QString::number( Hcash::getInstance()->assetInfoMap.value(quoteAsset).precision, 'g', 15).count() - 1;

//    qDebug() << QString::number(amount,'f',quotePrecison).toDouble();
//    qDebug() << QString::number(QString::number(amount,'f',quotePrecison).toDouble(),'g',18);
    ui->amountLineEdit->setText( QString::number(roundDown(amount,Hcash::getInstance()->assetInfoMap.value(quoteAsset).precision),'g',18) );


    editing = false;
}

void MarketPage::on_numLineEdit_textChanged(const QString &arg1)
{
    if( editing)    return;
    editing = true;
    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    if( quoteSymbol.isEmpty())    return;

    double amount = ui->priceLineEdit->text().toDouble() * ui->numLineEdit->text().toDouble();

    int quoteAsset      = Hcash::getInstance()->getAssetId(quoteSymbol);
//    int quotePrecison   = QString::number( Hcash::getInstance()->assetInfoMap.value(quoteAsset).precision, 'g', 15).count() - 1;

//    ui->amountLineEdit->setText( QString::number(QString::number(amount,'f',quotePrecison).toDouble(),'g',18) );
    ui->amountLineEdit->setText( QString::number(roundDown(amount,Hcash::getInstance()->assetInfoMap.value(quoteAsset).precision),'g',18) );


    editing = false;
}

void MarketPage::on_amountLineEdit_textChanged(const QString &arg1)
{
    if( editing)    return;
    editing = true;
    QString baseSymbol = getCurrentQuoteOrBaseAsset(false);
    if( baseSymbol.isEmpty())    return;
    if( ui->priceLineEdit->text().toDouble() <= 0)  ui->numLineEdit->setText("0");

    double num = ui->amountLineEdit->text().toDouble() / ui->priceLineEdit->text().toDouble();
    int baseAsset      = Hcash::getInstance()->getAssetId(baseSymbol);
    int basePrecision   = QString::number( Hcash::getInstance()->assetInfoMap.value(baseAsset).precision, 'g', 15).count() - 1;

    ui->numLineEdit->setText( QString::number( QString::number(num,'f',basePrecision).toDouble(),'g',18));

    editing = false;
}

void MarketPage::on_priceLineEdit2_textChanged(const QString &arg1)
{
    if( editing)    return;
    editing = true;
    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    if( quoteSymbol.isEmpty())    return;

    double amount = ui->priceLineEdit2->text().toDouble() * ui->numLineEdit2->text().toDouble();

    int quoteAsset      = Hcash::getInstance()->getAssetId(quoteSymbol);
//    int quotePrecison   = QString::number( Hcash::getInstance()->assetInfoMap.value(quoteAsset).precision, 'g', 15).count() - 1;

//    ui->amountLineEdit2->setText( QString::number(QString::number(amount,'f',quotePrecison).toDouble(),'g',18) );
    ui->amountLineEdit2->setText( QString::number(roundDown(amount,Hcash::getInstance()->assetInfoMap.value(quoteAsset).precision),'g',18) );


    editing = false;
}

void MarketPage::on_numLineEdit2_textChanged(const QString &arg1)
{
    if( editing)    return;
    editing = true;
    QString quoteSymbol = getCurrentQuoteOrBaseAsset(true);
    if( quoteSymbol.isEmpty())    return;

    double amount = ui->priceLineEdit2->text().toDouble() * ui->numLineEdit2->text().toDouble();

    int quoteAsset      = Hcash::getInstance()->getAssetId(quoteSymbol);
//    int quotePrecison   = QString::number( Hcash::getInstance()->assetInfoMap.value(quoteAsset).precision, 'g', 15).count() - 1;

//    ui->amountLineEdit2->setText( QString::number(QString::number(amount,'f',quotePrecison).toDouble(),'g',18) );
    ui->amountLineEdit2->setText( QString::number(roundDown(amount,Hcash::getInstance()->assetInfoMap.value(quoteAsset).precision),'g',18) );


    editing = false;
}

void MarketPage::on_amountLineEdit2_textChanged(const QString &arg1)
{
    if( editing)    return;
    editing = true;
    QString baseSymbol = getCurrentQuoteOrBaseAsset(false);
    if( baseSymbol.isEmpty())    return;
    if( ui->priceLineEdit2->text().toDouble() <= 0)  ui->numLineEdit2->setText("0");

    double num = ui->amountLineEdit2->text().toDouble() / ui->priceLineEdit2->text().toDouble();
    int baseAsset      = Hcash::getInstance()->getAssetId(baseSymbol);
    int basePrecision   = QString::number( Hcash::getInstance()->assetInfoMap.value(baseAsset).precision, 'g', 15).count() - 1;

    ui->numLineEdit2->setText( QString::number( QString::number(num,'f',basePrecision).toDouble(),'g',18));

    editing = false;
}

void MarketPage::on_bidOrderTableWidget_cellClicked(int row, int column)
{
    ui->priceLineEdit->setText( ui->bidOrderTableWidget->item(row,0)->text());
    ui->priceLineEdit2->setText( ui->bidOrderTableWidget->item(row,0)->text());
}

void MarketPage::on_askOrderTableWidget_cellClicked(int row, int column)
{
    ui->priceLineEdit->setText( ui->askOrderTableWidget->item(row,0)->text());
    ui->priceLineEdit2->setText( ui->askOrderTableWidget->item(row,0)->text());
}



