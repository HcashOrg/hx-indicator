#include "locktominerdialog.h"
#include "ui_locktominerdialog.h"

#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/TransactionResultDialog.h"

LockToMinerDialog::LockToMinerDialog(QString _accountName, QWidget *parent) :
    QDialog(parent),
    m_accountName(_accountName),
    ui(new Ui::LockToMinerDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color:rgb(255,255,255);border-radius:10px;}");

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    qDebug()<<_accountName;
    ui->stackedWidget->addWidget(new FeeChooseWidget(UBChain::getInstance()->feeChargeInfo.minerForeCloseFee.toDouble(),
                                                     UBChain::getInstance()->feeType,_accountName));
    ui->stackedWidget->setCurrentIndex(0);
    init();

}

LockToMinerDialog::~LockToMinerDialog()
{
    delete ui;
}

void LockToMinerDialog::pop()
{
    move(0,0);
    exec();
}

void LockToMinerDialog::init()
{
    ui->accountNameLabel->setText(m_accountName);

    QStringList miners;
    foreach (Miner m, UBChain::getInstance()->minersVector)
    {
        miners += m.name;
    }

    ui->minerComboBox->addItems(miners);

    QStringList keys = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString key, keys)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(key).symbol, key);
    }
}

void LockToMinerDialog::setMiner(QString _minerName)
{
    ui->minerComboBox->setCurrentText(_minerName);
}

void LockToMinerDialog::setAsset(QString _assetName)
{
    ui->assetComboBox->setCurrentText(_assetName);
}

void LockToMinerDialog::jsonDataUpdated(QString id)
{
    if( id == "id-lock_balance_to_miner")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        qDebug() << id << result;
        if(result.startsWith("\"result\":{"))
        {
            close();

            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of lock-to-miner has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Fail to lock balance to miner!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }
    }
}

void LockToMinerDialog::on_okBtn_clicked()
{
    if(!UBChain::getInstance()->ValidateOnChainOperation()) return;

    UBChain::getInstance()->postRPC( "id-lock_balance_to_miner",
                                     toJsonFormat( "lock_balance_to_miner",
                                                   QJsonArray() << ui->minerComboBox->currentText() << m_accountName
                                                   << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
                                                   << true ));

}

void LockToMinerDialog::on_cancelBtn_clicked()
{
    close();
}

void LockToMinerDialog::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    ui->amountLineEdit->clear();

    AssetAmount assetAmount = UBChain::getInstance()->accountInfoMap.value(m_accountName).assetAmountMap.value(ui->assetComboBox->currentData().toString());
    QString amountStr = getBigNumberString(assetAmount.amount, UBChain::getInstance()->assetInfoMap.value(ui->assetComboBox->currentData().toString()).precision);

    ui->amountLineEdit->setPlaceholderText(tr("Max: %1 %2").arg(amountStr).arg(ui->assetComboBox->currentText()) );
}

void LockToMinerDialog::on_closeBtn_clicked()
{
    close();
}
