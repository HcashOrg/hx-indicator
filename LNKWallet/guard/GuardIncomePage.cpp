#include "GuardIncomePage.h"
#include "ui_GuardIncomePage.h"

GuardIncomePage::GuardIncomePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GuardIncomePage)
{
    ui->setupUi(this);
}

GuardIncomePage::~GuardIncomePage()
{
    delete ui;
}
