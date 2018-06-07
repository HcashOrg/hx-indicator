#include "AssetIconItem.h"
#include "ui_AssetIconItem.h"

#include "wallet.h"

//QMap<QString,QString> AssetIconItem::assetColorMap.insert("BTC","#F5A623");
QMap<QString,QString> AssetIconItem::assetColorMap = {{"BTC","#F5A623"}, {"LTC","#4A4A4A"}, {"UB","#38BDE9"}};

AssetIconItem::AssetIconItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssetIconItem)
{
    ui->setupUi(this);
}

AssetIconItem::~AssetIconItem()
{
    delete ui;
}

void AssetIconItem::setAsset(QString assetSymbol)
{
    QString assetColor = assetColorMap.value(assetSymbol);
    ui->barLabel->setStyleSheet(QString("border-top-right-radius:3px;border-bottom-right-radius:3px;"
                                        "background-color: %1;").arg(assetColor));

    ui->iconLabel->setStyleSheet(QString("border-image: url(:/ui/wallet_ui/coin_%1.png);").arg(assetSymbol));
    ui->assetLabel->setText(assetSymbol);
}

void AssetIconItem::setBackgroundColor(QString color)
{
    setStyleSheet(QString("#widget{background-color:%1;}").arg(color));
}
