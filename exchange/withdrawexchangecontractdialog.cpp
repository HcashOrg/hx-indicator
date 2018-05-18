#include "withdrawexchangecontractdialog.h"
#include "ui_withdrawexchangecontractdialog.h"

#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"

WithdrawExchangeContractDialog::WithdrawExchangeContractDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WithdrawExchangeContractDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    feeChoose = new FeeChooseWidget(0,UBChain::getInstance()->feeType);
    ui->stackedWidget->addWidget(feeChoose);
    ui->stackedWidget->setCurrentIndex(0);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    init();
}

WithdrawExchangeContractDialog::~WithdrawExchangeContractDialog()
{
    delete ui;
}

void WithdrawExchangeContractDialog::pop()
{
    move(0,0);
    exec();
}

void WithdrawExchangeContractDialog::init()
{
    ui->accountNameLabel->setText(UBChain::getInstance()->currentAccount);

    QStringList assetIds = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
    }
}

void WithdrawExchangeContractDialog::setCurrentAsset(QString _assetSymbol)
{
    ui->assetComboBox->setCurrentText(_assetSymbol);
}

void WithdrawExchangeContractDialog::jsonDataUpdated(QString id)
{

    if( id.startsWith( "id-unlock-WithdrawExchangeContractDialog") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result == "\"result\":null")
        {
            AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(ui->assetComboBox->currentText()));
            QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountNameLabel->text());

            QString params = QString("%1,%2").arg(ui->assetComboBox->currentText())
                    .arg(decimalToIntegerStr(ui->amountLineEdit->text(), assetInfo.precision));

            UBChain::getInstance()->postRPC( "id-invoke_contract-withdrawAsset", toJsonFormat( "invoke_contract",
                                                                                   QJsonArray() << ui->accountNameLabel->text()
                                                                                   << UBChain::getInstance()->currentContractFee() << stepCount
                                                                                   << contractAddress
                                                                                   << "withdrawAsset"  << params));
        }
        else if(result.startsWith("\"error\":"))
        {
            ui->tipLabel->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("Wrong password!") + "</font></body>" );
        }

        return;
    }

    if( id == "id-invoke_contract-withdrawAsset")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            close();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of withdraw from the exchange contract has been sent out!"));
            commonDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Withdraw from the exchange contract failed: " + errorMessage );
            commonDialog.pop();
        }

        return;
    }

    if( id == "id-invoke_contract_testing-withdrawAsset")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            UBChain::TotalContractFee totalFee = UBChain::getInstance()->parseTotalContractFee(result);
            stepCount = totalFee.step;
            unsigned long long totalAmount = totalFee.baseAmount + totalFee.step * UBChain::getInstance()->contractFee;

            feeChoose->updateFeeNumberSlots(getBigNumberString(totalAmount, ASSET_PRECISION).toDouble());
        }

        return;
    }
}

void WithdrawExchangeContractDialog::on_okBtn_clicked()
{
    if(ui->amountLineEdit->text().toDouble() <= 0)  return;
    if(ui->pwdLineEdit->text().isEmpty())          return;

    UBChain::getInstance()->postRPC( "id-unlock-WithdrawExchangeContractDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));
}

void WithdrawExchangeContractDialog::on_cancelBtn_clicked()
{
    close();
}

void WithdrawExchangeContractDialog::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    AssetAmountMap map = UBChain::getInstance()->accountInfoMap.value(ui->accountNameLabel->text()).assetAmountMap;
    AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(ui->assetComboBox->currentText()));


    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(assetInfo.precision));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->clear();

    unsigned long long maxAmount = UBChain::getInstance()->accountExchangeContractBalancesMap.value(ui->accountNameLabel->text()).value(ui->assetComboBox->currentText());
    ui->amountLineEdit->setPlaceholderText(tr("Max: %1 %2").arg(getBigNumberString(maxAmount, assetInfo.precision)).arg(assetInfo.symbol));
}

void WithdrawExchangeContractDialog::on_withdrawAllBtn_clicked()
{
    unsigned long long maxAmount = UBChain::getInstance()->accountExchangeContractBalancesMap.value(ui->accountNameLabel->text()).value(ui->assetComboBox->currentText());
    AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(ui->assetComboBox->currentText()));

    ui->amountLineEdit->setText(getBigNumberString(maxAmount, assetInfo.precision));
}

void WithdrawExchangeContractDialog::on_closeBtn_clicked()
{
    close();
}

void WithdrawExchangeContractDialog::estimateContractFee()
{
    AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(ui->assetComboBox->currentText()));
    QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountNameLabel->text());

    QString params = QString("%1,%2").arg(ui->assetComboBox->currentText())
            .arg(decimalToIntegerStr(ui->amountLineEdit->text(), assetInfo.precision));

    UBChain::getInstance()->postRPC( "id-invoke_contract_testing-withdrawAsset", toJsonFormat( "invoke_contract_testing",
                                                                           QJsonArray() << ui->accountNameLabel->text()
                                                                           << contractAddress
                                                                           << "withdrawAsset"  << params));


}

void WithdrawExchangeContractDialog::on_amountLineEdit_textChanged(const QString &arg1)
{
    estimateContractFee();
}
