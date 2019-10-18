#include "ApplyExchangeDialog.h"
#include "ui_ApplyExchangeDialog.h"

#include <QClipboard>

#include "wallet.h"
#include "commondialog.h"
#include "dapp/ContractTokenPage.h"

#define APPLY_EXCHANGE_ADDRESS ""

ApplyExchangeDialog::ApplyExchangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplyExchangeDialog)
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

    ui->copyBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/copy.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                               "QToolButton:hover{background-image:url(:/ui/wallet_ui/copy_hover.png);}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));


    init();
}

ApplyExchangeDialog::~ApplyExchangeDialog()
{
    delete ui;
}

void ApplyExchangeDialog::pop()
{
    move(0,0);
    exec();
}

void ApplyExchangeDialog::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);
    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

}

void ApplyExchangeDialog::setParentPage(ContractTokenPage *p)
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

    inited = true;
    calculateMemo();
}

void ApplyExchangeDialog::on_closeBtn_clicked()
{
    close();
}

void ApplyExchangeDialog::on_cancelBtn_clicked()
{
    close();
}

void ApplyExchangeDialog::on_okBtn_clicked()
{
    if(!ui->hxCheckBox->isChecked() && !ui->paxCheckBox->isChecked())
    {
        CommonDialog dialog(CommonDialog::OkOnly);
        dialog.setText(QString::fromLocal8Bit("You must select at least 1 pair!"));
        dialog.pop();
    }
    else if(ui->memoLabel->text().isEmpty())
    {
        CommonDialog dialog(CommonDialog::OkOnly);
        dialog.setText(QString::fromLocal8Bit("Please select the contract and account!"));
        dialog.pop();
    }
    else
    {
        int amount = 0;
        if(ui->hxCheckBox->isChecked())     amount += 1000;
        if(ui->paxCheckBox->isChecked())    amount += 10000;

        close();
        if(page)    page->gotoTransferPage(ui->accountComboBox->currentText(), APPLY_EXCHANGE_ADDRESS, QString::number(amount), ui->memoLabel->text());
    }
}

void ApplyExchangeDialog::calculateMemo()
{
    QString contractAddress = ui->contractComboBox->currentData().toString();
    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());

    ui->memoLabel->clear();
    if(contractAddress.isEmpty() || info.address.isEmpty())     return;

    QByteArray byte_array;
    byte_array.append((contractAddress + info.address).toUtf8());
    QByteArray hash_byte_array = QCryptographicHash::hash(byte_array, QCryptographicHash::Md5);
    QString md5 = hash_byte_array.toHex();
    qDebug() <<"mmmmmmmm " << md5;
    ui->memoLabel->setText(md5.left(8));
}



void ApplyExchangeDialog::on_contractComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;
    calculateMemo();
}

void ApplyExchangeDialog::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;
    calculateMemo();
}

void ApplyExchangeDialog::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->memoLabel->text());
}
