#include "BlankDefaultWidget.h"
#include "ui_BlankDefaultWidget.h"
#include <QTimer>
#include <QPainter>
BlankDefaultWidget::BlankDefaultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlankDefaultWidget)
{
    ui->setupUi(this);
    if(parent)
    {
        this->resize(parent->size());
    }
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

    ui->label_pic->setPixmap(QPixmap(":/ui/wallet_ui/blank.png"));
    this->setStyleSheet("QWidget{border-radius:5px;}");
}

void BlankDefaultWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(243,241,250));
    painter.drawRoundedRect(rect(),5,5);
    QWidget::paintEvent(event);

}
