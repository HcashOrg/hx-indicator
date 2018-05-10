#include "BuyOrderWidget.h"
#include "ui_BuyOrderWidget.h"

#include "wallet.h"

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
    if("id-transfer_to_contract")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

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

void BuyOrderWidget::on_okBtn_clicked()
{
    if(accountName.isEmpty() || ui->contractAddressLabel->text().isEmpty() || ui->amountLineEdit->text().isEmpty() || payAsset.isEmpty())   return;

    UBChain::getInstance()->postRPC( "id-transfer_to_contract", toJsonFormat( "transfer_to_contract",
                                                                           QJsonArray() << accountName << ui->contractAddressLabel->text()
                                                                           << ui->amountLineEdit->text() << payAsset
                                                                           << "deposit to exchange contract"
                                                                           << UBChain::getInstance()->currentContractFee() << 1000
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
