#include "HelpWidget.h"
#include "ui_HelpWidget.h"

#include <QPainter>
HelpWidget::HelpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpWidget)
{
    ui->setupUi(this);
    InitWidget();
}

HelpWidget::~HelpWidget()
{
    delete ui;
}

void HelpWidget::InitWidget()
{
    InitStyle();
}

void HelpWidget::InitStyle()
{
    setStyleSheet(
                  "QToolButton{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
             "QToolButton:hover{background-color:#00D2FF;}"
                  "QTextBrowser{background-color:transparent;color:#333333;border:none;font-size:12px;font-family:Microsoft YaHei UI Light;}");


}

void HelpWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);//最后一位是设置透明属性（在0-255取值）
    painter.drawRoundedRect(rect(),10,10);

    painter.drawRect(0,0,10,10);
    QWidget::paintEvent(event);
}
