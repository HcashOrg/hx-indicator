#include "ExchangeModeWidget.h"
#include "ui_ExchangeModeWidget.h"

#include "ExchangePairSelectDialog.h"

ExchangeModeWidget::ExchangeModeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangeModeWidget)
{
    ui->setupUi(this);
    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->sellPositionTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->sellPositionTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->sellPositionTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->sellPositionTableWidget->setFrameShape(QFrame::NoFrame);
    ui->sellPositionTableWidget->setMouseTracking(true);
    ui->sellPositionTableWidget->setShowGrid(false);//隐藏表格线
    ui->sellPositionTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->sellPositionTableWidget->horizontalHeader()->setVisible(false);
    ui->sellPositionTableWidget->verticalHeader()->setVisible(false);
    ui->sellPositionTableWidget->setColumnWidth(0,40);
    ui->sellPositionTableWidget->setColumnWidth(1,60);
    ui->sellPositionTableWidget->setColumnWidth(2,60);
    ui->sellPositionTableWidget->setColumnWidth(3,36);

    init();
}

ExchangeModeWidget::~ExchangeModeWidget()
{
    delete ui;
}

void ExchangeModeWidget::init()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);

    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }
}

void ExchangeModeWidget::jsonDataUpdated(QString id)
{
    if( id == "ExchangeModeWidget+invoke_contract_offline+getUserBalances+" + ui->accountComboBox->currentText())
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
            QStringList assetSymbols = resultObject.keys();

            foreach(QString assetSymbol, assetSymbols)
            {
                QJsonObject object = resultObject.value(assetSymbol).toObject();
                ExchangeBalance balance;
                if(object.contains("locked"))   balance.locked = jsonValueToULL( object.value("locked"));
                if(object.contains("available"))   balance.available = jsonValueToULL( object.value("available"));
                assetExchangeBalanceMap.insert(assetSymbol, balance);
            }

            onPairSelected(HXChain::getInstance()->currentExchangePair);
        }

        return;
    }

    if( id == "ExchangeModeWidget+invoke_contract_offline+getSellOrders+" +
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
            QJsonArray resultArray = QJsonDocument::fromJson(resultStr.toLatin1()).array();

            sellOrdersVector.clear();
            foreach (QJsonValue v, resultArray)
            {
                QString str = v.toString();
                QStringList strList = str.split(",");
                OrderInfo orderInfo;
                orderInfo.baseAmount = strList.at(0).toULongLong();
                orderInfo.quoteAmount = strList.at(1).toULongLong();
                orderInfo.address = strList.at(2);
                orderInfo.trxId = strList.at(3);
                sellOrdersVector.append(orderInfo);
            }

            AssetInfo baseAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.first));
            AssetInfo quoteAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(HXChain::getInstance()->currentExchangePair.second));

            int size = sellOrdersVector.size();
            ui->sellPositionTableWidget->setRowCount(0);
            ui->sellPositionTableWidget->setRowCount(size);
            for(int i = 0; i < size; i++)
            {
                const OrderInfo& orderInfo = sellOrdersVector.at(i);
                ui->sellPositionTableWidget->setItem(i,0, new QTableWidgetItem(tr("Sell%1").arg(i + 1)));

                double price = (double)orderInfo.baseAmount / qPow(10,baseAssetInfo.precision) / orderInfo.quoteAmount * qPow(10,quoteAssetInfo.precision);
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(price,'g',8));
//                item->setData(Qt::UserRole,contractAddress);
                ui->sellPositionTableWidget->setItem(i,1,item);

                ui->sellPositionTableWidget->setItem(i,2, new QTableWidgetItem( getBigNumberString(orderInfo.baseAmount, baseAssetInfo.precision)));

            }
        }

        return;
    }
}

void ExchangeModeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void ExchangeModeWidget::on_orderModeBtn_clicked()
{
    close();
}

