#include "BlankDefaultWidget.h"
#include "ui_BlankDefaultWidget.h"
#include <QTimer>
#include <QPainter>
BlankDefaultWidget::BlankDefaultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlankDefaultWidget)
{
    ui->setupUi(this);
    QTimer::singleShot(100,this,&BlankDefaultWidget::InitWidget);
}

BlankDefaultWidget::~BlankDefaultWidget()
{
    delete ui;
}

void BlankDefaultWidget::setTextTip(const QString &tip)
{
    ui->label_tip->setText(tip);
}

void BlankDefaultWidget::InitWidget()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::white);
    setPalette(palette);

    ui->label_pic->setPixmap(QPixmap(":/ui/wallet_ui/blank.png").scaled(ui->label_pic->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void BlankDefaultWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(rect());
    QWidget::paintEvent(event);

}
