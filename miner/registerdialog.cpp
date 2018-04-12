#include "registerdialog.h"
#include "ui_registerdialog.h"

#include "wallet.h"
#include "commondialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
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

    init();

}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::pop()
{
    move(0,0);
    exec();
}

void RegisterDialog::init()
{
    QStringList keys = UBChain::getInstance()->getUnregisteredAccounts();
    ui->accountComboBox->addItems(keys);

    ui->stackedWidget->setCurrentIndex(0);
}

void RegisterDialog::jsonDataUpdated(QString id)
{
    if( id == "id-unlock-RegisterDialog")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if(result == "\"result\":null")
        {
                UBChain::getInstance()->postRPC(  "id-register_account", toJsonFormat("register_account",
                                                            QJsonArray() << ui->accountComboBox->currentText() << true ));

        }
        else if(result.startsWith("\"error\":"))
        {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("Wrong password!") + "</font></body>" );
        }

        return;
    }

    if( id == "id-register_account")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":{"))
        {
            close();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Register transaction has been sent out!"));
            commonDialog.pop();
        }

        return;
    }
}

void RegisterDialog::on_closeBtn_clicked()
{
    close();
}

void RegisterDialog::on_okBtn_clicked()
{
    if(ui->accountComboBox->currentText().isEmpty())    return;
    if(ui->registerNameLineEdit->text().isEmpty())      return;

    ui->stackedWidget->setCurrentIndex(1);

}

void RegisterDialog::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->addressLabel->setText(UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address);
}

void RegisterDialog::on_okBtn2_clicked()
{
    if( ui->pwdLineEdit->text().isEmpty())  return;

    UBChain::getInstance()->postRPC( "id-unlock-RegisterDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));
}
