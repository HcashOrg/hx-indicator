#include "depositdialog.h"
#include "ui_depositdialog.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>

#include "wallet.h"
#include "../commondialog.h"
#include "checkpwddialog.h"

DepositDialog::DepositDialog(QString address, QWidget *parent) :
    QDialog(parent),
    contractAddress(address),
    ui(new Ui::DepositDialog)
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


    QRegExp rx1( QString("^([0]|[1-9][0-9]{0,18})(?:\\.\\d{0,5})?$|(^\\t?$)") );
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    init();
}

DepositDialog::~DepositDialog()
{
    delete ui;
}

void DepositDialog::pop()
{
    move(0,0);
    exec();
}

void DepositDialog::jsonDataUpdated(QString id)
{
    if( id ==  "id_wallet_transfer_to_contract+" + contractAddress)
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith( "\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of deposit has been sent!"));
            commonDialog.pop();

            close();
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText("error: " + errorMessage);
            commonDialog.pop();

            close();
        }

        return;
    }

    if( id ==  "id_wallet_transfer_to_contract_testing+" + contractAddress)
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
            QByteArray ba = utfCodec->fromUnicode(result);

            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);

            unsigned long long feeAmount = 0;
            if(json_error.error == QJsonParseError::NoError)
            {
                if( parse_doucment.isObject())
                {
                    QJsonObject jsonObject = parse_doucment.object();
                    QJsonValue resultValue = jsonObject.take("result");
                    QJsonArray array = resultValue.toArray();
                    if( array.size() != 3)  return;
                    QJsonValue amount = array.at(2).toObject().take("amount");
                    feeAmount = QString::number(amount.toInt(),'g',10).toULongLong();

                    ui->callContractFeeLabel->setText( getBigNumberString(feeAmount,ASSET_PRECISION));

                    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
                    QString account = UBChain::getInstance()->addressToName(info.admin);
                    unsigned long long balanceAmount = UBChain::getInstance()->accountBalanceMap.value(account).value(0);
                    double fee = ui->feeLabel->text().toDouble();
                    double depositAmount = ui->amountLineEdit->text().toDouble();

                    if( balanceAmount < depositAmount * ASSET_PRECISION + feeAmount + fee * ASSET_PRECISION)
                    {
                        ui->okBtn->setEnabled(false);
                    }
                    else
                    {
                        ui->okBtn->setEnabled(true);
                    }


                    checkIsFeeEnough();
                }
            }

        }

        return;
    }
}

void DepositDialog::on_okBtn_clicked()
{
    CheckPwdDialog checkPwdDialog;
    if(!checkPwdDialog.pop())   return;

    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
    QString account = UBChain::getInstance()->addressToName( info.admin);
    double amount = ui->amountLineEdit->text().toDouble();

    if( amount > 0)
    {
        QString amountStr = QString::number( amount, 'g', 15);
        UBChain::getInstance()->postRPC( "id_wallet_transfer_to_contract+" + contractAddress,
                                         toJsonFormat( "wallet_transfer_to_contract", QStringList() << amountStr
                                                       << ASSET_NAME << account << contractAddress <<  ui->callContractFeeLabel->text()
                                                       ));
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("The amount can not be 0!"));
        commonDialog.pop();
    }
}

void DepositDialog::on_cancelBtn_clicked()
{
    close();
}

void DepositDialog::init()
{
    ui->contractAddressLabel->setText(contractAddress);

    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
    ui->contractBalanceLabel->setText( getBigNumberString(info.contractBalance,ASSET_PRECISION) );

    QString account = UBChain::getInstance()->addressToName(info.admin);
    ui->balanceLabel->setText( getBigNumberString(UBChain::getInstance()->accountBalanceMap.value(account).value(0),ASSET_PRECISION) );

    ui->accountLabel->setText(account);

    QString percentStr = QString::number( info.mortgageRate.toDouble() * 100, 'f',2);
    ui->mortgageLabel->setText( percentStr + "%");

    ui->feeLabel->setText( getBigNumberString(UBChain::getInstance()->transactionFee,ASSET_PRECISION));
}

void DepositDialog::checkIsFeeEnough()
{
//    double fee = ui->feeLabel->text().toDouble();

//    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
//    unsigned long long balanceAmount = UBChain::getInstance()->accountBalanceMap.value(account).value(0);


//    QString strBalanceTemp = UBChain::getInstance()->accountBalanceMap(account).remove(",");
//    strBalanceTemp = strBalanceTemp.remove(" " + QString(ASSET_NAME));
//    double dBalance = strBalanceTemp.remove(",").toDouble();

//    if(fee + ui->callContractFeeLabel->text().toDouble() > dBalance)
//    {
//        ui->tipLabel3->show();
//        ui->tipLabel3->setText( "UB" + QString::fromLocal8Bit(" 余额不足"));
//        ui->tipLabel5->show();

//        ui->sendBtn->setEnabled(false);
//    }
//    else
//    {
//        ui->tipLabel3->hide();
//        ui->tipLabel5->hide();

//        ui->sendBtn->setEnabled(true);
//    }

}

void DepositDialog::on_amountLineEdit_textEdited(const QString &arg1)
{
    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
    QString account = UBChain::getInstance()->addressToName( info.admin);
    double amount = ui->amountLineEdit->text().toDouble();

    if( amount > 0)
    {
        QString amountStr = QString::number( amount, 'g', 15);
        UBChain::getInstance()->postRPC( "id_wallet_transfer_to_contract_testing+" + contractAddress,
                                         toJsonFormat( "wallet_transfer_to_contract_testing", QStringList() << amountStr
                                                       << ASSET_NAME << account << contractAddress
                                                       ));
    }



    if( ui->amountLineEdit->text().toDouble() > ui->balanceLabel->text().toDouble())
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void DepositDialog::on_closeBtn_clicked()
{
    close();
}
