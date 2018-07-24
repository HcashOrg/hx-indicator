#include "FunctionExchangeWidget.h"
#include "ui_FunctionExchangeWidget.h"

#include "extra/style.h"
#include <QPainter>

FunctionExchangeWidget::FunctionExchangeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionExchangeWidget)
{
    ui->setupUi(this);
    InitWidget();
}

FunctionExchangeWidget::~FunctionExchangeWidget()
{
    delete ui;
}

void FunctionExchangeWidget::retranslator()
{
    ui->retranslateUi(this);
}

void FunctionExchangeWidget::DefaultShow()
{
    on_onchainOrderBtn_clicked();
}

void FunctionExchangeWidget::InitWidget()
{
    InitStyle();
    ui->onchainOrderBtn->setCheckable(true);
    ui->myOrderBtn->setCheckable(true);
}

void FunctionExchangeWidget::InitStyle()
{
    setStyleSheet(FUNCTIONBAR_PUSHBUTTON_STYLE);

}

void FunctionExchangeWidget::on_onchainOrderBtn_clicked()
{
    ui->onchainOrderBtn->setChecked(true);
    ui->myOrderBtn->setChecked(false);
    emit showOnchainOrderSignal();
}

void FunctionExchangeWidget::on_myOrderBtn_clicked()
{
    ui->onchainOrderBtn->setChecked(false);
    ui->myOrderBtn->setChecked(true);
    emit showMyOrderSignal();
}

void FunctionExchangeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QLinearGradient linear(QPointF(0, 480), QPointF(130, 0));
    linear.setColorAt(0, QColor(56,19,56));
    linear.setColorAt(1, QColor(27,17,44));
    linear.setSpread(QGradient::PadSpread);
    painter.setBrush(linear);
    painter.drawRect(QRect(-1,-1,131,481));

    painter.setPen(QColor(45,29,71));
    painter.drawLine(20,62,110,62);
}
