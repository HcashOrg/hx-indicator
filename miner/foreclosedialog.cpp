#include "foreclosedialog.h"
#include "ui_foreclosedialog.h"

#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"

ForecloseDialog::ForecloseDialog(QWidget *parent) :
    QDialog(parent),
    yesOrNo(false),
    ui(new Ui::ForecloseDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color:rgb(255,255,255);border-radius:10px;}");

    ui->okBtn->setStyleSheet("QToolButton#okBtn{border:none;color:white;border-radius:10px;font-size:12pt;background-color:#5474EB;}"
                             "QToolButton#okBtn::hover{background-color:#00D2FF;}");
    ui->cancelBtn->setStyleSheet("QToolButton#cancelBtn{border:none;color:white;border-radius:10px;font-size:12pt;background-color:#E5E5E5;}"
                                 "QToolButton#cancelBtn::hover{background-color:#00D2FF;}");

    ui->closeBtn->setIconSize(QSize(12,12));
    ui->closeBtn->setIcon(QIcon(":/ui/wallet_ui/close.png"));
    ui->closeBtn->setStyleSheet("QToolButton{background-color:transparent;border:none;}"
                                "QToolButton:hover{background-color:rgb(208,228,255);}");

    ui->amountLineEdit->setStyleSheet("QLineEdit{border-top:none;border-left:none;border-right:none;border-bottom:1px solid gray;\
                                     background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;padding-right:80px;}");

    ui->stackedWidget->addWidget(new FeeChooseWidget(UBChain::getInstance()->feeChargeInfo.minerForeCloseFee.toDouble(),"LNK"));
    ui->stackedWidget->setCurrentIndex(0);
}

ForecloseDialog::~ForecloseDialog()
{
    delete ui;
}

QString ForecloseDialog::pop()
{
    move(0,0);
    exec();

    if(yesOrNo)
    {
        return ui->amountLineEdit->text();
    }
    else
    {
        return "";
    }
}

void ForecloseDialog::on_okBtn_clicked()
{
    yesOrNo = true;
    close();
}

void ForecloseDialog::on_cancelBtn_clicked()
{
    yesOrNo = false;
    close();
}

void ForecloseDialog::on_closeBtn_clicked()
{
    yesOrNo = false;
    close();
}
