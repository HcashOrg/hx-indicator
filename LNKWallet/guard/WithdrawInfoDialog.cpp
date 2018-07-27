#include "WithdrawInfoDialog.h"
#include "ui_WithdrawInfoDialog.h"

#include "wallet.h"
#include "WithdrawConfirmPage.h"

WithdrawInfoDialog::WithdrawInfoDialog(WithdrawConfirmPage* _page, QWidget *parent) :
    QDialog(parent),
    page(_page),
    ui(new Ui::WithdrawInfoDialog)
{
    ui->setupUi(this);

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->closeBtn->setStyleSheet(CANCELBTN_STYLE);
}

WithdrawInfoDialog::~WithdrawInfoDialog()
{
    delete ui;
}

void WithdrawInfoDialog::pop()
{
    move(0,0);
    exec();
}

void WithdrawInfoDialog::setTrxId(QString _trxId)
{
    if(page == NULL)    return;
    ApplyTransaction at;
    if(page->currentType == 1)
    {
        at = page->applyTransactionMap.value(_trxId);
    }
    else if(page->currentType == 0)
    {
        at = page->pendingApplyTransactionMap.value(_trxId);
    }

    ui->amountLabel->setText(QString("%1 %2").arg(at.amount).arg(at.assetSymbol));
    ui->withdrawAddressLabel->setText(at.withdrawAddress);
    ui->crosschainAddressLabel->setText(at.crosschainAddress);

    QString generatedTrxId = page->lookupGeneratedTrxByApplyTrxId(_trxId);
    QStringList guardAddresses = page->lookupSignedGuardsByGeneratedTrxId(generatedTrxId);

    int totalNum = HXChain::getInstance()->formalGuardMap.size();
    ui->stateLabel->setText(tr("%1 sentors have signed. %2 sentors have not yet.").arg(guardAddresses.size()).arg(totalNum - guardAddresses.size()));

    QString str;
    foreach (QString address, guardAddresses)
    {
        str += HXChain::getInstance()->guardAddressToName(address) + "  ";
    }
    ui->signedGuardsLabel->setText(str);

}

void WithdrawInfoDialog::on_closeBtn_clicked()
{
    close();
}