void ExchangeModeWidget::on_favoriteMarketBtn_clicked()
{
    ExchangePairSelectDialog dialog;
    connect(&dialog, &ExchangePairSelectDialog::pairSelected, this, &ExchangeModeWidget::onPairSelected);
    dialog.move(ui->favoriteMarketBtn->mapToGlobal( QPoint(ui->favoriteMarketBtn->width() / 2 - dialog.width() / 2,ui->favoriteMarketBtn->height())));

    dialog.exec();
}

void ExchangeModeWidget::on_marketBtn1_clicked()
{

}

void ExchangeModeWidget::onPairSelected(const ExchangePair &_pair)
{
    HXChain::getInstance()->currentExchangePair = _pair;
    ui->currentPairLabel->setText( QString("%1 / %2").arg(revertERCSymbol(_pair.first)).arg(revertERCSymbol(_pair.second)));

    ui->buyPriceLabel->setText(tr("Price (%1)").arg(revertERCSymbol( _pair.second)));
    ui->sellPriceLabel->setText(tr("Price (%1)").arg(revertERCSymbol( _pair.second)));
    ui->buyAmountLabel->setText(tr("Amount (%1)").arg(revertERCSymbol( _pair.first)));
    ui->sellAmountLabel->setText(tr("Amount (%1)").arg(revertERCSymbol( _pair.first)));

    const AssetInfo& assetInfo1 = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(_pair.first));
    const AssetInfo& assetInfo2 = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(_pair.second));
    const ExchangeBalance& assetAmount1 = assetExchangeBalanceMap.value(_pair.first);
    const ExchangeBalance& assetAmount2 = assetExchangeBalanceMap.value(_pair.second);
qDebug() << "aaaaaaaaaaaaaaa " << assetExchangeBalanceMap.keys();
    ui->availableLabel1->setText( QString("%1 %2").arg(getBigNumberString(assetAmount2.available, assetInfo2.precision)).arg(revertERCSymbol( assetInfo2.symbol)));
    ui->availableLabel2->setText( QString("%1 %2").arg(getBigNumberString(assetAmount1.available, assetInfo1.precision)).arg(revertERCSymbol( assetInfo1.symbol)));

    ui->ableToBuyLabel->setText(assetInfo1.symbol);
    ui->ableToSellLabel->setText(assetInfo2.symbol);

    getSellOrders(_pair);
}



void ExchangeModeWidget::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    assetExchangeBalanceMap.clear();
    getUserBalances();
}

void ExchangeModeWidget::getUserBalances()
{
    AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    HXChain::getInstance()->postRPC( "ExchangeModeWidget+invoke_contract_offline+getUserBalances+" + ui->accountComboBox->currentText(),
                                     toJsonFormat( "invoke_contract_offline",
                                     QJsonArray() << ui->accountComboBox->currentText() << EXCHANGE_MODE_CONTRACT_ADDRESS
                                                   << "getUserBalances"  << accountInfo.address));
}

void ExchangeModeWidget::getSellOrders(const ExchangePair &_pair)
{
    QString params = QString("%1,%2,%3,%4").arg(_pair.first).arg(_pair.second).arg(20).arg(0);
    HXChain::getInstance()->postRPC( "ExchangeModeWidget+invoke_contract_offline+getSellOrders+" + _pair.first + "+" + _pair.second,
                                     toJsonFormat( "invoke_contract_offline",
                                     QJsonArray() << ui->accountComboBox->currentText() << EXCHANGE_MODE_CONTRACT_ADDRESS
                                                   << "getSellOrders"  << params));

}

void ExchangeModeWidget::getBuyOrders(const ExchangePair &_pair)
{
    QString params = QString("%1,%2,%3,%4").arg(_pair.first).arg(_pair.second).arg(20).arg(0);
    HXChain::getInstance()->postRPC( "ExchangeModeWidget+invoke_contract_offline+getBuyOrders+" + _pair.first + "+" + _pair.second,
                                     toJsonFormat( "invoke_contract_offline",
                                     QJsonArray() << ui->accountComboBox->currentText() << EXCHANGE_MODE_CONTRACT_ADDRESS
                                                   << "getBuyOrders"  << params));
}
