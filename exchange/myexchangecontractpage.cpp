#include "myexchangecontractpage.h"
#include "ui_myexchangecontractpage.h"

#include <QtMath>

#include "wallet.h"
#include "commondialog.h"
#include "selldialog.h"
#include "contractbalancewidget.h"
#include "ToolButtonWidget.h"
#include "WithdrawOrderDialog.h"
#include "depositpage/FeeChargeWidget.h"

MyExchangeContractPage::MyExchangeContractPage(QWidget *parent) :
    QWidget(parent),
    currentWidget(NULL),
    ui(new Ui::MyExchangeContractPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->ordersTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->ordersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ordersTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->ordersTableWidget->setFrameShape(QFrame::NoFrame);
    ui->ordersTableWidget->setMouseTracking(true);
    ui->ordersTableWidget->setShowGrid(false);//隐藏表格线
    ui->ordersTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->ordersTableWidget->horizontalHeader()->setFixedHeight(35);
    ui->ordersTableWidget->horizontalHeader()->setVisible(true);
    ui->ordersTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->ordersTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->ordersTableWidget->verticalHeader()->setVisible(false);
    ui->ordersTableWidget->setColumnWidth(0,160);
    ui->ordersTableWidget->setColumnWidth(1,160);
    ui->ordersTableWidget->setColumnWidth(2,160);
    ui->ordersTableWidget->setColumnWidth(3,90);
    ui->ordersTableWidget->setColumnWidth(4,90);

    ui->ordersTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->balanceBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->sellBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->withdrawAllBtn->setStyleSheet(TOOLBUTTON_STYLE_1);


    init();
}

MyExchangeContractPage::~MyExchangeContractPage()
{
    delete ui;
}

void MyExchangeContractPage::init()
{
    QStringList accounts = UBChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    QStringList assetIds = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
        ui->assetComboBox2->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
    }

    inited = true;

    on_accountComboBox_currentIndexChanged(ui->accountComboBox->currentText());
}

void MyExchangeContractPage::refresh()
{
    on_accountComboBox_currentIndexChanged(ui->accountComboBox->currentText());

    if( currentWidget != NULL && currentWidget->objectName() == "contractBalanceWidget")
    {
        static_cast<ContractBalanceWidget*>(currentWidget)->refresh();
    }
}

void MyExchangeContractPage::onBack()
{
    if(currentWidget)
    {
        currentWidget->close();
        currentWidget = NULL;
    }
}

void MyExchangeContractPage::showOrders()
{
    SellOrders orders = accountSellOrdersMap.value(ui->accountComboBox->currentText());
    QVector<OrderAmount> vector = orders.value(ui->assetComboBox->currentText() + "," + ui->assetComboBox2->currentText());

    AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(ui->assetComboBox->currentData().toString());
    AssetInfo assetInfo2 = UBChain::getInstance()->assetInfoMap.value(ui->assetComboBox2->currentData().toString());

    int size = vector.size();
    ui->ordersTableWidget->setRowCount(0);
    ui->ordersTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        OrderAmount amount = vector.at(i);

        ui->ordersTableWidget->setItem(i,0, new QTableWidgetItem(getBigNumberString(amount.sellAmount, assetInfo.precision)));
        ui->ordersTableWidget->setItem(i,1, new QTableWidgetItem(getBigNumberString(amount.buyAmount, assetInfo2.precision)));

        double price = ((double)amount.sellAmount / qPow(10,assetInfo.precision)) / (amount.buyAmount / qPow(10,assetInfo2.precision) );
        ui->ordersTableWidget->setItem(i,2, new QTableWidgetItem(QString::number(price,'g',12)));
        ui->ordersTableWidget->setItem(i,3, new QTableWidgetItem(tr("cancel")));
        ui->ordersTableWidget->setItem(i,4, new QTableWidgetItem(tr("add")));

        for(int j = 0; j < 5; j++)
        {
            ui->ordersTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
            if(i%2)
            {
                ui->ordersTableWidget->item(i,j)->setBackgroundColor(QColor(255,255,255));
            }
            else
            {
                ui->ordersTableWidget->item(i,j)->setBackgroundColor(QColor(252,253,255));
            }
        }

        for(int j = 3;j < 5;++j)
        {
            ToolButtonWidgetItem *toolButtonItem = new ToolButtonWidgetItem(i,j);
            toolButtonItem->setInitGray(j%2 != 0);
            toolButtonItem->setText(ui->ordersTableWidget->item(i,j)->text());
            ui->ordersTableWidget->setCellWidget(i,j,toolButtonItem);
            connect(toolButtonItem,SIGNAL(itemClicked(int,int)),this,SLOT(onItemClicked(int,int)));
        }
    }

}

