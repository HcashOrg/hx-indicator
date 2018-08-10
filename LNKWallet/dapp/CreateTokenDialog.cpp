#include "CreateTokenDialog.h"
#include "ui_CreateTokenDialog.h"

#include "wallet.h"
#include "FeeChooseWidget.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"

CreateTokenDialog::CreateTokenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateTokenDialog)
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

    QRegExp rx1("[a-zA-Z0-9]{0,20}");
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->tokenNameLineEdit->setValidator(pReg1);

    QRegExp rx2("[a-zA-Z0-9]{0,8}");
    QRegExpValidator *pReg2 = new QRegExpValidator(rx2, this);
    ui->tokenSymbolLineEdit->setValidator(pReg2);

    init();
}

CreateTokenDialog::~CreateTokenDialog()
{
    delete ui;
}

void CreateTokenDialog::pop()
{
    move(0,0);
    exec();
}

void CreateTokenDialog::init()
{
    ui->stackedWidget->setCurrentIndex(0);

    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

    QFileInfo fileInfo(QDir::currentPath() + "/contracts/token.lua.gpc");
    qDebug() << fileInfo.exists();
    if(!fileInfo.exists())
    {
        return;
    }
    ui->gpcPathLineEdit->setText(fileInfo.absoluteFilePath());
    ui->gpcPathLineEdit->setEnabled(false);
    HXChain::getInstance()->postRPC("CreateTokenDialog-register_contract_testing",
                                    toJsonFormat( "register_contract_testing",
                                    QJsonArray() << ui->accountComboBox->currentText() << ui->gpcPathLineEdit->text()));

    ui->initBtn->setEnabled(false);

}

