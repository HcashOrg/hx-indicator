#include "exchangewidget.h"
#include "ui_exchangewidget.h"

#include "wallet.h"

ExchangeWidget::ExchangeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangeWidget)
{
    ui->setupUi(this);
}

ExchangeWidget::~ExchangeWidget()
{
    delete ui;
}

void ExchangeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}

void ExchangeWidget::on_backBtn_clicked()
{
    close();
}
