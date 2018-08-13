#include "TokenTransferWidget.h"
#include "ui_TokenTransferWidget.h"

TokenTransferWidget::TokenTransferWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TokenTransferWidget)
{
    ui->setupUi(this);
}

TokenTransferWidget::~TokenTransferWidget()
{
    delete ui;
}
