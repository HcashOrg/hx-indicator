#include "AddPledgeDialog.h"
#include "ui_AddPledgeDialog.h"

#include <functional>
#include "wallet.h"
#include "FeeChooseWidget.h"
#include "capitalTransferPage/PasswordConfirmWidget.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"

AddPledgeDialog::AddPledgeDialog(const QString &name,const ProposalInfo &info,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPledgeDialog),
    accountName(name),
    proposalInfo(info)
{
    ui->setupUi(this);
    InitWidget();
}

AddPledgeDialog::~AddPledgeDialog()
{
    delete ui;
}

void AddPledgeDialog::AddPledgeSlots()
{
    //输入密码框

    PasswordConfirmWidget *wi = new PasswordConfirmWidget(HXChain::getInstance()->mainFrame);
    connect(wi,&PasswordConfirmWidget::confirmSignal,[this](){
        if(FeeChooseWidget* feeWidget = dynamic_cast<FeeChooseWidget*>(this->ui->stackedWidget_fee->currentWidget()))
        {
            feeWidget->updatePoundageID();
        }
        HXChain::getInstance()->postRPC("id_add_referendum_pledge",toJsonFormat("referendum_accelerate_pledge",
                                        QJsonArray()<<this->proposalInfo.proposalId<<this->ui->lineEdit_feeNumber->text()<<true));

    });
    wi->show();
}

void AddPledgeDialog::jsonDataUpdated(QString id)
{
    if("id_add_referendum_pledge" == id)
    {
        close();
        QString result = HXChain::getInstance()->jsonDataValue(id);
        if( result.startsWith("\"result\":{"))             // 成功
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of adding pledge has been sent,please wait for confirmation"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Failed!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }
    }
}

void AddPledgeDialog::InitWidget()
{
    setParent(HXChain::getInstance()->mainFrame);
    move(0,0);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    InitData();

    connect(ui->closeBtn,&QToolButton::clicked,this,&QDialog::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&QDialog::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&AddPledgeDialog::AddPledgeSlots);
    connect(HXChain::getInstance(),&HXChain::jsonDataUpdated,this,&AddPledgeDialog::jsonDataUpdated);

    FeeChooseWidget *feeWidget = new FeeChooseWidget(0,HXChain::getInstance()->feeType,accountName);
    connect(feeWidget,&FeeChooseWidget::feeSufficient,ui->okBtn,&QToolButton::setEnabled);
    connect(ui->lineEdit_feeNumber,&QLineEdit::textChanged,[feeWidget](const QString &number){
        feeWidget->updateFeeNumberSlots(number.toDouble());
    });
    ui->stackedWidget_fee->addWidget(feeWidget);
    ui->stackedWidget_fee->setCurrentIndex(0);
    ui->stackedWidget_fee->currentWidget()->resize(ui->stackedWidget_fee->size());
}

void AddPledgeDialog::InitData()
{
    ui->currentPledge->setText(proposalInfo.pledge);
}