void CreateTokenDialog::jsonDataUpdated(QString id)
{
    if( id == "CreateTokenDialog-register_contract_testing")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            HXChain::TotalContractFee totalFee = HXChain::getInstance()->parseTotalContractFee(result);
            stepCount = totalFee.step;

            unsigned long long totalAmount = totalFee.baseAmount + ceil(totalFee.step * HXChain::getInstance()->contractFee / 100.0);

            qDebug() << totalAmount;

            registerFeeWidget = new FeeChooseWidget( getBigNumberString(totalAmount, ASSET_PRECISION).toDouble(),HXChain::getInstance()->feeType,
                                                         ui->accountComboBox->currentText(), ui->registerContractPage);
            registerFeeWidget->move(50,120);
            registerFeeWidget->show();

            //fee->SetTitle(tr("Register Contract"));
        }
        else
        {
//            CommonDialog dia(CommonDialog::OkOnly);
//            dia.setText(result);
//            dia.pop();
        }

        return;
    }

    if( id == "CreateTokenDialog-register_contract")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith(QString("\"result\":")))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject object = parse_doucment.object().value("result").toObject();
            QString registeredContractId = object.value("contract_id").toString();
            QString trxId = object.value("trxid").toString();

            ui->stackedWidget->setCurrentIndex(1);
            ui->contractIdLabel->setText(registeredContractId);
            ui->trxIdLabel->setText(trxId);

            timerForRegister = new QTimer(this);
            connect(timerForRegister,&QTimer::timeout,[this](){
                HXChain::getInstance()->postRPC( "CreateTokenDialog-get_transaction-" + this->ui->trxIdLabel->text(),
                                                 toJsonFormat( "get_transaction",
                                                               QJsonArray() << this->ui->trxIdLabel->text()));

            });
            timerForRegister->start(5000);

        }
        return;
    }

    if( id.startsWith("CreateTokenDialog-get_transaction-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith(QString("\"result\":")))
        {
            // 如果注册交易已被确认
            timerForRegister->stop();
            ui->stackedWidget->setCurrentIndex(2);

            initFeeWidget = new FeeChooseWidget( getBigNumberString(0, ASSET_PRECISION).toDouble(),HXChain::getInstance()->feeType,
                                                         ui->accountComboBox->currentText(), ui->initPage);
            initFeeWidget->setGeometry(50,160,280,150);
            initFeeWidget->show();
        }

        return;
    }

    if( id.startsWith("CreateTokenDialog-invoke_contract_testing-init_token-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":"))
        {
            HXChain::TotalContractFee totalFee = HXChain::getInstance()->parseTotalContractFee(result);
            stepCount = totalFee.step;
            unsigned long long totalAmount = totalFee.baseAmount + ceil(totalFee.step * HXChain::getInstance()->contractFee / 100.0);

            initFeeWidget->updateFeeNumberSlots(getBigNumberString(totalAmount, ASSET_PRECISION).toDouble());
            ui->initBtn->setEnabled(true);
        }
        else
        {
            initFeeWidget->updateFeeNumberSlots(0);
            ui->initBtn->setEnabled(false);

        }

        return;
    }

    if( id.startsWith("CreateTokenDialog-invoke_contract-init_token-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":"))
        {
            HXChain::getInstance()->configFile->setValue("/contractTokens/" + ui->contractIdLabel->text(), 1);

            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of invoking contract function \"init_token\" has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();

            close();
        }
        else if(result.startsWith("\"error\":"))
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Initializing token failed!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        return;
    }
}

void CreateTokenDialog::on_registerBtn_clicked()
{
    if(!registerFeeWidget)     return;
    registerFeeWidget->updatePoundageID();
    HXChain::getInstance()->postRPC( "CreateTokenDialog-register_contract", toJsonFormat( "register_contract",
                                                                           QJsonArray() << ui->accountComboBox->currentText() << HXChain::getInstance()->currentContractFee()
                                                                           << stepCount  << ui->gpcPathLineEdit->text()));
}

void CreateTokenDialog::on_closeBtn_clicked()
{
    close();
}

void CreateTokenDialog::calculateInitFee()
{
    QString totalSupply = addPrecisionString(ui->totalSupplyLineEdit->text(),ui->precisionSpinBox->text().toInt());
    QString precision  = intToPrecisionString(ui->precisionSpinBox->text().toInt());
    QString params = QString("%1,%2,%3,%4").arg(ui->tokenNameLineEdit->text()).arg(ui->tokenSymbolLineEdit->text())
                                     .arg(totalSupply).arg(precision);

    HXChain::getInstance()->postRPC( "CreateTokenDialog-invoke_contract_testing-init_token-" + params, toJsonFormat( "invoke_contract_testing",
                                     QJsonArray() << ui->accountComboBox->currentText()
                                     << ui->contractIdLabel->text()
                                     << "init_token"  << params));

}

QString CreateTokenDialog::intToPrecisionString(int precision)
{
    QString result = "1";
    while (precision > 0)
    {
        result.append("0");
        precision--;
    }

    return result;
}

QString CreateTokenDialog::addPrecisionString(QString supply, int precision)
{
    QString result = supply;
    while (precision > 0)
    {
        result.append("0");
        precision--;
    }

    return result;
}

void CreateTokenDialog::on_tokenNameLineEdit_textEdited(const QString &arg1)
{
    calculateInitFee();
}

void CreateTokenDialog::on_tokenSymbolLineEdit_textEdited(const QString &arg1)
{
    calculateInitFee();
}

void CreateTokenDialog::on_precisionSpinBox_valueChanged(int arg1)
{
    setTotalSupplyValidator();
    calculateInitFee();
}

void CreateTokenDialog::on_totalSupplyLineEdit_textEdited(const QString &arg1)
{
    calculateInitFee();
}

void CreateTokenDialog::setTotalSupplyValidator()
{
    int precisionNum = ui->precisionSpinBox->text().toInt();

    QRegExp rx(QString("^([1-9][0-9]{0,%1})?$|(^\\t?$)").arg(17 - precisionNum));
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    ui->totalSupplyLineEdit->setValidator(validator);

}

void CreateTokenDialog::on_initBtn_clicked()
{
    QString totalSupply = addPrecisionString(ui->totalSupplyLineEdit->text(),ui->precisionSpinBox->text().toInt());
    QString precision  = intToPrecisionString(ui->precisionSpinBox->text().toInt());
    QString params = QString("%1,%2,%3,%4").arg(ui->tokenNameLineEdit->text()).arg(ui->tokenSymbolLineEdit->text())
                                     .arg(totalSupply).arg(precision);

    HXChain::getInstance()->postRPC( "CreateTokenDialog-invoke_contract-init_token-" + params, toJsonFormat( "invoke_contract",
                                     QJsonArray() << ui->accountComboBox->currentText()
                                      << HXChain::getInstance()->currentContractFee() << stepCount
                                     << ui->contractIdLabel->text()
                                     << "init_token"  << params
                                     << true));

}
