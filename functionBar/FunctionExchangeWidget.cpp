#include "FunctionExchangeWidget.h"
#include "ui_FunctionExchangeWidget.h"

#include "extra/style.h"

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
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(94,116,235));
    setPalette(palette);

    setStyleSheet(FUNCTIONBAR_TOOLBUTTON_STYLE);

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
