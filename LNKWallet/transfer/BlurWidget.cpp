#include "BlurWidget.h"
#include "ui_BlurWidget.h"

#include <QPainter>

BlurWidget::BlurWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlurWidget)
{
    ui->setupUi(this);
    setWindowFlags(  Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

}

BlurWidget::~BlurWidget()
{
    delete ui;
}

void BlurWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(10,10,10,100));//最后一位是设置透明属性（在0-255取值）
    painter.drawRect(rect());

    QWidget::paintEvent(event);

}
