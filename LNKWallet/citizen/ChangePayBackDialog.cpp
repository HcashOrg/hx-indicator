#include "ChangePayBackDialog.h"
#include "ui_ChangePayBackDialog.h"

#include <limits>
#include <functional>
#include <QIntValidator>
#include "wallet.h"
#include "FeeChooseWidget.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "capitalTransferPage/PasswordConfirmWidget.h"

ChangePayBackDialog::ChangePayBackDialog(const QString &name,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePayBackDialog),
    citizenName(name)
{
    ui->setupUi(this);
    InitWidget();
}

ChangePayBackDialog::~ChangePayBackDialog()
{
    delete ui;
}

void ChangePayBackDialog::ChangePaybackSlots()
{
    if( ui->lineEdit_feeNumber->text().isEmpty()) return;

    int number = ui->lineEdit_feeNumber->text().toInt();
    //输入密码框
    PasswordConfirmWidget *wi = new PasswordConfirmWidget(HXChain::getInstance()->mainFrame);
    connect(wi,&PasswordConfirmWidget::confirmSignal,[this,number](){
        if(FeeChooseWidget* feeWidget = dynamic_cast<FeeChooseWidget*>(this->ui->stackedWidget_fee->currentWidget()))
        {
            feeWidget->updatePoundageID();
        }
        HXChain::getInstance()->postRPC("id_change_payback_rate",toJsonFormat("set_citizen_pledge_pay_back_rate",
                                        QJsonArray()<<this->citizenName<<number<<true));

    });
    wi->show();
}

void ChangePayBackDialog::jsonDataUpdated(QString id)
{
    if("id_change_payback_rate" == id)
    {
        close();
        QString result = HXChain::getInstance()->jsonDataValue(id);
        if( result.startsWith("\"result\":{"))             // 成功
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of changing payback rate has been sent,please wait for confirmation"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();

            HXChain::getInstance()->fetchCitizenPayBack();
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

void ChangePayBackDialog::InitWidget()
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


    installDoubleValidator(ui->lineEdit_feeNumber,0,100);
    InitData();


    connect(ui->closeBtn,&QToolButton::clicked,this,&QDialog::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&QDialog::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&ChangePayBackDialog::ChangePaybackSlots);
    connect(HXChain::getInstance(),&HXChain::jsonDataUpdated,this,&ChangePayBackDialog::jsonDataUpdated);


    FeeChooseWidget *feeWidget = new FeeChooseWidget(HXChain::getInstance()->feeChargeInfo.changePaybackFee.toDouble(),
                                                     HXChain::getInstance()->feeType,citizenName);
    connect(feeWidget,&FeeChooseWidget::feeSufficient,ui->okBtn,&QToolButton::setEnabled);
    QTimer::singleShot(100,[this,feeWidget](){this->ui->okBtn->setEnabled(feeWidget->isSufficient());});
    ui->stackedWidget_fee->addWidget(feeWidget);
    ui->stackedWidget_fee->setCurrentIndex(0);
    ui->stackedWidget_fee->currentWidget()->resize(ui->stackedWidget_fee->size());
}

void ChangePayBackDialog::InitData()
{
    QStringList citizens =  HXChain::getInstance()->getMyCitizens();
    foreach (QString info, citizens) {
        QMap<QString,MinerInfo> miners(HXChain::getInstance()->minerMap);
        QString rate = QString::number(miners.value(info).payBack);
        ui->account->addItem(info,rate);
    }
    //初始化label
    ui->label_rate->setText(ui->account->currentData().toString());
    connect(ui->account,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),[this](){
        this->ui->label_rate->setText(this->ui->account->currentData().toString());});
}

void ChangePayBackDialog::installDoubleValidator(QLineEdit *line, int mi, int ma)
{
    QIntValidator *validator = new QIntValidator(mi,ma);
    line->setValidator( validator );
}
