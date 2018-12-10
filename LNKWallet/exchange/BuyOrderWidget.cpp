#include "BuyOrderWidget.h"
#include "ui_BuyOrderWidget.h"

#include <QtMath>

#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"

#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"

BuyOrderWidget::BuyOrderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BuyOrderWidget)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->allBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);

    ui->widget->setObjectName("framewidget");
    ui->widget->setStyleSheet("#framewidget{background-color:white;border:1px solid white;border-radius:15px;}");

    feeChoose = new FeeChooseWidget(0,HXChain::getInstance()->feeType);
    ui->stackedWidget->addWidget(feeChoose);
    feeChoose->resize(ui->stackedWidget->size());
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
    ui->okBtn->setEnabled(false);
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

    AssetInfo buyAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(getRealAssetSymbol( buySymbol)));
    AssetInfo sellAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(getRealAssetSymbol( sellSymbol)));

    double price = (double)buyAmount / qPow(10,buyAssetInfo.precision) / sellAmount * qPow(10,sellAssetInfo.precision);

    ui->priceLabel->setText(QString::number(price,'g',8) + " " + tr("%1/%2").arg(revertERCSymbol( buySymbol)).arg(revertERCSymbol( sellSymbol)));

    ui->amountLineEdit->setPlaceholderText(tr("Max: %1 %2").arg(HXChain::getInstance()->getAccountBalance(accountName, revertERCSymbol( sellSymbol))).arg(revertERCSymbol( sellSymbol)));

    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(sellAssetInfo.precision));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
}

void BuyOrderWidget::setContractAddress(QString _contractAddress)
{
    ui->contractAddressLabel->setText(_contractAddress);
}

void BuyOrderWidget::jsonDataUpdated(QString id)
{
    if( id == "id-transfer_to_contract-BuyOrderWidget")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            close();

            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of buy-order has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Transfer to exchange contract failed!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        return;
    }

    if( id == "id-transfer_to_contract_testing-BuyOrderWidget")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            ui->okBtn->setEnabled(true);
            HXChain::TotalContractFee totalFee = HXChain::getInstance()->parseTotalContractFee(result);
            stepCount = totalFee.step;
            unsigned long long totalAmount = totalFee.baseAmount + ceil(totalFee.step * HXChain::getInstance()->contractFee / 100.0);
            feeChoose->updateFeeNumberSlots(getBigNumberString(totalAmount, ASSET_PRECISION).toDouble());

            ui->feeLabel->setText(getBigNumberString(totalAmount,ASSET_PRECISION)
                                          + " " + ASSET_NAME);
        }
        else
        {
            ui->okBtn->setEnabled(false);
        }

        return;
    }
}

void BuyOrderWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void BuyOrderWidget::estimateContractFee()
{
    if(accountName.isEmpty() || ui->contractAddressLabel->text().isEmpty() || ui->amountLineEdit->text().isEmpty())
    {
        ui->okBtn->setEnabled(false);
        return;
    }


    // 计算实际能买到的数量
    AssetInfo buyAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(getRealAssetSymbol( buySymbol)));
    AssetInfo sellAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(getRealAssetSymbol( sellSymbol)));
    double amount = (double)buyAmount / qPow(10,buyAssetInfo.precision) / sellAmount * qPow(10,sellAssetInfo.precision)
            * ui->amountLineEdit->text().toDouble();


    HXChain::getInstance()->postRPC( "id-transfer_to_contract_testing-BuyOrderWidget", toJsonFormat( "transfer_to_contract_testing",
                                                                           QJsonArray() << accountName << ui->contractAddressLabel->text()
                                                                           << ui->amountLineEdit->text() << getRealAssetSymbol( sellSymbol)
                                                                           << QString("%1,%2").arg( getRealAssetSymbol( buySymbol)).arg(decimalToIntegerStr(QString::number(amount,'g',8),buyAssetInfo.precision))
                                                                           ));

    qDebug() << toJsonFormat( "transfer_to_contract_testing",
                              QJsonArray() << accountName << ui->contractAddressLabel->text()
                              << ui->amountLineEdit->text() << getRealAssetSymbol( sellSymbol)
                              << QString("%1,%2").arg( getRealAssetSymbol( buySymbol)).arg(decimalToIntegerStr(QString::number(amount,'g',8),buyAssetInfo.precision))
                              );
}

void BuyOrderWidget::on_okBtn_clicked()
{
    if(accountName.isEmpty() || ui->contractAddressLabel->text().isEmpty() || ui->amountLineEdit->text().isEmpty() )   return;

    // 计算实际能买到的数量
    AssetInfo buyAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(getRealAssetSymbol( buySymbol)));
    AssetInfo sellAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(getRealAssetSymbol( sellSymbol)));
    double amount = (double)buyAmount / qPow(10,buyAssetInfo.precision) / sellAmount * qPow(10,sellAssetInfo.precision)
            * ui->amountLineEdit->text().toDouble();
    feeChoose->updatePoundageID();
    HXChain::getInstance()->postRPC( "id-transfer_to_contract-BuyOrderWidget", toJsonFormat( "transfer_to_contract",
                                                                           QJsonArray() << accountName << ui->contractAddressLabel->text()
                                                                           << ui->amountLineEdit->text() << getRealAssetSymbol( sellSymbol)
                                                                           << QString("%1,%2").arg(getRealAssetSymbol( buySymbol)).arg(decimalToIntegerStr(QString::number(amount,'g',8),buyAssetInfo.precision))
                                                                           << HXChain::getInstance()->currentContractFee() << stepCount
                                                                           << true
                                                                           ));

}

void BuyOrderWidget::on_cancelBtn_clicked()
{
    close();
}

void BuyOrderWidget::on_allBtn_clicked()
{
    ui->amountLineEdit->setText(HXChain::getInstance()->getAccountBalance(accountName,getRealAssetSymbol( sellSymbol)));
}

void BuyOrderWidget::on_amountLineEdit_textChanged(const QString &arg1)
{
    estimateContractFee();
}

void BuyOrderWidget::on_amountLineEdit_textEdited(const QString &arg1)
{
    if(ui->amountLineEdit->text().toDouble() > HXChain::getInstance()->getAccountBalance(accountName,getRealAssetSymbol( sellSymbol)).toDouble())
    {
        ui->amountLineEdit->setText(HXChain::getInstance()->getAccountBalance(accountName,getRealAssetSymbol( sellSymbol)));
    }
}
