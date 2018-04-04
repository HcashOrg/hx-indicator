#include "addmultisigdialog.h"
#include "ui_addmultisigdialog.h"

#include "wallet.h"
#include "../commondialog.h"

AddMultiSigDialog::AddMultiSigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMultiSigDialog)
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


    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->okBtn->setEnabled(false);

    ui->multiSigAddressLineEdit->setFocus();

}

AddMultiSigDialog::~AddMultiSigDialog()
{
    delete ui;
}

void AddMultiSigDialog::pop()
{
    move(0,0);
    exec();
}

void AddMultiSigDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id_wallet_import_multisig_account-AddMultiSigDialog+") )
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
            if(json_error.error == QJsonParseError::NoError)
            {
                if( parse_doucment.isObject())
                {

                    QJsonObject jsonObject = parse_doucment.object();
                    if( jsonObject.contains("result"))
                    {
                        QJsonValue resultValue = jsonObject.take("result");
                        if( resultValue.isObject())
                        {
                            QJsonObject resultObject = resultValue.toObject();
                            bool res = resultObject.take("result").toBool();
                            QString reason = resultObject.take("reason").toString();

                            if( res)
                            {
                                QString address = id.mid(QString("id_wallet_import_multisig_account-AddMultiSigDialog+").size());

                                QJsonArray array = resultObject.take("owners").toArray();
                                QStringList owners;
                                for(int i = 0; i < array.size(); i++)
                                {
                                    owners << array.at(i).toString();
                                }

                                UBChain::getInstance()->multiSigInfoMap[address].multiSigAddress = address;
                                UBChain::getInstance()->multiSigInfoMap[address].requires = resultObject.take("requires").toInt();
                                UBChain::getInstance()->multiSigInfoMap[address].owners = owners;

                                CommonDialog commonDialog(CommonDialog::OkOnly);
                                commonDialog.setText(tr("This multisig address has been imported!"));
                                commonDialog.pop();

                                close();
                            }
                            else
                            {
                                if( reason == "This multisig account does not exist!")
                                {
                                    CommonDialog commonDialog(CommonDialog::OkOnly);
                                    commonDialog.setText( tr("This multisig address doesn't exist!"));
                                    commonDialog.pop();
                                }
                                else
                                {
                                    CommonDialog commonDialog(CommonDialog::OkOnly);
                                    commonDialog.setText( tr("Fail to import multisig address : ") + reason);
                                    commonDialog.pop();
                                }


                            }
                        }
                    }
                }
            }

        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Fail to import multisig address : ") + errorMessage);
            commonDialog.pop();
        }

        return;
    }

}

void AddMultiSigDialog::on_okBtn_clicked()
{
    QString address = ui->multiSigAddressLineEdit->text().simplified();
    if(address.isEmpty())   return;


    UBChain::getInstance()->postRPC( "id_wallet_import_multisig_account-AddMultiSigDialog+" + address,
                                     toJsonFormat( "wallet_import_multisig_account", QStringList() << address
                                               ));
}

void AddMultiSigDialog::on_cancelBtn_clicked()
{
    close();
}

void AddMultiSigDialog::on_multiSigAddressLineEdit_textEdited(const QString &arg1)
{
    QString address = ui->multiSigAddressLineEdit->text().simplified();
    ui->multiSigAddressLineEdit->setText(address);
    if(checkAddress(address,MultiSigAddress))
    {
        ui->okBtn->setEnabled(true);
    }
    else
    {
        ui->okBtn->setEnabled(false);
    }
}

void AddMultiSigDialog::on_closeBtn_clicked()
{
     on_cancelBtn_clicked();
}
