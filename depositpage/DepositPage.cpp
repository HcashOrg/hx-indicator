#include "DepositPage.h"
#include "ui_DepositPage.h"

DepositPage::DepositPage(DepositDataInput data,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DepositPage)
{
    ui->setupUi(this);
    InitWidget();
}

DepositPage::~DepositPage()
{
    delete ui;
}

void DepositPage::InitWidget()
{
    InitStyle();
}

void DepositPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248,249,253));
    setPalette(palette);
}
