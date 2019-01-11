#include "AutoUpdateDialog.h"
#include "ui_AutoUpdateDialog.h"

#include "wallet.h"

AutoUpdateDialog::AutoUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoUpdateDialog)
{
    ui->setupUi(this);
    InitWidget();
}

AutoUpdateDialog::~AutoUpdateDialog()
{
    delete ui;
}

bool AutoUpdateDialog::pop()
{
    move(0,0);
    exec();
    return YesOrNo;
}

void AutoUpdateDialog::setVersion(const QString &version, bool isForceUpdate)
{
//    ui->label_version->setText(version);
    if(isForceUpdate)
    {
        ui->okBtn->move(ui->okBtn->geometry().topLeft().x()+70,ui->okBtn->geometry().topLeft().y());

        ui->cancelBtn->setVisible(false);
        ui->closeBtn->setVisible(false);
    }
}

void AutoUpdateDialog::setUpdateLog(const QString &updateLog)
{
    ui->textBrowser->setText(updateLog);
}

void AutoUpdateDialog::confirmUpdateSlots()
{
    YesOrNo = true;
    close();
}

void AutoUpdateDialog::InitWidget()
{
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
    ui->label_pic->setPixmap(QPixmap(":/ui/wallet_ui/auto_update.png"));
    connect(ui->cancelBtn,&QToolButton::clicked,this,&AutoUpdateDialog::close);
    connect(ui->closeBtn,&QToolButton::clicked,this,&AutoUpdateDialog::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&AutoUpdateDialog::confirmUpdateSlots);


}
