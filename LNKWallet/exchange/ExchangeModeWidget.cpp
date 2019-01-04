#include "ExchangeModeWidget.h"
#include "ui_ExchangeModeWidget.h"

#include "ExchangePairSelectDialog.h"

ExchangeModeWidget::ExchangeModeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangeModeWidget)
{
    ui->setupUi(this);


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

    AssetAmountMap assetAmountMap = HXChain::getInstance()->accountInfoMap.value(HXChain::getInstance()->currentAccount).assetAmountMap;
    const AssetInfo& assetInfo1 = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(_pair.first));
    const AssetInfo& assetInfo2 = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(_pair.second));
    const AssetAmount& assetAmount1 = assetAmountMap.value(assetInfo1.id);
    const AssetAmount& assetAmount2 = assetAmountMap.value(assetInfo2.id);

    ui->availableLabel1->setText( QString("%1 %2").arg(getBigNumberString(assetAmount2.amount, assetInfo2.precision)).arg(revertERCSymbol( assetInfo2.symbol)));
    ui->availableLabel2->setText( QString("%1 %2").arg(getBigNumberString(assetAmount1.amount, assetInfo1.precision)).arg(revertERCSymbol( assetInfo1.symbol)));

    ui->ableToBuyLabel->setText(assetInfo1.symbol);
    ui->ableToSellLabel->setText(assetInfo2.symbol);
}


