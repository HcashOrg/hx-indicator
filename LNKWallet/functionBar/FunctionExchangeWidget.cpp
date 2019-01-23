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
    on_exchangeBtn_clicked();
}

void FunctionExchangeWidget::InitWidget()
{
    InitStyle();
    ui->exchangeBtn->setCheckable(true);
    ui->onchainOrderBtn->setCheckable(true);
    ui->contractTokenBtn->setCheckable(true);
}

void FunctionExchangeWidget::InitStyle()
{
    setStyleSheet(FUNCTIONBAR_PUSHBUTTON_STYLE);

}


void FunctionExchangeWidget::on_exchangeBtn_clicked()
{
    ui->exchangeBtn->setChecked(true);
    ui->onchainOrderBtn->setChecked(false);
    ui->contractTokenBtn->setChecked(false);
    Q_EMIT showExchangeModeSignal();
}


void FunctionExchangeWidget::on_onchainOrderBtn_clicked()
{
    ui->exchangeBtn->setChecked(false);
    ui->onchainOrderBtn->setChecked(true);
    ui->contractTokenBtn->setChecked(false);
    Q_EMIT showOnchainOrderSignal();
}

void FunctionExchangeWidget::on_myOrderBtn_clicked()
{
    ui->exchangeBtn->setChecked(false);
    ui->onchainOrderBtn->setChecked(false);
    ui->contractTokenBtn->setChecked(false);
    Q_EMIT showMyOrderSignal();
}

void FunctionExchangeWidget::on_contractTokenBtn_clicked()
{
    ui->exchangeBtn->setChecked(false);
    ui->onchainOrderBtn->setChecked(false);
    ui->contractTokenBtn->setChecked(true);
    Q_EMIT showContractTokenSignal();
}


void FunctionExchangeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QLinearGradient linear(QPointF(0, 480), QPointF(130, 0));
    linear.setColorAt(0, QColor(71,33,84));
    linear.setColorAt(0.5, QColor(55,36,88));
    linear.setColorAt(1, QColor(51,37,90));
    linear.setSpread(QGradient::PadSpread);
    painter.setBrush(linear);
    painter.drawRect(QRect(-1,-1,131,481));

    painter.setPen(QColor(81,59,134));
    painter.drawLine(20,62,110,62);
    painter.drawLine(20,202,110,202);
}

