#include "ColdHotTransferDialog.h"
#include "ui_ColdHotTransferDialog.h"

#include "wallet.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"

ColdHotTransferDialog::ColdHotTransferDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColdHotTransferDialog)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CANCELBTN_STYLE);

    init();
}

ColdHotTransferDialog::~ColdHotTransferDialog()
{
    delete ui;
}

void ColdHotTransferDialog::pop()
{
    move(0,0);
    exec();
}

void ColdHotTransferDialog::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->getMyFormalGuards();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

    QStringList assetIds = HXChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        if(assetId == "1.3.0")  continue;
        ui->assetComboBox->addItem(HXChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
    }

    QIntValidator *validator = new QIntValidator(1, 60 * 60 * 24 * 365,this);
    ui->timeLineEdit->setValidator(validator);

    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

}

void ColdHotTransferDialog::jsonDataUpdated(QString id)
{
    if( id == "id-transfer_from_cold_to_hot")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result.startsWith("\"result\":{"))             // 成功
        {
            close();

            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of cold-hot-trx proposal has been sent out. You can check the voting state in the proposal page."));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Failed!"));
            errorResultDialog.setDetailText(result);
        }

        return;

    }
}

void ColdHotTransferDialog::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object().value("result").toObject();
    QString assetSymbol = object.take("chainId").toString().toUpper();
    if(assetSymbol != ui->assetComboBox->currentText())     return;

    AssetInfo info = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(assetSymbol));
//    queriedBalance = jsonValueToDouble( object.value("balance"));

//    qDebug() << assetSymbol << info.symbol << queriedBalance << info.precision << QString::number( roundDown( queriedBalance, info.precision), 'f', info.precision);

    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(info.precision));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->clear();

    // ETH 小数精度有18位 小数点8位以后去掉
    if(object.value("balance").isString())
    {
        QString balanceStr = object.value("balance").toString();
        if(balanceStr.contains("."))
        {
            QStringList strList = balanceStr.split(".");
            if(strList.size() == 2)
            {
                QString str = strList.at(1);
                if(str.size() > info.precision)  str = str.left(info.precision);
                balanceStr = strList.at(0) + "." + str;
            }
        }
        queriedBalance = balanceStr.toDouble();
        ui->amountLineEdit->setPlaceholderText(tr("max:") + balanceStr);
    }
    else
    {
        queriedBalance = object.value("balance").toDouble();
        ui->amountLineEdit->setPlaceholderText(tr("max:") + QString::number(roundDown( queriedBalance, info.precision), 'f', info.precision));
    }

}

void ColdHotTransferDialog::on_okBtn_clicked()
{
    if(ui->accountComboBox->currentText().isEmpty() || ui->fromLineEdit->text().isEmpty() || ui->toLineEdit->text().isEmpty()
            || ui->amountLineEdit->text().toDouble() <= 0 || ui->timeLineEdit->text().toInt() <= 0)
        return;

    HXChain::getInstance()->postRPC( "id-transfer_from_cold_to_hot", toJsonFormat( "transfer_from_cold_to_hot",
                                     QJsonArray() << ui->accountComboBox->currentText() << ui->fromLineEdit->text()
                                     << ui->toLineEdit->text() << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
                                     << "" << ui->timeLineEdit->text().toInt() << true));
}

void ColdHotTransferDialog::on_closeBtn_clicked()
{
    close();
}

void ColdHotTransferDialog::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    showAddresses();
}

void ColdHotTransferDialog::on_typeComboBox_currentIndexChanged(const QString &arg1)
{
    showAddresses();
}

void ColdHotTransferDialog::showAddresses()
{
    AssetInfo info = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(ui->assetComboBox->currentText()));

    ui->amountLineEdit->clear();
    ui->amountLineEdit->setPlaceholderText("");
    if(ui->typeComboBox->currentIndex() == 0)
    {
        ui->fromLineEdit->setText(info.coldAddress);
        ui->toLineEdit->setText(info.hotAddress);
        ui->fromLineEdit->setReadOnly(true);
        ui->toLineEdit->setReadOnly(true);
    }
    else if(ui->typeComboBox->currentIndex() == 1)
    {
        ui->fromLineEdit->setText(info.hotAddress);
        ui->toLineEdit->setText(info.coldAddress);
        ui->fromLineEdit->setReadOnly(true);
        ui->toLineEdit->setReadOnly(true);
    }
    else
    {
        ui->fromLineEdit->clear();
        ui->toLineEdit->clear();
        ui->fromLineEdit->setReadOnly(false);
        ui->toLineEdit->setReadOnly(false);
    }

    queryMultisigBalance();
}

void ColdHotTransferDialog::queryMultisigBalance()
{
    if(ui->assetComboBox->currentText().isEmpty() || ui->fromLineEdit->text().isEmpty()) return;
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","Zchain.Address.GetBalance");
    QJsonObject paramObject;
    paramObject.insert("chainId",ui->assetComboBox->currentText());
    paramObject.insert("addr",ui->fromLineEdit->text());
    object.insert("params",paramObject);
    httpManager.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());
}

void ColdHotTransferDialog::checkOkBtnEnable()
{
    if(ui->accountComboBox->currentText().isEmpty() || ui->fromLineEdit->text().isEmpty() || ui->toLineEdit->text().isEmpty()
            || ui->amountLineEdit->text().toDouble() <= 0 || ui->timeLineEdit->text().toInt() <= 0)
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void ColdHotTransferDialog::on_amountLineEdit_textEdited(const QString &arg1)
{
    AssetInfo info = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(ui->assetComboBox->currentText()));
    if(ui->amountLineEdit->text().toDouble() > queriedBalance)
    {
        ui->amountLineEdit->setText(QString::number(queriedBalance, 'f', info.precision));
    }
}

void ColdHotTransferDialog::on_fromLineEdit_textEdited(const QString &arg1)
{
    queryMultisigBalance();
}
