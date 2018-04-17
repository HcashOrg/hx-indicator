#include "PasswordConfirmWidget.h"
#include "ui_PasswordConfirmWidget.h"

PasswordConfirmWidget::PasswordConfirmWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PasswordConfirmWidget)
{
    ui->setupUi(this);
}

PasswordConfirmWidget::~PasswordConfirmWidget()
{
    delete ui;
}
