#include "destroydialog.h"
#include "ui_destroydialog.h"

#include <QDebug>

#include "lnk.h"
#include "../commondialog.h"
#include "checkpwddialog.h"

DestroyDialog::DestroyDialog(QString address, QWidget *parent) :
    QDialog(parent),
    contractAddress(address),
    ui(new Ui::DestroyDialog)
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

    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
    int precisionSize = QString::number(info.precision,'g',15).size() - 1;

    QRegExp rx1( QString("^([0]|[1-9][0-9]{0,18})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(precisionSize) );
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    init();
}

DestroyDialog::~DestroyDialog()
{
    delete ui;
}

void DestroyDialog::pop()
{
    move(0,0);
    exec();
}

void DestroyDialog::jsonDataUpdated(QString id)
{
    if( id ==  "id_contract_call+" + contractAddress + "+destroy")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith( "\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of destroying token has been sent!"));
            commonDialog.pop();

            close();
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            if( errorMessage == "lua lvm internal error")
            {
                int pos = result.indexOf("\"exception_msg\":\"") + 17;
                QString exceptionMsg = result.mid(pos, result.indexOf("\"}", pos) - pos);

                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("Execute contract error: ") + exceptionMsg);
                commonDialog.pop();
            }
            else
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText("error: " + errorMessage);
                commonDialog.pop();
            }


            close();
        }

        return;
    }
}

void DestroyDialog::on_okBtn_clicked()
{
    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
    QString account = UBChain::getInstance()->addressToName( info.admin);
    double amount = ui->amountLineEdit->text().toDouble();
    if( amount > 0)
    {
        CheckPwdDialog checkPwdDialog;
        if(!checkPwdDialog.pop())   return;

        QString amountStr = QString::number( amount * info.precision, 'g', 15);
        UBChain::getInstance()->postRPC( "id_contract_call+" + contractAddress + "+destroy",
                                         toJsonFormat( "contract_call", QStringList() << contractAddress
                                                       << account << "destroy" << amountStr << "UB" << "1"
                                                       ));
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("The amount can not be 0!"));
        commonDialog.pop();
    }

}

void DestroyDialog::on_cancelBtn_clicked()
{
    close();
}

void DestroyDialog::init()
{
    ui->contractAddressLabel->setText(contractAddress);

    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
    ui->contractBalanceLabel->setText( getBigNumberString(info.contractBalance,ASSET_PRECISION) + " " + ASSET_NAME);

    QString percentStr = QString::number( info.mortgageRate.toDouble() * 100, 'f',2);
    ui->mortgageLabel->setText( percentStr + "%");

    ui->assetNameLabel->setText( info.contractName);
    ui->assetNameLabel2->setText( info.contractName);

    QString account = UBChain::getInstance()->addressToName( info.admin);
    ui->balanceLabel->setText( getBigNumberString( UBChain::getInstance()->accountContractBalanceMap.value(UBChain::getInstance()->addressMap.value(account).ownerAddress).value(contractAddress), info.precision) );

    ui->accountLabel->setText(account);
}

void DestroyDialog::on_amountLineEdit_textEdited(const QString &arg1)
{
    if( ui->amountLineEdit->text().toDouble() > ui->balanceLabel->text().toDouble())
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void DestroyDialog::on_closeBtn_clicked()
{
    close();
}