void MyExchangeContractPage::updateTableHeaders()
{
    if(ui->assetComboBox->currentText() == ui->assetComboBox2->currentText())
    {
        ui->ordersTableWidget->horizontalHeaderItem(0)->setText(tr("Sell"));
        ui->ordersTableWidget->horizontalHeaderItem(1)->setText(tr("Buy"));
        ui->ordersTableWidget->horizontalHeaderItem(2)->setText(tr("Price"));
    }
    else
    {
        ui->ordersTableWidget->horizontalHeaderItem(0)->setText(tr("Sell / %1").arg(ui->assetComboBox->currentText()));
        ui->ordersTableWidget->horizontalHeaderItem(1)->setText(tr("Buy / %1").arg(ui->assetComboBox2->currentText()));
        ui->ordersTableWidget->horizontalHeaderItem(2)->setText(tr("Price (%1/%2)").arg(ui->assetComboBox->currentText()).arg(ui->assetComboBox2->currentText()));
    }
}

void MyExchangeContractPage::fetchAccountExchangeContractBalances()
{
    QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());

    if(!contractAddress.isEmpty())
    {
        UBChain::getInstance()->postRPC( "id-invoke_contract_offline-owner_assets-" + ui->accountComboBox->currentText(), toJsonFormat( "invoke_contract_offline",
                                                                               QJsonArray() << ui->accountComboBox->currentText() << contractAddress
                                                                               << "owner_assets"  << ""));
    }
}


