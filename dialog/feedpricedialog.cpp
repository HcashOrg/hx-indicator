#include "feedpricedialog.h"
#include "ui_feedpricedialog.h"

#include "lnk.h"
#include "../commondialog.h"
#include "checkpwddialog.h"

#include <QDebug>
#include <QListView>


FeedPriceDialog::FeedPriceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FeedPriceDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->feedBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->tokenComboBox->setView(new QListView());
    ui->accountComboBox->setView(new QListView());


    QRegExp rx1( QString("^([0]|[1-9][0-9]{0,7})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(QString::number(ASSET_PRECISION).size() - 1 ));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->myPriceLineEdit->setValidator(pReg1);
    ui->myPriceLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);



    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    addTokens();
    addAccounts();

}

FeedPriceDialog::~FeedPriceDialog()
{
    delete ui;
}

void FeedPriceDialog::pop()
{
    move(0,0);
    exec();
}

void FeedPriceDialog::addTokens()
{
    QStringList tokens = UBChain::getInstance()->tokenPriceMap.keys();
    ui->tokenComboBox->addItems(tokens);
}

void FeedPriceDialog::setCurrentToken(QString token)
{
    ui->tokenComboBox->setCurrentText(token);
}

void FeedPriceDialog::addAccounts()
{
    ui->accountComboBox->addItems(UBChain::getInstance()->getMyFeederAccount());
}

void FeedPriceDialog::on_feedBtn_clicked()
{
    if(ui->myPriceLineEdit->text().isEmpty())
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Please enter the price!"));
        commonDialog.pop();

        return;
    }

    QString price = ui->myPriceLineEdit->text();
    price = price.simplified();
    if(price.toDouble() < 0.00001)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("The price can not be 0!"));
        commonDialog.pop();

        return;
    }


    CheckPwdDialog checkPwdDialog;
    if(!checkPwdDialog.pop())   return;



    price = QString::number(price.toDouble() * ASSET_PRECISION,'g',15);
    UBChain::getInstance()->postRPC( QString("id_contract_call_") + FEEDER_CONTRACT_ADDRESS + "_feed_price",
                                     toJsonFormat( "contract_call",
                                     QStringList() << FEEDER_CONTRACT_ADDRESS << ui->accountComboBox->currentText()
                                                   << "feed_price" << ui->tokenComboBox->currentText() + "," + price << ASSET_NAME << "1"));
}

void FeedPriceDialog::on_cancelBtn_clicked()
{
    close();
}

void FeedPriceDialog::jsonDataUpdated(QString id)
{
    if( id == QString("id_contract_call_") + FEEDER_CONTRACT_ADDRESS + "_feed_price")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of feeding price has been sent!"));
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

    }

}

void FeedPriceDialog::on_tokenComboBox_currentIndexChanged(const QString &arg1)
{
    ui->currentPriceLabel->setText( getBigNumberString( UBChain::getInstance()->tokenPriceMap.value( arg1).toULongLong(), ASSET_PRECISION) );

}

void FeedPriceDialog::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->accountBalanceLabel->setText( getBigNumberString( UBChain::getInstance()->accountBalanceMap.value(arg1).value(0), ASSET_PRECISION) );
}

void FeedPriceDialog::on_closeBtn_clicked()
{
    on_cancelBtn_clicked();
}
