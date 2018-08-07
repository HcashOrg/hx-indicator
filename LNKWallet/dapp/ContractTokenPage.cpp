#include "ContractTokenPage.h"
#include "ui_ContractTokenPage.h"

ContractTokenPage::ContractTokenPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContractTokenPage)
{
    ui->setupUi(this);
}

ContractTokenPage::~ContractTokenPage()
{
    delete ui;
}

