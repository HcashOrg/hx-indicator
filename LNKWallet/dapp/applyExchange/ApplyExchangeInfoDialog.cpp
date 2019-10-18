#include "ApplyExchangeInfoDialog.h"
#include "ui_ApplyExchangeInfoDialog.h"

#include "wallet.h"
#include "dapp/ContractTokenPage.h"

ApplyExchangeInfoDialog::ApplyExchangeInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplyExchangeInfoDialog)
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

    init();
}

ApplyExchangeInfoDialog::~ApplyExchangeInfoDialog()
{
    delete ui;
}

void ApplyExchangeInfoDialog::pop()
{
    move(0,0);
    exec();
}

void ApplyExchangeInfoDialog::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);
    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }
}

void ApplyExchangeInfoDialog::setParentPage(ContractTokenPage *p)
{
    page = p;
    QStringList contractIds = page->contractTokenInfoMap.keys();
    for(int i = 0; i < contractIds.size(); i++)
    {
        QString contractId = contractIds.at(i);
        ContractTokenInfo tokenInfo = page->contractTokenInfoMap.value(contractId);
        QString str = QString("%1 (%2)").arg(tokenInfo.symbol).arg(contractId);
        ui->contractComboBox->addItem(str, contractId);
    }

}

void ApplyExchangeInfoDialog::on_okBtn_clicked()
{

}

void ApplyExchangeInfoDialog::on_cancelBtn_clicked()
{
    close();
}

void ApplyExchangeInfoDialog::on_closeBtn_clicked()
{
    close();
}
