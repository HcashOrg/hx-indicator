#include "FunctionExchangeWidget.h"
#include "ui_FunctionExchangeWidget.h"

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

void FunctionExchangeWidget::InitWidget()
{
    ui->onchainOrderBtn->setCheckable(true);
    ui->myOrderBtn->setCheckable(true);
}

void FunctionExchangeWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(94,116,235));
    setPalette(palette);

    setStyleSheet("QToolButton{background: rgb(94,116,235);font-size:14px;font-family:Microsoft YaHei UI Light;}\
                   QToolButton:hover{background-color: rgb(0,210, 255);}\
                   QToolButton:pressed{background-color: rgb(130,157, 255);}\
                   QToolButton:checked{background-color: rgb(130,157, 255);}");

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
