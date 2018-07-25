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

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->stackedWidget->addWidget(new FeeChooseWidget(HXChain::getInstance()->feeChargeInfo.minerForeCloseFee.toDouble(),
                                                     HXChain::getInstance()->feeType,accountName));
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->currentWidget()->resize(ui->stackedWidget->size());
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
