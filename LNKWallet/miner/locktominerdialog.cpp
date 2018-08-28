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

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    qDebug()<<_accountName;
    ui->stackedWidget->addWidget(new FeeChooseWidget(HXChain::getInstance()->feeChargeInfo.minerForeCloseFee.toDouble(),
                                                     HXChain::getInstance()->feeType,_accountName));
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->currentWidget()->resize(ui->stackedWidget->size());
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

    ui->minerComboBox->addItems(HXChain::getInstance()->minerMap.keys());

    QStringList keys = HXChain::getInstance()->assetInfoMap.keys();
    foreach (QString key, keys)
    {
        ui->assetComboBox->addItem(HXChain::getInstance()->assetInfoMap.value(key).symbol, key);
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
    if( id == "id-lock_balance_to_citizen")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

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
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

    HXChain::getInstance()->postRPC( "id-lock_balance_to_citizen",
                                     toJsonFormat( "lock_balance_to_citizen",
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

    AssetAmount assetAmount = HXChain::getInstance()->accountInfoMap.value(m_accountName).assetAmountMap.value(ui->assetComboBox->currentData().toString());
    QString amountStr = getBigNumberString(assetAmount.amount, HXChain::getInstance()->assetInfoMap.value(ui->assetComboBox->currentData().toString()).precision);

    ui->amountLineEdit->setPlaceholderText(tr("Max: %1 %2").arg(amountStr).arg(ui->assetComboBox->currentText()) );

    AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(ui->assetComboBox->currentText()));
    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(assetInfo.precision));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->clear();
}

void LockToMinerDialog::on_closeBtn_clicked()
{
    close();
}

void LockToMinerDialog::on_amountLineEdit_textEdited(const QString &arg1)
{
    QString assetId = HXChain::getInstance()->getAssetId(ui->assetComboBox->currentText());
    AssetAmount assetAmount = HXChain::getInstance()->accountInfoMap.value(ui->accountNameLabel->text()).assetAmountMap.value(assetId);
    QString amountStr = getBigNumberString(assetAmount.amount, HXChain::getInstance()->assetInfoMap.value(assetId).precision);

    if(ui->amountLineEdit->text().toDouble() > amountStr.toDouble())
    {
        ui->amountLineEdit->setText(amountStr);
    }
}
