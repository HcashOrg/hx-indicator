#include "CreateSenatorDialog.h"
#include "ui_CreateSenatorDialog.h"

#include "wallet.h"
#include "FeeChooseWidget.h"
#include "miner/registerdialog.h"
#include "capitalTransferPage/PasswordConfirmWidget.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"

CreateSenatorDialog::CreateSenatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateSenatorDialog)
{
    ui->setupUi(this);
    InitWidget();
}

CreateSenatorDialog::~CreateSenatorDialog()
{
    delete ui;
}

void CreateSenatorDialog::jsonDataUpdated(QString id)
{
    if("id_create_senator" == id)
    {
        close();
        QString result = HXChain::getInstance()->jsonDataValue(id);
        if( result.startsWith("\"result\":{"))             // 成功
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of create senator has been sent,please wait for confirmation"));
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

void CreateSenatorDialog::on_registerLabel_linkActivated(const QString &link)
{
    close();

    RegisterDialog registerDialog;
    registerDialog.pop();

}

void CreateSenatorDialog::CreateSenatorSlots()
{
    //输入密码框
    PasswordConfirmWidget *wi = new PasswordConfirmWidget(HXChain::getInstance()->mainFrame);
    connect(wi,&PasswordConfirmWidget::confirmSignal,[this](){
        if(FeeChooseWidget* feeWidget = dynamic_cast<FeeChooseWidget*>(this->ui->stackedWidget_fee->currentWidget()))
        {
            feeWidget->updatePoundageID();
        }
        HXChain::getInstance()->postRPC("id_create_senator",toJsonFormat("create_senator_member",
                                        QJsonArray()<<this->ui->accountComboBox->currentText()<<true));

    });
    wi->show();
}

void CreateSenatorDialog::InitWidget()
{
    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

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

    ui->stackedWidget->setCurrentIndex(0);

    InitData();

    connect(ui->closeBtn,&QToolButton::clicked,this,&CreateSenatorDialog::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&CreateSenatorDialog::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&CreateSenatorDialog::CreateSenatorSlots);

    FeeChooseWidget *feeWidget = new FeeChooseWidget(HXChain::getInstance()->feeChargeInfo.createSenatorFee.toDouble(),
                                                     HXChain::getInstance()->feeType,ui->accountComboBox->currentText());
//    QTimer::singleShot(100,[this,feeWidget](){this->ui->okBtn->setEnabled(feeWidget->isSufficient());});
    connect(feeWidget,&FeeChooseWidget::feeSufficient,ui->okBtn,&QToolButton::setEnabled);
    connect(ui->accountComboBox,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),
            std::bind(&FeeChooseWidget::updateAccountNameSlots,feeWidget,std::placeholders::_1,true));
    ui->stackedWidget_fee->addWidget(feeWidget);
    ui->stackedWidget_fee->setCurrentIndex(0);
    ui->stackedWidget_fee->currentWidget()->resize(ui->stackedWidget_fee->size());
}

void CreateSenatorDialog::InitData()
{
    QStringList keys = HXChain::getInstance()->getRegisteredAccounts();
    QStringList miners = HXChain::getInstance()->minerMap.keys();
    QStringList citizens = HXChain::getInstance()->getMyCitizens();
    foreach (QString key, keys)
    {
        if(miners.contains(key) || citizens.contains(key))
        {
            keys.removeAll(key);
        }
    }
    ui->accountComboBox->addItems(keys);

}
