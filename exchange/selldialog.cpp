#include "selldialog.h"
#include "ui_selldialog.h"

#include "wallet.h"
#include "commondialog.h"

SellDialog::SellDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SellDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);


    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->sellAmountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->buyAmountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    init();
}

SellDialog::~SellDialog()
{
    delete ui;
}

void SellDialog::pop()
{
    move(0,0);
    exec();
}

void SellDialog::init()
{
    ui->accountNameLabel->setText(UBChain::getInstance()->currentAccount);

    QStringList assetIds = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
        ui->assetComboBox2->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
    }
}

void SellDialog::setSellAsset(QString _assetSymbol)
{
    ui->assetComboBox->setCurrentText(_assetSymbol);
}

void SellDialog::setBuyAsset(QString _assetSymbol)
{
    ui->assetComboBox2->setCurrentText(_assetSymbol);
}

void SellDialog::jsonDataUpdated(QString id)
{
    if( id == "id-invoke_contract-putOnSellOrder")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            close();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of sell-order has been sent out!"));
            commonDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Put on sell-order failed: " + errorMessage );
            commonDialog.pop();
        }

        return;
    }
}

void SellDialog::on_okBtn_clicked()
{
    if(ui->sellAmountLineEdit->text().toDouble() <= 0)  return;
    if(ui->buyAmountLineEdit->text().toDouble() <= 0)  return;

    QString contractAddress = UBChain::getInstance()->getExchangeContractAddress(ui->accountNameLabel->text());

    AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(ui->assetComboBox->currentData().toString());
    AssetInfo assetInfo2 = UBChain::getInstance()->assetInfoMap.value(ui->assetComboBox2->currentData().toString());

    QString params = QString("%1,%2,%3,%4").arg(ui->assetComboBox->currentText()).arg(decimalToIntegerStr(ui->sellAmountLineEdit->text(), assetInfo.precision))
            .arg(ui->assetComboBox2->currentText()).arg(decimalToIntegerStr(ui->buyAmountLineEdit->text(), assetInfo2.precision));
    UBChain::getInstance()->postRPC( "id-invoke_contract-putOnSellOrder", toJsonFormat( "invoke_contract",
                                                                           QJsonArray() << ui->accountNameLabel->text() << 0.001 << 1000
                                                                           << contractAddress
                                                                           << "putOnSellOrder"  << params));
}

void SellDialog::on_cancelBtn_clicked()
{
    close();
}


void SellDialog::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    QString balanceStr = UBChain::getInstance()->getAccountBalance(ui->accountNameLabel->text(), ui->assetComboBox->currentText());

    ui->sellAmountLineEdit->setPlaceholderText(tr("Total %1 %2").arg(balanceStr).arg(ui->assetComboBox->currentText()));

    AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(ui->assetComboBox->currentText()));

    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(assetInfo.precision));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->sellAmountLineEdit->setValidator(pReg1);
    ui->sellAmountLineEdit->clear();
}

void SellDialog::on_assetComboBox2_currentIndexChanged(const QString &arg1)
{
    AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(ui->assetComboBox2->currentText()));

    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(assetInfo.precision));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->buyAmountLineEdit->setValidator(pReg1);
    ui->buyAmountLineEdit->clear();
}
