#include "depositexchangecontractdialog.h"
#include "ui_depositexchangecontractdialog.h"

#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/TransactionResultDialog.h"

DepositExchangeContractDialog::DepositExchangeContractDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DepositExchangeContractDialog)
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

    feeChoose = new FeeChooseWidget(0,HXChain::getInstance()->feeType);
    ui->stackedWidget->addWidget(feeChoose);
    feeChoose->resize(ui->stackedWidget->size());
    ui->stackedWidget->setCurrentIndex(0);


    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    init();
}

DepositExchangeContractDialog::~DepositExchangeContractDialog()
{
    delete ui;
}

void DepositExchangeContractDialog::pop()
{
    move(0,0);
    exec();
}

void DepositExchangeContractDialog::init()
{
    ui->accountNameLabel->setText(HXChain::getInstance()->currentAccount);

    QStringList assetIds = HXChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        ui->assetComboBox->addItem(HXChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
    }

}

void DepositExchangeContractDialog::setCurrentAsset(QString _assetSymbol)
{
    ui->assetComboBox->setCurrentText(_assetSymbol);
}

void DepositExchangeContractDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id-unlock-DepositExchangeContractDialog") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result == "\"result\":null")
        {
            QString contractAddress = HXChain::getInstance()->getExchangeContractAddress(ui->accountNameLabel->text());

            feeChoose->updatePoundageID();
            HXChain::getInstance()->postRPC( "id-transfer_to_contract", toJsonFormat( "transfer_to_contract",
                                                                                   QJsonArray() << ui->accountNameLabel->text() << contractAddress
                                                                                   << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
                                                                                   << "deposit to exchange contract"
                                                                                   << HXChain::getInstance()->currentContractFee() << stepCount
                                                                                   << true
                                                                                   ));

        }
        else if(result.startsWith("\"error\":"))
        {
            ui->tipLabel->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("Wrong password!") + "</font></body>" );
        }

        return;
    }


    if( id == "id-transfer_to_contract")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            close();

            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of deposit has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {            
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Fail to deposit to the contract!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        return;
    }

    if( id == "id-transfer_to_contract_testing-DepositExchangeContractDialog")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            HXChain::TotalContractFee totalFee = HXChain::getInstance()->parseTotalContractFee(result);
            stepCount = totalFee.step;
            unsigned long long totalAmount = totalFee.baseAmount + ceil(totalFee.step * HXChain::getInstance()->contractFee / 100.0);

            feeChoose->updateFeeNumberSlots(getBigNumberString(totalAmount, ASSET_PRECISION).toDouble());
            feeChoose->updateAccountNameSlots(ui->accountNameLabel->text(),true);
        }


        return;
    }
}

void DepositExchangeContractDialog::on_okBtn_clicked()
{
    if(!HXChain::getInstance()->ValidateOnChainOperation())     return;

    if(ui->amountLineEdit->text().toDouble() <= 0)  return;
    if(ui->pwdLineEdit->text().isEmpty())           return;

    HXChain::getInstance()->postRPC( "id-unlock-DepositExchangeContractDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));

}

void DepositExchangeContractDialog::on_cancelBtn_clicked()
{
    close();
}

void DepositExchangeContractDialog::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    AssetAmountMap map = HXChain::getInstance()->accountInfoMap.value(ui->accountNameLabel->text()).assetAmountMap;
    AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(ui->assetComboBox->currentText()));

    ui->balanceLabel->setText(getBigNumberString(map.value(assetInfo.id).amount, assetInfo.precision) + " " + assetInfo.symbol );

    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(assetInfo.precision));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->clear();
}

void DepositExchangeContractDialog::on_closeBtn_clicked()
{
    close();
}

void DepositExchangeContractDialog::estimateContractFee()
{
    QString contractAddress = HXChain::getInstance()->getExchangeContractAddress(ui->accountNameLabel->text());

    HXChain::getInstance()->postRPC( "id-transfer_to_contract_testing-DepositExchangeContractDialog", toJsonFormat( "transfer_to_contract_testing",
                                                                           QJsonArray() << ui->accountNameLabel->text() << contractAddress
                                                                           << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
                                                                           << "deposit to exchange contract"
                                                                           ));


}

void DepositExchangeContractDialog::on_amountLineEdit_textChanged(const QString &arg1)
{
    estimateContractFee();
}