void MyExchangeContractPage::jsonDataUpdated(QString id)
{
    if( id.startsWith("id-invoke_contract_offline-owner_assets-") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        QString accountName = id.mid(QString("id-invoke_contract_offline-owner_assets-").size());

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonObject object = QJsonDocument::fromJson(jsonObject.take("result").toString().toLatin1()).object();

            ExchangeContractBalances balances;
            foreach (QString key, object.keys())
            {
                QJsonValue amountValue = object.take(key);
                unsigned long long amount = 0;

                if(amountValue.isString())
                {
                    amount = amountValue.toString().toULongLong();
                }
                else
                {
                    amount = QString::number(amountValue.toDouble(),'g',12).toULongLong();
                }

                balances.insert(key,amount);
            }

            UBChain::getInstance()->accountExchangeContractBalancesMap.insert(accountName,balances);
        }

        return;
    }

    if( id == "id-register_contract")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Create exchange contract successfully! Please wait for the confirmation of the block chain. Please do not repeat the creation of the contract.") );
            commonDialog.pop();

        }
        else if(result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Create exchange contract failed: " + errorMessage );
            commonDialog.pop();
        }

        return;
    }


    if( id.startsWith("id-invoke_contract_offline-sell_orders-"))
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        QString accountName = id.mid(QString("id-invoke_contract_offline-sell_orders-").size());

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonObject object = QJsonDocument::fromJson(jsonObject.take("result").toString().toLatin1()).object();

            SellOrders orders;
            foreach (QString key, object.keys())
            {
                QJsonObject object2 = QJsonDocument::fromJson(object.take(key).toString().toLatin1()).object();
                QJsonArray orderArray = object2.take("orderArray").toArray();

                QVector<OrderAmount> vector;
                foreach (QJsonValue v, orderArray)
                {
                    QString amountStr = v.toString();
                    OrderAmount amount;
                    amount.sellAmount = amountStr.split(",").at(0).toULongLong();
                    amount.buyAmount = amountStr.split(",").at(1).toULongLong();
                    vector.append(amount);
                }

                orders.insert(key,vector);
            }

            accountSellOrdersMap.insert(accountName,orders);
        }

        showOrders();

        return;
    }

    if( id.startsWith("id-invoke_contract_testing-cancelSellOrder-") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":"))
        {
            UBChain::TotalContractFee totalFee = UBChain::getInstance()->parseTotalContractFee(result);
            int stepCount = totalFee.step;
            unsigned long long totalAmount = totalFee.baseAmount + totalFee.step * UBChain::getInstance()->contractFee;

            WithdrawOrderDialog withdrawOrderDialog;
            withdrawOrderDialog.setContractFee(getBigNumberString(totalAmount, ASSET_PRECISION).toDouble());
            withdrawOrderDialog.setContractFee(getBigNumberString(totalAmount, ASSET_PRECISION)
                                               + " " + ASSET_NAME);
            withdrawOrderDialog.setAccountName(ui->accountComboBox->currentText());
            withdrawOrderDialog.setText(tr("Sure to cancel this order? You need to pay the fee for contract execution."));
            if(withdrawOrderDialog.pop())
            {
                QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());

                QString params = id.mid(QString("id-invoke_contract_testing-cancelSellOrder-").size());
                withdrawOrderDialog.updatePoundageID();
                UBChain::getInstance()->postRPC( "id-invoke_contract-cancelSellOrder", toJsonFormat( "invoke_contract",
                                                                                       QJsonArray() << ui->accountComboBox->currentText()
                                                                                       << UBChain::getInstance()->currentContractFee() << stepCount
                                                                                       << contractAddress
                                                                                       << "cancelSellOrder"  << params));
            }



        }

        return;
    }

    if( id == "id-invoke_contract-cancelSellOrder" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Transaction of withdraw-order has been sent out!");
            commonDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Create exchange contract failed: " + errorMessage );
            commonDialog.pop();
        }


        return;
    }

    if( id.startsWith("id-invoke_contract_testing-cancelSellOrderPair-") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":"))
        {
            UBChain::TotalContractFee totalFee = UBChain::getInstance()->parseTotalContractFee(result);
            int stepCount = totalFee.step;
            unsigned long long totalAmount = totalFee.baseAmount + totalFee.step * UBChain::getInstance()->contractFee;

            WithdrawOrderDialog withdrawOrderDialog;
            withdrawOrderDialog.setContractFee(getBigNumberString(totalAmount, ASSET_PRECISION)
                                               + " " + ASSET_NAME);
            withdrawOrderDialog.setContractFee(getBigNumberString(totalAmount, ASSET_PRECISION).toDouble());
            withdrawOrderDialog.setAccountName(ui->accountComboBox->currentText());
            withdrawOrderDialog.setText(tr("You need to pay the fee for contract execution."));
            qDebug()<<totalAmount;
            if(withdrawOrderDialog.pop())
            {
                QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());
                QString params = id.mid(QString("id-invoke_contract_testing-cancelSellOrderPair-").size());

                withdrawOrderDialog.updatePoundageID();
                UBChain::getInstance()->postRPC( "id-invoke_contract-cancelSellOrderPair", toJsonFormat( "invoke_contract",
                                                                                                         QJsonArray() << ui->accountComboBox->currentText()
                                                                                                         << UBChain::getInstance()->currentContractFee() << stepCount
                                                                                                         << contractAddress
                                                                                                         << "cancelSellOrderPair"  << params));


            }

        }

        return;
    }

    if( id == "id-invoke_contract-cancelSellOrderPair" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Transaction of withdraw-order-pair has been sent out!");
            commonDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Create exchange contract failed: " + errorMessage );
            commonDialog.pop();
        }

        return;
    }

    if("contract-register_contract_testing" == id)
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            UBChain::TotalContractFee totalFee = UBChain::getInstance()->parseTotalContractFee(result);
            int stepCount = totalFee.step;

            unsigned long long totalAmount = totalFee.baseAmount + totalFee.step * UBChain::getInstance()->contractFee;

            qDebug()<<totalAmount;

            FeeChargeWidget *fee = new FeeChargeWidget( getBigNumberString(totalAmount, ASSET_PRECISION).toDouble(),UBChain::getInstance()->feeType,
                                                         ui->accountComboBox->currentText(),UBChain::getInstance()->mainFrame);

            fee->SetInfo(tr("register contract!"));
            connect(fee,&FeeChargeWidget::confirmSignal,[this,stepCount](){
                QString filePath = QDir::currentPath() + "/contracts/blocklink_exchange.lua.gpc";
                QFileInfo fileInfo(filePath);
                if(fileInfo.exists())
                {
                    UBChain::getInstance()->postRPC( "id-register_contract", toJsonFormat( "register_contract",
                                                                                           QJsonArray() << this->ui->accountComboBox->currentText() << UBChain::getInstance()->currentContractFee()
                                                                                           << stepCount  << filePath));
                }

            });

            fee->show();
        }
        else
        {
            CommonDialog dia(CommonDialog::OkOnly);
            dia.setText(result);
            dia.pop();
        }
    }
}


void MyExchangeContractPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}

void MyExchangeContractPage::registerContract()
{
    // 如果还没有兑换合约  先注册
    CommonDialog commonDialog(CommonDialog::OkAndCancel);
    commonDialog.setText(tr("You don't have an exchange contract at the moment. Will you create it?"));
    if(commonDialog.pop())
    {
        QString filePath = QDir::currentPath() + "/contracts/blocklink_exchange.lua.gpc";
        QFileInfo fileInfo(filePath);
        if(fileInfo.exists())
        {
            //查询注册合约费用
            UBChain::getInstance()->postRPC("contract-register_contract_testing",toJsonFormat( "register_contract_testing",
                                                                                               QJsonArray() << ui->accountComboBox->currentText() << filePath));


//            UBChain::getInstance()->postRPC( "id-register_contract", toJsonFormat( "register_contract",
//                                                                                   QJsonArray() << ui->accountComboBox->currentText() << UBChain::getInstance()->currentContractFee()
//                                                                                   << "10000"  << filePath));
        }
        else
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Can not find file contracts/blocklink_exchange.glua.gpc!"));
            commonDialog.pop();
        }
    }
}

void MyExchangeContractPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;

    UBChain::getInstance()->currentAccount = ui->accountComboBox->currentText();
    QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());

    if(contractAddress.isEmpty())
    {
        ui->ordersTableWidget->setRowCount(0);
    }
    else
    {
        UBChain::getInstance()->postRPC( "id-invoke_contract_offline-sell_orders-" + ui->accountComboBox->currentText(), toJsonFormat( "invoke_contract_offline",
                                                                               QJsonArray() << ui->accountComboBox->currentText() << contractAddress
                                                                               << "sell_orders"  << ""));

        fetchAccountExchangeContractBalances();
    }

}

void MyExchangeContractPage::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    updateTableHeaders();
    if(ui->assetComboBox->currentText() == ui->assetComboBox2->currentText())   return;

    showOrders();
}

void MyExchangeContractPage::on_assetComboBox2_currentIndexChanged(const QString &arg1)
{
    updateTableHeaders();
    if(ui->assetComboBox->currentText() == ui->assetComboBox2->currentText())   return;

    showOrders();
}

void MyExchangeContractPage::on_sellBtn_clicked()
{
    QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());

    if(contractAddress.isEmpty())
    {
        registerContract();
    }
    else
    {
        SellDialog sellDialog;
        sellDialog.setSellAsset(ui->assetComboBox->currentText());
        sellDialog.setBuyAsset(ui->assetComboBox2->currentText());
        sellDialog.pop();
    }


}

void MyExchangeContractPage::on_balanceBtn_clicked()
{
    QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());

    if(contractAddress.isEmpty())
    {
        registerContract();
    }
    else
    {
        ContractBalanceWidget* contractBalanceWidget = new ContractBalanceWidget(this);
        contractBalanceWidget->setAttribute(Qt::WA_DeleteOnClose);
        contractBalanceWidget->setObjectName("contractBalanceWidget");
        contractBalanceWidget->setAccount(ui->accountComboBox->currentText());
        contractBalanceWidget->move(0,0);
        contractBalanceWidget->show();

        currentWidget = contractBalanceWidget;

        emit backBtnVisible(true);
    }
}



void MyExchangeContractPage::on_withdrawAllBtn_clicked()
{
    if(ui->ordersTableWidget->rowCount() < 1)   return;

    CommonDialog commonDialog(CommonDialog::OkAndCancel);
    commonDialog.setText( tr("Sure to cancel all orders of %1-to-%2 ?").arg(ui->assetComboBox->currentText()).arg(ui->assetComboBox2->currentText()) );
    if(commonDialog.pop())
    {
        QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());
        QString params = QString("%1,%2").arg(ui->assetComboBox->currentText()).arg(ui->assetComboBox2->currentText());

        UBChain::getInstance()->postRPC( "id-invoke_contract_testing-cancelSellOrderPair-" + params, toJsonFormat( "invoke_contract_testing",
                                                                                                 QJsonArray() << ui->accountComboBox->currentText()
                                                                                                 << contractAddress
                                                                                                 << "cancelSellOrderPair"  << params));
    }
}

void MyExchangeContractPage::onItemClicked(int _row, int _column)
{
    if(_column == 3)
    {
        // 撤销挂单
        QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());
        AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(ui->assetComboBox->currentData().toString());
        AssetInfo assetInfo2 = UBChain::getInstance()->assetInfoMap.value(ui->assetComboBox2->currentData().toString());

        QString params = QString("%1,%2,%3,%4").arg(assetInfo.symbol).arg(decimalToIntegerStr(ui->ordersTableWidget->item(_row,0)->text(), assetInfo.precision))
                .arg(assetInfo2.symbol).arg(decimalToIntegerStr(ui->ordersTableWidget->item(_row,1)->text(), assetInfo2.precision));


        UBChain::getInstance()->postRPC( "id-invoke_contract_testing-cancelSellOrder-" + params, toJsonFormat( "invoke_contract_testing",
                                                                                                               QJsonArray() << ui->accountComboBox->currentText()
                                                                                                               << contractAddress
                                                                                                               << "cancelSellOrder"  << params));


        return;
    }

    if(_column == 4)
    {
        SellDialog sellDialog;
        sellDialog.setSellAsset(ui->assetComboBox->currentText());
        sellDialog.setBuyAsset(ui->assetComboBox2->currentText());
        sellDialog.pop();

        return;
    }
}
