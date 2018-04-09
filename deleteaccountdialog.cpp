#include "deleteaccountdialog.h"
#include "ui_deleteaccountdialog.h"
#include "wallet.h"
#include "commondialog.h"

#include <QDebug>
#include <QFocusEvent>

DeleteAccountDialog::DeleteAccountDialog(QString name , QWidget *parent) :
    QDialog(parent),
    accountName(name),
    yesOrNo( false),
    ui(new Ui::DeleteAccountDialog)
{
    
    ui->setupUi(this);

//    UBChain::getInstance()->appendCurrentDialogVector(this);
    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);


    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));


    ui->pwdLineEdit->setPlaceholderText( tr("Login password"));
    ui->pwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);


    ui->okBtn->setText(tr("Ok"));

    ui->cancelBtn->setText(tr("Cancel"));

    ui->okBtn->setEnabled(false);

    ui->pwdLineEdit->setFocus();
}

DeleteAccountDialog::~DeleteAccountDialog()
{
    delete ui;
}

void DeleteAccountDialog::on_okBtn_clicked()
{

    if( ui->pwdLineEdit->text().isEmpty())
    {
        ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("Please enter the password!") + "</font></body>" );
        return;
    }

    UBChain::getInstance()->postRPC( "id_wallet_check_passphrase", toJsonFormat( "wallet_check_passphrase", QJsonArray() << ui->pwdLineEdit->text()
                                               ));


}

void DeleteAccountDialog::on_cancelBtn_clicked()
{
    close();
}

void DeleteAccountDialog::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
        return;
    }

    if( arg1.isEmpty())
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void DeleteAccountDialog::pop()
{
    ui->pwdLineEdit->grabKeyboard();

//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();

}

void DeleteAccountDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id_wallet_check_passphrase") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":true")
        {
            UBChain::getInstance()->postRPC( "id_wallet_account_delete", toJsonFormat( "wallet_account_delete", QJsonArray() << accountName
                                                       ));

        }
        else if( result == "\"result\":false")
        {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("Wrong password!") + "</font></body>" );
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            if( errorMessage == "password too short")
            {
                ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("At least 8 letters!") + "</font></body>" );
            }
            else
            {
                ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + errorMessage + "</font></body>" );
            }
        }

        return;
    }

    if( id == "id_wallet_account_delete" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":true")
        {
            UBChain::getInstance()->deleteAccountInConfigFile( accountName);
            UBChain::getInstance()->addressMapRemove( accountName);
            UBChain::getInstance()->balanceMapRemove( accountName);
            UBChain::getInstance()->registerMapRemove( accountName);

            close();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Account deleted."));
            commonDialog.pop();
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Delete account failed: ") + errorMessage);
            commonDialog.pop();
        }


        return;
    }
}

void DeleteAccountDialog::on_pwdLineEdit_returnPressed()
{
    on_okBtn_clicked();
}

void DeleteAccountDialog::on_closeBtn_clicked()
{
    on_cancelBtn_clicked();
}
