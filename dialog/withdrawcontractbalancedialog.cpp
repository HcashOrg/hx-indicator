#include "withdrawcontractbalancedialog.h"
#include "ui_withdrawcontractbalancedialog.h"

#include <QDebug>

#include "lnk.h"
#include "../commondialog.h"

WithdrawContractBalanceDialog::WithdrawContractBalanceDialog(QString address, QWidget *parent) :
    QDialog(parent),
    contractAddress(address),
    ui(new Ui::WithdrawContractBalanceDialog)
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

    int precisionSize = QString::number(ASSET_PRECISION,'g',15).size() - 1;

    QRegExp rx1( QString("^([0]|[1-9][0-9]{0,11})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(precisionSize) );
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    init();
}

WithdrawContractBalanceDialog::~WithdrawContractBalanceDialog()
{
    delete ui;
}

void WithdrawContractBalanceDialog::pop()
{
    move(0,0);
    exec();
}

void WithdrawContractBalanceDialog::jsonDataUpdated(QString id)
{
    if( id ==  "id_contract_call+" + contractAddress + "+withdraw_unused")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith( "\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of withdrawing has been sent!"));
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

void WithdrawContractBalanceDialog::on_okBtn_clicked()
{
    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
    QString account = UBChain::getInstance()->addressToName( info.admin);
    double amount = ui->amountLineEdit->text().toDouble();
    if( amount > 0)
    {
        QString amountStr = QString::number( amount * ASSET_PRECISION, 'g', 15);
        UBChain::getInstance()->postRPC( "id_contract_call+" + contractAddress + "+withdraw_unused",
                                         toJsonFormat( "contract_call", QStringList() << contractAddress
                                                       << account << "withdraw_unused" << amountStr << "UB" << "1"
                                                       ));
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("The amount can not be 0!"));
        commonDialog.pop();
    }

}

void WithdrawContractBalanceDialog::on_cancelBtn_clicked()
{
    close();
}

void WithdrawContractBalanceDialog::init()
{
    ui->contractAddressLabel->setText(contractAddress);

    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
    ui->contractBalanceLabel->setText( getBigNumberString(info.contractBalance,ASSET_PRECISION));

    QString account = UBChain::getInstance()->addressToName(info.admin);
    ui->accountLabel->setText(account);
}

void WithdrawContractBalanceDialog::on_amountLineEdit_textEdited(const QString &arg1)
{

    if( ui->amountLineEdit->text().toDouble() > ui->contractBalanceLabel->text().toDouble())
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void WithdrawContractBalanceDialog::on_closeBtn_clicked()
{
    close();
}
