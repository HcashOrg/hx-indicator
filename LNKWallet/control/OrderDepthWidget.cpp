#include "OrderDepthWidget.h"
#include "ui_OrderDepthWidget.h"

#include <QPainter>

OrderDepthWidget::OrderDepthWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderDepthWidget)
{
    ui->setupUi(this);
}

OrderDepthWidget::~OrderDepthWidget()
{
    delete ui;
}

void OrderDepthWidget::setType(bool _sellOrNot)
{
    sellOrNot = _sellOrNot;
    if(sellOrNot)
    {
        ui->numberLabel->setStyleSheet("color: rgb(44,202,148);font: 9px \"微软雅黑\";");
    }
    else
    {
        ui->numberLabel->setStyleSheet("color: rgb(235,0,94);font: 9px \"微软雅黑\";");
    }
}

void OrderDepthWidget::setNumber(int _number)
{
    number = _number;
    QString str = (sellOrNot ? tr("sell ") : tr("buy "));
    ui->numberLabel->setText(str + QString::number(number));
}

void OrderDepthWidget::setPrice(QString _price)
{
    price = _price;
    ui->priceLabel->setText( price);
    ui->priceLabel->setStyleSheet("color: rgb(83,61,138);font: 9px \"微软雅黑\";");
}

void OrderDepthWidget::setAmount(QString _amount)
{
    amount = _amount;
    ui->amountLabel->setText( amount);
    ui->amountLabel->setStyleSheet("color: rgb(83,61,138);font: 9px \"微软雅黑\";");
}

void OrderDepthWidget::setLength(double _length)
{
    length = _length;
    repaint();
}

void OrderDepthWidget::setBaseAmount(unsigned long long _baseAmount)
{
    baseAmount = _baseAmount;
}

void OrderDepthWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    if(sellOrNot)
    {
        painter.setBrush(QColor(224,239,236));
    }
    else
    {
        painter.setBrush(QColor(249,234,234));
    }
    painter.drawRect(this->width(),0, 0 - ((length * this->width() > 1 ? length * this->width() : 1)) , 22);

    QWidget::paintEvent(event);
}
