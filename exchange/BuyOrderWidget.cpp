#include "BuyOrderWidget.h"
#include "ui_BuyOrderWidget.h"

#include "wallet.h"

BuyOrderWidget::BuyOrderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BuyOrderWidget)
{
    ui->setupUi(this);

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

void BuyOrderWidget::setSellAsset(QString _assetSymbol)
{
}

void BuyOrderWidget::setBuyAsset(QString _assetSymbol)
{
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
//    QString contractAddress = u

//    UBChain::getInstance()->postRPC( "id-transfer_to_contract", toJsonFormat( "transfer_to_contract",
//                                                                           QJsonArray() << ui->accountNameLabel->text() << contractAddress
//                                                                           << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
//                                                                           << "deposit to exchange contract" << "0.001" << 1000 << true
//                                                                           ));

}

void BuyOrderWidget::on_cancelBtn_clicked()
{
    close();
}
