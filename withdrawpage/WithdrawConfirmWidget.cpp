#include "WithdrawConfirmWidget.h"
#include "ui_WithdrawConfirmWidget.h"

WithdrawConfirmWidget::WithdrawConfirmWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WithdrawConfirmWidget)
{
    ui->setupUi(this);
}

WithdrawConfirmWidget::~WithdrawConfirmWidget()
{
    delete ui;
}
