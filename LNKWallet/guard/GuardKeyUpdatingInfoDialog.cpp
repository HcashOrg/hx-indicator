#include "GuardKeyUpdatingInfoDialog.h"
#include "ui_GuardKeyUpdatingInfoDialog.h"

#include "wallet.h"

GuardKeyUpdatingInfoDialog::GuardKeyUpdatingInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GuardKeyUpdatingInfoDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->closeBtn->setStyleSheet(CANCELBTN_STYLE);

}

GuardKeyUpdatingInfoDialog::~GuardKeyUpdatingInfoDialog()
{
    delete ui;
}

void GuardKeyUpdatingInfoDialog::pop()
{
    move(0,0);
    exec();
}

void GuardKeyUpdatingInfoDialog::setAsset(QString _assetSymbol)
{
    ui->assetLabel->setText(_assetSymbol);
    QStringList guardAccountIds = UBChain::getInstance()->getInstance()->getAssetMultisigUpdatedGuards(_assetSymbol);
    int totalNum = UBChain::getInstance()->formalGuardMap.size();
    ui->stateLabel->setText(tr("%1 guards have updated. %2 guards have not yet.").arg(guardAccountIds.size()).arg(totalNum - guardAccountIds.size()));

    QString str;
    foreach (QString accountId, guardAccountIds)
    {
        str += UBChain::getInstance()->guardAccountIdToName(accountId) + "  ";
    }
    ui->updatedGuardsLabel->setText(str);
}

void GuardKeyUpdatingInfoDialog::on_closeBtn_clicked()
{
    close();
}
