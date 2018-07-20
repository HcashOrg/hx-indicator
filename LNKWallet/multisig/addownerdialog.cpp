#include "addownerdialog.h"
#include "ui_addownerdialog.h"

#include <QDebug>

#include "wallet.h"
#include "../commondialog.h"
#include "showcontentdialog.h"

AddOwnerDialog::AddOwnerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOwnerDialog)
{
    ui->setupUi(this);

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->okBtn->setEnabled(false);

    ui->addressLineEdit->setPlaceholderText( tr("Please enter an account address."));
    ui->addressLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->addressLineEdit->setFocus();
}

AddOwnerDialog::~AddOwnerDialog()
{
    delete ui;
}

void AddOwnerDialog::pop()
{
    move(0,0);
    exec();
}

void AddOwnerDialog::on_okBtn_clicked()
{
    if(!checkAddress(ui->addressLineEdit->text()))  return;

    ownerAddress = ui->addressLineEdit->text();

    close();
}

void AddOwnerDialog::on_cancelBtn_clicked()
{
    close();
}

void AddOwnerDialog::on_addressLineEdit_textEdited(const QString &arg1)
{
    QString address = ui->addressLineEdit->text();
    address = address.simplified();
    ui->addressLineEdit->setText(address);

    if(checkAddress(address))
    {
        ui->okBtn->setEnabled(true);
    }
    else
    {
        ui->okBtn->setEnabled(false);
    }
}

void AddOwnerDialog::on_closeBtn_clicked()
{
    close();
}
