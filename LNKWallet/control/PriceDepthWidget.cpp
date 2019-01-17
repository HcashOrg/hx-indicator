#include "PriceDepthWidget.h"
#include "ui_PriceDepthWidget.h"

#include "wallet.h"

PriceDepthWidget::PriceDepthWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PriceDepthWidget)
{
    ui->setupUi(this);
}

PriceDepthWidget::~PriceDepthWidget()
{
    delete ui;
}

void PriceDepthWidget::setLength(double _length)
{
    length = _length;
}

void PriceDepthWidget::setType(int _type)
{
    type = _type;
}

void PriceDepthWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    if(type == 0)
    {
        painter.setBrush(QColor(44,202,148));
    }
    else
    {
        painter.setBrush(QColor(235,0,94));
    }
    painter.drawRect(0,3, (length * this->width() > 1 ? length * this->width() : 1) , 16);

    QWidget::paintEvent(event);
}
