#include "ExchangeSinglePairCellWidget.h"
#include "ui_ExchangeSinglePairCellWidget.h"


ExchangeSinglePairCellWidget::ExchangeSinglePairCellWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangeSinglePairCellWidget)
{
    ui->setupUi(this);

    ui->checkLabel->setStyleSheet("border-image: url(:/ui/wallet_ui/checkMark.png);");

    setStyleSheet("ExchangeSinglePairCellWidget{background:white;}"
                  "ExchangeSinglePairCellWidget:hover{background:rgb(243,241,252);}");
}

ExchangeSinglePairCellWidget::~ExchangeSinglePairCellWidget()
{
    delete ui;
}

void ExchangeSinglePairCellWidget::setPair(const ExchangePair &_pair)
{
    pair = _pair;
    ui->iconLabel->setStyleSheet(QString("border-image: url(:/ui/wallet_ui/coin_%1.png);").arg(getRealAssetSymbol( _pair.first)));
    ui->pairLabel->setText(QString("%1/%2").arg(_pair.first).arg(pair.second));

    ui->checkLabel->setVisible(HXChain::getInstance()->currentExchangePair == _pair);
}

void ExchangeSinglePairCellWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
