#include "myexchangecontractpage.h"
#include "ui_myexchangecontractpage.h"

#include <QtMath>

#include "wallet.h"
#include "commondialog.h"
#include "selldialog.h"
#include "depositexchangecontractdialog.h"
#include "withdrawexchangecontractdialog.h"

#define EXCHANGE_CONTRACT_HASH  "1897a3046537f0610851192a9bf1853a9b1732b1"

MyExchangeContractPage::MyExchangeContractPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyExchangeContractPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));


    init();
}

MyExchangeContractPage::~MyExchangeContractPage()
{
    delete ui;
}

void MyExchangeContractPage::init()
{
    ui->stackedWidget->setCurrentIndex(1);

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
        ui->ordersTableWidget->setItem(i,3, new QTableWidgetItem(tr("withdraw")));
        ui->ordersTableWidget->setItem(i,4, new QTableWidgetItem(tr("add")));
    }

}

void MyExchangeContractPage::showContractBalances()
{
    ExchangeContractBalances balances = UBChain::getInstance()->accountExchangeContractBalancesMap.value(ui->accountComboBox->currentText());

    QStringList keys = balances.keys();
    int size = keys.size();
    ui->balancesTableWidget->setRowCount(0);
    ui->balancesTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        QString key = keys.at(i);
        AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(key));

        ui->balancesTableWidget->setItem(i,0, new QTableWidgetItem(key));
        ui->balancesTableWidget->setItem(i,1, new QTableWidgetItem(getBigNumberString(balances.value(key), assetInfo.precision)));
        ui->balancesTableWidget->setItem(i,2, new QTableWidgetItem(tr("deposit")));
        ui->balancesTableWidget->setItem(i,3, new QTableWidgetItem(tr("withdraw")));
    }
}


void MyExchangeContractPage::jsonDataUpdated(QString id)
{
    if( id == "id-register_contract")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Create exchange contract successfully! Please wait for the confirmation of the block chain.") );
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
        qDebug() << id << result;

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

        showContractBalances();

        return;
    }

    if( id == "id-invoke_contract-cancelSellOrder" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Transaction of withdraw-all has been sent out!");
            commonDialog.pop();
        }

        return;
    }

    if( id == "id-invoke_contract-cancelSellOrderPair" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Transaction of withdraw-order has been sent out!");
            commonDialog.pop();
        }

        return;
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
            UBChain::getInstance()->postRPC( "id-register_contract", toJsonFormat( "register_contract",
                                                                                   QJsonArray() << ui->accountComboBox->currentText() << "0.001"
                                                                                   << "10000"  << filePath));
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


        UBChain::getInstance()->postRPC( "id-invoke_contract_offline-owner_assets-" + ui->accountComboBox->currentText(), toJsonFormat( "invoke_contract_offline",
                                                                               QJsonArray() << ui->accountComboBox->currentText() << contractAddress
                                                                               << "owner_assets"  << ""));
    }

}

void MyExchangeContractPage::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    showOrders();
}

void MyExchangeContractPage::on_assetComboBox2_currentIndexChanged(const QString &arg1)
{
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
    ui->stackedWidget->setCurrentIndex(2);
}

void MyExchangeContractPage::on_myOrdersBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MyExchangeContractPage::on_balancesTableWidget_cellPressed(int row, int column)
{
    if(column == 2)
    {
        // 合约充值
        DepositExchangeContractDialog depositExchangeContractDialog;
        depositExchangeContractDialog.setCurrentAsset(ui->balancesTableWidget->item(row,0)->text());
        depositExchangeContractDialog.pop();

        return;
    }

    if(column == 3)
    {
        // 合约提现
        WithdrawExchangeContractDialog withdrawExchangeContractDialog;
        withdrawExchangeContractDialog.setCurrentAsset(ui->balancesTableWidget->item(row,0)->text());
        withdrawExchangeContractDialog.pop();

        return;
    }
}

void MyExchangeContractPage::on_ordersTableWidget_cellPressed(int row, int column)
{
    if(column == 3)
    {
        // 撤销挂单
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText( tr("Sure to withdraw this order?") );
        if(commonDialog.pop())
        {
            QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());
            AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(ui->assetComboBox->currentData().toString());
            AssetInfo assetInfo2 = UBChain::getInstance()->assetInfoMap.value(ui->assetComboBox2->currentData().toString());

            QString params = QString("%1,%2,%3,%4").arg(assetInfo.symbol).arg(decimalToIntegerStr(ui->ordersTableWidget->item(row,0)->text(), assetInfo.precision))
                    .arg(assetInfo2.symbol).arg(decimalToIntegerStr(ui->ordersTableWidget->item(row,1)->text(), assetInfo2.precision));

            UBChain::getInstance()->postRPC( "id-invoke_contract-cancelSellOrder", toJsonFormat( "invoke_contract",
                                                                                   QJsonArray() << ui->accountComboBox->currentText() << 0.001 << 1000
                                                                                   << contractAddress
                                                                                   << "cancelSellOrder"  << params));


        }

        return;
    }
}

void MyExchangeContractPage::on_withdrawAllBtn_clicked()
{
    QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());
    QString params = QString("%1,%2").arg(ui->assetComboBox->currentText()).arg(ui->assetComboBox2->currentText());

    UBChain::getInstance()->postRPC( "id-invoke_contract-cancelSellOrderPair", toJsonFormat( "invoke_contract",
                                                                           QJsonArray() << ui->accountComboBox->currentText() << 0.001 << 1000
                                                                           << contractAddress
                                                                           << "cancelSellOrderPair"  << params));

}

void MyExchangeContractPage::on_depositBtn_clicked()
{
    QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText());

    if(contractAddress.isEmpty())
    {
        registerContract();
    }
    else
    {
        DepositExchangeContractDialog depositExchangeContractDialog;
        depositExchangeContractDialog.pop();
    }
}
