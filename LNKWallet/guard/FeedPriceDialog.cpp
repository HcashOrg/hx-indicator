#include "FeedPriceDialog.h"
#include "ui_FeedPriceDialog.h"

#include "wallet.h"

FeedPriceDialog::FeedPriceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FeedPriceDialog)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CANCELBTN_STYLE);
}

FeedPriceDialog::~FeedPriceDialog()
{
    delete ui;
}

void FeedPriceDialog::pop()
{
    move(0,0);
    exec();
}

void FeedPriceDialog::setAsset(QString _assetSymbol)
{
    ui->assetLabel->setText(_assetSymbol);
}

void FeedPriceDialog::jsonDataUpdated(QString id)
{
    if( id == "id-publish_normal_asset_feed")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        return;
    }
}

void FeedPriceDialog::on_okBtn_clicked()
{
    QJsonObject baseObject;
    baseObject.insert("amount", 1);
    baseObject.insert("asset_id", UBChain::getInstance()->getAssetId(ui->assetLabel->text()));
    QJsonObject quoteObject;
    quoteObject.insert("amount", ui->quoteLineEdit->text().toInt());
    quoteObject.insert("asset_id", "1.3.0");
    QJsonObject settlementPriceObject;
    settlementPriceObject.insert("base", baseObject);
    settlementPriceObject.insert("quote", quoteObject);
    QJsonObject object;
    object.insert("settlement_price", settlementPriceObject);

//    object.insert("core_exchange_rate", settlementPriceObject);

    UBChain::getInstance()->postRPC( "id-publish_normal_asset_feed", toJsonFormat( "publish_normal_asset_feed",
                            QJsonArray() << "guard6" << ui->assetLabel->text() << object << true));
qDebug() << toJsonFormat( "publish_normal_asset_feed",
                          QJsonArray() << "guard6" << ui->assetLabel->text() << object << true).simplified().remove(" ");
}

void FeedPriceDialog::on_closeBtn_clicked()
{
    close();
}
