#include "commondialog.h"
#include "ui_commondialog.h"
#include <QDebug>

#include "wallet.h"

CommonDialog::CommonDialog(commonDialogType type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommonDialog)
{
    
    ui->setupUi(this);


    setParent(UBChain::getInstance()->mainFrame);
    move(UBChain::getInstance()->mainFrame->pos());

    UBChain::getInstance()->appendCurrentDialogVector(this);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget{background-color:rgba(10, 10, 10,100);}");

    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color:rgb(255,255,255);border-radius:10px;}");

    ui->okBtn->setText(tr("Ok"));
    ui->cancelBtn->setText(tr("Cancel"));

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);


    yesOrNO = false;

    if( type == OkAndCancel)
    {
    }
    else if( type == OkOnly)
    {
        ui->cancelBtn->hide();
        ui->okBtn->move(130,160);
    }
    else if( type == YesOrNo)
    {
        ui->okBtn->setText(tr("Yes"));
        ui->cancelBtn->setText(tr("No"));
    }

    
}

CommonDialog::~CommonDialog()
{
    
    qDebug() << "CommonDialog  delete ";
    delete ui;
    UBChain::getInstance()->removeCurrentDialogVector(this);
    
}

void CommonDialog::on_okBtn_clicked()
{
    yesOrNO = true;
    close();
}

void CommonDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

bool CommonDialog::pop()
{
    
    exec();
    return yesOrNO;
}

void CommonDialog::setText(QString text)
{
    ui->textLabel->setText(text);
}

void CommonDialog::on_closeBtn_clicked()
{
    yesOrNO = false;
    close();
}
