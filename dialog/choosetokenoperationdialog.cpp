#include "choosetokenoperationdialog.h"
#include "ui_choosetokenoperationdialog.h"

#include "lnk.h"
#include "../commondialog.h"

#include <QDebug>

ChooseTokenOperationDialog::ChooseTokenOperationDialog(QWidget *parent) :
    QDialog(parent),
    choice(0),
    ui(new Ui::ChooseTokenOperationDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

}

ChooseTokenOperationDialog::~ChooseTokenOperationDialog()
{
    delete ui;
}

int ChooseTokenOperationDialog::pop()
{
    move(0,0);
    exec();
    return choice;
}

void ChooseTokenOperationDialog::jsonDataUpdated(QString id)
{

}

void ChooseTokenOperationDialog::on_depositBtn_clicked()
{
    choice = 1;
    close();
}

void ChooseTokenOperationDialog::on_destroyBtn_clicked()
{
    choice = 2;
    close();
}

void ChooseTokenOperationDialog::on_cancelBtn_clicked()
{
    choice = 0;
    close();
}

void ChooseTokenOperationDialog::on_mintBtn_clicked()
{
    choice = 3;
    close();
}

void ChooseTokenOperationDialog::on_withdrawBtn_clicked()
{
    choice = 4;
    close();
}
