#include "choosemultisigoperationdialog.h"
#include "ui_choosemultisigoperationdialog.h"

#include "lnk.h"
#include "../commondialog.h"

#include <QDebug>

ChooseMultiSigOperationDialog::ChooseMultiSigOperationDialog(QWidget *parent) :
    QDialog(parent),
    choice(0),
    ui(new Ui::ChooseMultiSigOperationDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);
}

ChooseMultiSigOperationDialog::~ChooseMultiSigOperationDialog()
{
    delete ui;
}

int ChooseMultiSigOperationDialog::pop()
{
    exec();
    return choice;
}


void ChooseMultiSigOperationDialog::on_detailBtn_clicked()
{
    choice = 1;
    close();
}

void ChooseMultiSigOperationDialog::on_depositBtn_clicked()
{
    choice = 2;
    close();
}

void ChooseMultiSigOperationDialog::on_sendOutBtn_clicked()
{
    choice = 3;
    close();
}

void ChooseMultiSigOperationDialog::on_transactionBtn_clicked()
{
    choice = 4;
    close();
}
