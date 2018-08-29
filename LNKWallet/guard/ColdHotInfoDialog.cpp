#include "ColdHotInfoDialog.h"
#include "ui_ColdHotInfoDialog.h"

#include "wallet.h"
#include "ColdHotTransferPage.h"

ColdHotInfoDialog::ColdHotInfoDialog(ColdHotTransferPage *_page, QWidget *parent) :
    QDialog(parent),
    page(_page),
    ui(new Ui::ColdHotInfoDialog)
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

ColdHotInfoDialog::~ColdHotInfoDialog()
{
    delete ui;
}

void ColdHotInfoDialog::pop()
{
    move(0,0);
    exec();
}

void ColdHotInfoDialog::setTrxId(QString _trxId)
{
    if(page == NULL)    return;

    ColdHotTransaction cht;
    if(page->currentType == 1)
    {
        cht = page->coldHotTransactionMap.value(_trxId);
    }
    else
    {
        cht = page->pendingColdHotTransactionMap.value(_trxId);
    }

    ui->amountLabel->setText(QString("%1 %2").arg(cht.amount).arg(cht.assetSymbol));
    ui->fromLabel->setText(cht.withdrawAddress);
    ui->toLabel->setText(cht.depositAddress);

    QString crosschainTrxId = page->lookupCrosschainTrxByColdHotTrxId(_trxId);
    QStringList guardAddresses = page->lookupSignedGuardByCrosschainTrx(crosschainTrxId);

    int totalNum = HXChain::getInstance()->getFormalGuards().size();
    ui->stateLabel->setText(tr("%1 guards have signed. %2 guards have not yet.").arg(guardAddresses.size()).arg(totalNum - guardAddresses.size()));

    QString str;
    foreach (QString address, guardAddresses)
    {
        str += HXChain::getInstance()->guardAddressToName(address) + "  ";
    }
    ui->signedGuardsLabel->setText(str);

}

void ColdHotInfoDialog::on_closeBtn_clicked()
{
    close();
}
