#include "OnchainOrderPage.h"
#include "ui_OnchainOrderPage.h"

OnchainOrderPage::OnchainOrderPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnchainOrderPage)
{
    ui->setupUi(this);
}

OnchainOrderPage::~OnchainOrderPage()
{
    delete ui;
}
