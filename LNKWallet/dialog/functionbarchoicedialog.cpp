#include "functionbarchoicedialog.h"
#include "ui_functionbarchoicedialog.h"

#include "wallet.h"
#include "../commondialog.h"

#include <QDebug>


FunctionBarChoiceDialog::FunctionBarChoiceDialog(QWidget *parent) :
    QDialog(parent),
    choice(0),
    ui(new Ui::FunctionBarChoiceDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);
}

FunctionBarChoiceDialog::~FunctionBarChoiceDialog()
{
    delete ui;
}

void FunctionBarChoiceDialog::on_lockBtn_clicked()
{
    choice = 1;
    close();
}

void FunctionBarChoiceDialog::on_consoleBtn_clicked()
{
    choice = 2;
    close();
}

void FunctionBarChoiceDialog::on_setBtn_clicked()
{
    choice = 3;
    close();
}
