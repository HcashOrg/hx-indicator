#include "registerdialog.h"
#include "ui_registerdialog.h"

#include <QPainter>
#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/TransactionResultDialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

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
    ui->okBtn2->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn2->setStyleSheet(CANCELBTN_STYLE);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->stackedWidget_fee->addWidget(new FeeChooseWidget(HXChain::getInstance()->feeChargeInfo.minerRegisterFee.toDouble(),
                                                         HXChain::getInstance()->feeType));
    ui->stackedWidget_fee->setCurrentIndex(0);
    ui->stackedWidget_fee->currentWidget()->resize(ui->stackedWidget_fee->size());
    init();
    ui->registerNameLineEdit->setMaxLength(63);
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
    ui->okBtn->setEnabled(false);

    QStringList keys = HXChain::getInstance()->getUnregisteredAccounts();
    ui->accountComboBox->addItems(keys);

    ui->stackedWidget->setCurrentIndex(0);
}

void RegisterDialog::jsonDataUpdated(QString id)
{
    if( id == "id-unlock-RegisterDialog")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result == "\"result\":null")
        {
            if(ui->accountComboBox->currentText() != ui->registerNameLineEdit->text())
            {
                // 如果注册的名字和当前名字不一样 先改为注册的名字
                HXChain::getInstance()->postRPC(  "id-change_account_name", toJsonFormat("change_account_name",
                                               QJsonArray() << ui->accountComboBox->currentText() << ui->registerNameLineEdit->text() ));
            }
            else
            {
                HXChain::getInstance()->postRPC(  "id-register_account", toJsonFormat("register_account",
                                                                                      QJsonArray() << ui->accountComboBox->currentText() << true ));
            }

        }
        else if(result.startsWith("\"error\":"))
        {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#EB005E>" + tr("Wrong password!") + "</font></body>" );
        }

        return;
    }

    if( id == "id-change_account_name")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":{\"id\":\"0.0.0\""))
        {
            HXChain::getInstance()->postRPC(  "id-register_account", toJsonFormat("register_account",
                                                                                  QJsonArray() << ui->registerNameLineEdit->text() << true ));
        }

        return;
    }

    if( id == "id-register_account")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":{"))
        {
            close();

            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of register-account has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setDetailText(result);

            if(result.contains("Insufficient Balance:"))
            {
                errorResultDialog.setInfoText(tr("Balance of this account is not enough!"));
            }
            else
            {                
                errorResultDialog.setInfoText(tr("Fail to register account!"));
            }

            errorResultDialog.pop();
        }

        return;
    }

    if( id.startsWith("id-get_account-") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        // 如果跟当前输入框中的内容不一样，则是过时的rpc返回，不用处理
        if( id.mid(QString("id-get_account-").size()) != ui->registerNameLineEdit->text())  return;
;

        if( result.startsWith("\"result\":{\"id\":\"0.0.0\""))
        {
            ui->okBtn->setEnabled(true);
            ui->tipLabel->setText("<body><font style=\"font-size:12px\" color=#543D89>" + tr( "The name is available") + "</font></body>" );
        }
        else
        {
            ui->okBtn->setEnabled(false);
            ui->tipLabel->setText("<body><font style=\"font-size:12px\" color=#EB005E>" + tr( "This name has been used") + "</font></body>" );
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

    if(ui->accountComboBox->currentText() != ui->registerNameLineEdit->text())
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText(tr("The name that will be registered is not the same as current name. Sure to change the name of this account?"));
        if(commonDialog.pop())
        {
            ui->stackedWidget->setCurrentIndex(1);
        }
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(1);
    }

}

void RegisterDialog::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->addressLabel->setText(HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address);
    ui->registerNameLineEdit->setText(ui->accountComboBox->currentText());
    if(FeeChooseWidget *feeWidget = qobject_cast<FeeChooseWidget*>(ui->stackedWidget_fee->currentWidget()))
    {
        feeWidget->updateAccountNameSlots(ui->accountComboBox->currentText(),true);
    }
}

void RegisterDialog::on_okBtn2_clicked()
{
    if( ui->pwdLineEdit->text().isEmpty())  return;

    HXChain::getInstance()->postRPC( "id-unlock-RegisterDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));
}

void RegisterDialog::on_cancelBtn_clicked()
{
    close();
}

void RegisterDialog::on_registerNameLineEdit_textChanged(const QString &arg1)
{
    HXChain::getInstance()->postRPC( "id-get_account-" + ui->registerNameLineEdit->text(),
                                     toJsonFormat( "get_account", QJsonArray() << ui->registerNameLineEdit->text() ));
}

void RegisterDialog::on_cancelBtn2_clicked()
{
    close();
}

void RegisterDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //painter.drawPixmap(ui->containerWidget->rect().topLeft(),QPixmap(":/ui/wallet_ui/whitebord.png").scaled(ui->containerWidget->size()*1.2));
    QDialog::paintEvent(event);
}
