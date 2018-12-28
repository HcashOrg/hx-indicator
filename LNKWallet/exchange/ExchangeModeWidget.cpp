#include "ExchangeModeWidget.h"
#include "ui_ExchangeModeWidget.h"

#include "ExchangePairSelectDialog.h"

ExchangeModeWidget::ExchangeModeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangeModeWidget)
{
    ui->setupUi(this);

    ui->widget->setObjectName("framewidget");
    ui->widget->setStyleSheet("#framewidget{background-color:white;border:1px solid white;border-radius:15px;}");

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
}


