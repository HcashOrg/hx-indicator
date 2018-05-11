#include "depositexchangecontractdialog.h"
#include "ui_depositexchangecontractdialog.h"

#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"

DepositExchangeContractDialog::DepositExchangeContractDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DepositExchangeContractDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->closeBtn->setIconSize(QSize(12,12));
    ui->closeBtn->setIcon(QIcon(":/ui/wallet_ui/close.png"));
    ui->closeBtn->setStyleSheet("QToolButton{background-color:transparent;border:none;}"
                                "QToolButton:hover{background-color:rgb(208,228,255);}");

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);

    feeChoose = new FeeChooseWidget(0,"LNK");
    ui->stackedWidget->addWidget(feeChoose);
    ui->stackedWidget->setCurrentIndex(0);


    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->label_3->setVisible(false);
    ui->contractFeeLabel->setVisible(false);
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
    ui->accountNameLabel->setText(UBChain::getInstance()->currentAccount);

    QStringList assetIds = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
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
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result == "\"result\":null")
        {
            QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountNameLabel->text());

            UBChain::getInstance()->postRPC( "id-transfer_to_contract", toJsonFormat( "transfer_to_contract",
                                                                                   QJsonArray() << ui->accountNameLabel->text() << contractAddress
                                                                                   << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
                                                                                   << "deposit to exchange contract"
                                                                                   << UBChain::getInstance()->currentContractFee() << stepCount
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
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            close();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of deposit has been sent out!"));
            commonDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Deposit to exchange contract failed: " + errorMessage );
            commonDialog.pop();
        }

        return;
    }

    if( id == "id-transfer_to_contract_testing-DepositExchangeContractDialog")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            stepCount = result.mid(QString("\"result\":").size()).toInt();

            feeChoose->updateFeeNumberSlots(getBigNumberString(stepCount * UBChain::getInstance()->contractFee, ASSET_PRECISION).toDouble());
            ui->contractFeeLabel->setText(getBigNumberString(stepCount * UBChain::getInstance()->contractFee, ASSET_PRECISION)
                                          + " " + ASSET_NAME);
        }


        return;
    }
}

void DepositExchangeContractDialog::on_okBtn_clicked()
{
    if(ui->amountLineEdit->text().toDouble() <= 0)  return;
    if(ui->pwdLineEdit->text().isEmpty())           return;

    UBChain::getInstance()->postRPC( "id-unlock-DepositExchangeContractDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));

}

void DepositExchangeContractDialog::on_cancelBtn_clicked()
{
    close();
}

void DepositExchangeContractDialog::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    AssetAmountMap map = UBChain::getInstance()->accountInfoMap.value(ui->accountNameLabel->text()).assetAmountMap;
    AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(ui->assetComboBox->currentText()));

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
    QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountNameLabel->text());

    UBChain::getInstance()->postRPC( "id-transfer_to_contract_testing-DepositExchangeContractDialog", toJsonFormat( "transfer_to_contract_testing",
                                                                           QJsonArray() << ui->accountNameLabel->text() << contractAddress
                                                                           << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
                                                                           << "deposit to exchange contract"
                                                                           ));

    qDebug() <<toJsonFormat( "transfer_to_contract_testing",
                             QJsonArray() << ui->accountNameLabel->text() << contractAddress
                             << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
                             << "deposit to exchange contract"
                             );

}

void DepositExchangeContractDialog::on_amountLineEdit_textChanged(const QString &arg1)
{
    estimateContractFee();
}
