#include "BuyOrderWidget.h"
#include "ui_BuyOrderWidget.h"

#include <QtMath>

#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"

BuyOrderWidget::BuyOrderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BuyOrderWidget)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->allBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);

    ui->widget->setObjectName("framewidget");
    ui->widget->setStyleSheet("#framewidget{background-color:white;border:1px solid white;border-radius:15px;}");

    feeChoose = new FeeChooseWidget(0,UBChain::getInstance()->feeType);
    ui->stackedWidget->addWidget(feeChoose);
    ui->stackedWidget->setCurrentIndex(0);
    ui->label->setVisible(false);
    ui->feeLabel->setVisible(false);
    init();
}

BuyOrderWidget::~BuyOrderWidget()
{
    delete ui;
}

void BuyOrderWidget::init()
{

}

void BuyOrderWidget::setAccount(QString _accountName)
{
    accountName = _accountName;
    feeChoose->updateAccountNameSlots(_accountName);
}



void BuyOrderWidget::setOrderInfo(unsigned long long _buyAmount, QString _buySymbol, unsigned long long _sellAmount, QString _sellSymbol)
{
    buyAmount = _buyAmount;
    sellAmount = _sellAmount;
    buySymbol = _buySymbol;
    sellSymbol = _sellSymbol;

    AssetInfo buyAssetInfo  = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(buySymbol));
    AssetInfo sellAssetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(sellSymbol));

    double price = (double)buyAmount / qPow(10,buyAssetInfo.precision) / sellAmount * qPow(10,sellAssetInfo.precision);

    ui->priceLabel->setText(QString::number(price,'g',8) + " " + tr("%1/%2").arg(buySymbol).arg(sellSymbol));

    ui->amountLineEdit->setPlaceholderText(tr("Max: %1 %2").arg(UBChain::getInstance()->getAccountBalance(accountName,sellSymbol)).arg(sellSymbol));
}

void BuyOrderWidget::setContractAddress(QString _contractAddress)
{
    ui->contractAddressLabel->setText(_contractAddress);
}

void BuyOrderWidget::jsonDataUpdated(QString id)
{
    if( id == "id-transfer_to_contract-BuyOrderWidget")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            close();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of buy-order has been sent out!"));
            commonDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Transfer to exchange contract failed: " + errorMessage );
            commonDialog.pop();
        }

        return;
    }

    if( id == "id-transfer_to_contract_testing-BuyOrderWidget")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            UBChain::TotalContractFee totalFee = UBChain::getInstance()->parseTotalContractFee(result);
            stepCount = totalFee.step;
            unsigned long long totalAmount = totalFee.baseAmount + ceil(totalFee.step * UBChain::getInstance()->contractFee / 100.0);
            feeChoose->updateFeeNumberSlots(getBigNumberString(totalAmount, ASSET_PRECISION).toDouble());

            ui->feeLabel->setText(getBigNumberString(totalAmount,ASSET_PRECISION)
                                          + " " + ASSET_NAME);
        }

        return;
    }
}

void BuyOrderWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}

void BuyOrderWidget::estimateContractFee()
{
    if(accountName.isEmpty() || ui->contractAddressLabel->text().isEmpty() || ui->amountLineEdit->text().isEmpty())   return;


    // 计算实际能买到的数量
    AssetInfo buyAssetInfo  = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(buySymbol));
    AssetInfo sellAssetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(sellSymbol));
    double amount = (double)buyAmount / qPow(10,buyAssetInfo.precision) / sellAmount * qPow(10,sellAssetInfo.precision)
            * ui->amountLineEdit->text().toDouble();


    UBChain::getInstance()->postRPC( "id-transfer_to_contract_testing-BuyOrderWidget", toJsonFormat( "transfer_to_contract_testing",
                                                                           QJsonArray() << accountName << ui->contractAddressLabel->text()
                                                                           << ui->amountLineEdit->text() << sellSymbol
                                                                           << QString("%1,%2").arg(buySymbol).arg(decimalToIntegerStr(QString::number(amount,'g',8),buyAssetInfo.precision))
                                                                           ));

    qDebug() << toJsonFormat( "transfer_to_contract_testing",
                              QJsonArray() << accountName << ui->contractAddressLabel->text()
                              << ui->amountLineEdit->text() << sellSymbol
                              << QString("%1,%2").arg(buySymbol).arg(decimalToIntegerStr(QString::number(amount,'g',8),buyAssetInfo.precision))
                              );
}

void BuyOrderWidget::on_okBtn_clicked()
{
    if(accountName.isEmpty() || ui->contractAddressLabel->text().isEmpty() || ui->amountLineEdit->text().isEmpty() )   return;

    // 计算实际能买到的数量
    AssetInfo buyAssetInfo  = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(buySymbol));
    AssetInfo sellAssetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(sellSymbol));
    double amount = (double)buyAmount / qPow(10,buyAssetInfo.precision) / sellAmount * qPow(10,sellAssetInfo.precision)
            * ui->amountLineEdit->text().toDouble();
    feeChoose->updatePoundageID();
    UBChain::getInstance()->postRPC( "id-transfer_to_contract-BuyOrderWidget", toJsonFormat( "transfer_to_contract",
                                                                           QJsonArray() << accountName << ui->contractAddressLabel->text()
                                                                           << ui->amountLineEdit->text() << sellSymbol
                                                                           << QString("%1,%2").arg(buySymbol).arg(decimalToIntegerStr(QString::number(amount,'g',8),buyAssetInfo.precision))
                                                                           << UBChain::getInstance()->currentContractFee() << stepCount
                                                                           << true
                                                                           ));

}

void BuyOrderWidget::on_cancelBtn_clicked()
{
    close();
}

void BuyOrderWidget::on_allBtn_clicked()
{
    ui->amountLineEdit->setText(UBChain::getInstance()->getAccountBalance(accountName,sellSymbol));
}

void BuyOrderWidget::on_amountLineEdit_textChanged(const QString &arg1)
{
    estimateContractFee();
}
