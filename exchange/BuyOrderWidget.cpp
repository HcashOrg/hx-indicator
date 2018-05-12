#include "BuyOrderWidget.h"
#include "ui_BuyOrderWidget.h"

#include "wallet.h"
#include "commondialog.h"

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
}

void BuyOrderWidget::setPrice(QString _price, QString _assetSymbol, QString _assetSymbol2)
{
    ui->priceLabel->setText(_price + " " + tr("%1/%2").arg(_assetSymbol).arg(_assetSymbol2));
    payAsset = _assetSymbol2;

    ui->amountLineEdit->setPlaceholderText(tr("Max: %1 %2").arg(UBChain::getInstance()->getAccountBalance(accountName,payAsset))
                                           .arg(payAsset) );
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
            stepCount = result.mid(QString("\"result\":").size()).toInt();
            ui->feeLabel->setText(getBigNumberString(stepCount * UBChain::getInstance()->contractFee, ASSET_PRECISION)
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
    if(accountName.isEmpty() || ui->contractAddressLabel->text().isEmpty() || ui->amountLineEdit->text().isEmpty() || payAsset.isEmpty())   return;

    UBChain::getInstance()->postRPC( "id-transfer_to_contract_testing-BuyOrderWidget", toJsonFormat( "transfer_to_contract_testing",
                                                                           QJsonArray() << accountName << ui->contractAddressLabel->text()
                                                                           << ui->amountLineEdit->text() << payAsset
                                                                           << "deposit to exchange contract"
                                                                           ));
}

void BuyOrderWidget::on_okBtn_clicked()
{
    if(accountName.isEmpty() || ui->contractAddressLabel->text().isEmpty() || ui->amountLineEdit->text().isEmpty() || payAsset.isEmpty())   return;

    UBChain::getInstance()->postRPC( "id-transfer_to_contract-BuyOrderWidget", toJsonFormat( "transfer_to_contract",
                                                                           QJsonArray() << accountName << ui->contractAddressLabel->text()
                                                                           << ui->amountLineEdit->text() << payAsset
                                                                           << "deposit to exchange contract"
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
    ui->amountLineEdit->setText(UBChain::getInstance()->getAccountBalance(accountName,payAsset));
}

void BuyOrderWidget::on_amountLineEdit_textChanged(const QString &arg1)
{
    estimateContractFee();
}
