#include "foreclosedialog.h"
#include "ui_foreclosedialog.h"

#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"

ForecloseDialog::ForecloseDialog(const QString &accountName,QWidget *parent) :
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

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->stackedWidget->addWidget(new FeeChooseWidget(UBChain::getInstance()->feeChargeInfo.minerForeCloseFee.toDouble(),
                                                     UBChain::getInstance()->feeType,accountName));
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
