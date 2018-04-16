#include "DepositRecrdWideget.h"
#include "ui_DepositRecrdWideget.h"

DepositRecrdWideget::DepositRecrdWideget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DepositRecrdWideget)
{
    ui->setupUi(this);
}

DepositRecrdWideget::~DepositRecrdWideget()
{
    delete ui;
}
