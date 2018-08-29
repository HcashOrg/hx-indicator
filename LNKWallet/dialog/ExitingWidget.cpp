#include "ExitingWidget.h"
#include "ui_ExitingWidget.h"

#include "wallet.h"

ExitingWidget::ExitingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExitingWidget)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/login_back.png").scaled(this->size())));
    setPalette(palette);

    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);
}

ExitingWidget::~ExitingWidget()
{
    delete ui;
}

void ExitingWidget::on_closeBtn_clicked()
{

}

void ExitingWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(243,241,250)));
    painter.drawRect(0,0,228,24);
    painter.drawPixmap(7,5,32,12,QPixmap(":/ui/wallet_ui/hx_label_logo.png").scaled(32,12,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    painter.drawPixmap(94,38,36,36,QPixmap(":/ui/wallet_ui/logo_center.png").scaled(36,36,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));


    QWidget::paintEvent(e);
}
