#include "ExchangeContractFeeDialog.h"
#include "ui_ExchangeContractFeeDialog.h"

#include "wallet.h"
#include "FeeChooseWidget.h"

ExchangeContractFeeDialog::ExchangeContractFeeDialog(unsigned long long _feeAmount, QString _accountName, QWidget *parent) :
    QDialog(parent),
    feeAmount(_feeAmount),
    account(_accountName),
    ui(new Ui::ExchangeContractFeeDialog)
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

    feeChoose = new FeeChooseWidget(0,HXChain::getInstance()->feeType);
    ui->stackedWidget->addWidget(feeChoose);
    feeChoose->resize(ui->stackedWidget->size());
    ui->stackedWidget->setCurrentIndex(0);

    init();
}

ExchangeContractFeeDialog::~ExchangeContractFeeDialog()
{
    delete ui;
}

bool ExchangeContractFeeDialog::pop()
{
    move(0,0);
    exec();

    return  yesOrNo;
}

void ExchangeContractFeeDialog::init()
{
    feeChoose->updateFeeNumberSlots(getBigNumberString(ceil(feeAmount * HXChain::getInstance()->contractFee / 100.0), ASSET_PRECISION).toDouble());
    feeChoose->updateAccountNameSlots(account);
}

void ExchangeContractFeeDialog::updatePoundageID()
{
    feeChoose->updatePoundageID();
}

void ExchangeContractFeeDialog::on_okBtn_clicked()
{
    yesOrNo = true;
    close();
}

void ExchangeContractFeeDialog::on_cancelBtn_clicked()
{
    yesOrNo = false;
    close();
}
