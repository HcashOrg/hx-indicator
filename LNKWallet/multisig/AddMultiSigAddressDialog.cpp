#include "AddMultiSigAddressDialog.h"
#include "ui_AddMultiSigAddressDialog.h"

#include "wallet.h"
#include "commondialog.h"

AddMultiSigAddressDialog::AddMultiSigAddressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMultiSigAddressDialog)
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
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    QRegExp regx(QString("[a-zA-Z0-9]{%1}").arg(QString(MULTISIG_ADDRESS_PREFIX).size() + 33));
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->addressLineEdit->setValidator( validator );

    ui->okBtn->setEnabled(false);
}

AddMultiSigAddressDialog::~AddMultiSigAddressDialog()
{
    delete ui;
}

void AddMultiSigAddressDialog::pop()
{
    move(0,0);
    exec();
}

void AddMultiSigAddressDialog::jsonDataUpdated(QString id)
{
    if( id == "AddMultiSigAddressDialog-get_multisig_address-" + ui->addressLineEdit->text())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":"))
        {
            multiSigAddress = ui->addressLineEdit->text();
            close();
        }
        else
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("This multi-sig address does not exist on the chain!"));
            commonDialog.pop();
        }

        return;
    }
}

void AddMultiSigAddressDialog::on_okBtn_clicked()
{
    if(checkAddress(ui->addressLineEdit->text(), MultiSigAddress) == MultiSigAddress)
    {
        HXChain::getInstance()->postRPC( "AddMultiSigAddressDialog-get_multisig_address-" + ui->addressLineEdit->text(),
                                         toJsonFormat( "get_multisig_address", QJsonArray() << ui->addressLineEdit->text() ));

    }
}

void AddMultiSigAddressDialog::on_cancelBtn_clicked()
{
    close();
}

void AddMultiSigAddressDialog::on_closeBtn_clicked()
{
    close();
}

void AddMultiSigAddressDialog::on_addressLineEdit_textEdited(const QString &arg1)
{
    if(checkAddress(ui->addressLineEdit->text(), MultiSigAddress) == MultiSigAddress)
    {
        ui->okBtn->setEnabled(true);
        ui->tipLabel->clear();
    }
    else
    {
        ui->okBtn->setEnabled(false);

        if(ui->addressLineEdit->text().isEmpty())
        {
            ui->tipLabel->clear();
        }
        else
        {
            ui->tipLabel->setText(tr("Invalid multi-sig address."));
        }
    }
}
