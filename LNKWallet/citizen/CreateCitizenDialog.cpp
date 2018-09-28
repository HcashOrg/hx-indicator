#include "CreateCitizenDialog.h"
#include "ui_CreateCitizenDialog.h"

#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/TransactionResultDialog.h"
#include "miner/registerdialog.h"

CreateCitizenDialog::CreateCitizenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateCitizenDialog)
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
    ui->okBtn2->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn2->setStyleSheet(CANCELBTN_STYLE);

    FeeChooseWidget *feeWidget = new FeeChooseWidget(HXChain::getInstance()->feeChargeInfo.createCitizenFee.toDouble(),
                                                     HXChain::getInstance()->feeType);
    connect(feeWidget,&FeeChooseWidget::feeSufficient,ui->okBtn,&QToolButton::setEnabled);
    ui->stackedWidget_fee->addWidget(feeWidget);
    ui->stackedWidget_fee->setCurrentIndex(0);
    ui->stackedWidget_fee->currentWidget()->resize(ui->stackedWidget_fee->size());
    init();
}

CreateCitizenDialog::~CreateCitizenDialog()
{
    delete ui;
}

void CreateCitizenDialog::pop()
{
    move(0,0);
    exec();
}

void CreateCitizenDialog::init()
{
    ui->okBtn->setEnabled(false);

    QStringList keys = HXChain::getInstance()->getRegisteredAccounts();
    QStringList miners = HXChain::getInstance()->minerMap.keys();
    QStringList guards = HXChain::getInstance()->getMyGuards();
    foreach (QString key, keys)
    {
        if(miners.contains(key) || guards.contains(key))
        {
            keys.removeAll(key);
        }
    }
    ui->accountComboBox->addItems(keys);

    ui->stackedWidget->setCurrentIndex(0);
}

void CreateCitizenDialog::jsonDataUpdated(QString id)
{
    if( id == "id-unlock-CreateCitizenDialog")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result == "\"result\":null")
        {
            HXChain::getInstance()->postRPC(  "id-create_citizen", toJsonFormat("create_citizen",
                                                                              QJsonArray() << ui->accountComboBox->currentText()
                                                                              << "" << true ));

        }
        else if(result.startsWith("\"error\":"))
        {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#EB005E>" + tr("Wrong password!") + "</font></body>" );
        }

        return;
    }

    if( id == "id-create_citizen")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":{"))
        {
            close();

            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of create-citizen has been sent out!"));
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
}

void CreateCitizenDialog::on_closeBtn_clicked()
{
    close();
}

void CreateCitizenDialog::on_okBtn_clicked()
{
    if(ui->accountComboBox->currentText().isEmpty())    return;

    ui->stackedWidget->setCurrentIndex(1);
}

void CreateCitizenDialog::on_cancelBtn_clicked()
{
    close();
}

void CreateCitizenDialog::on_registerLabel_linkActivated(const QString &link)
{
    close();

    RegisterDialog registerDialog;
    registerDialog.pop();
}

void CreateCitizenDialog::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->addressLabel->setText(HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address);
    if(FeeChooseWidget *feeWidget = qobject_cast<FeeChooseWidget*>(ui->stackedWidget_fee->currentWidget()))
    {
        feeWidget->updateAccountNameSlots(ui->accountComboBox->currentText(),true);
    }
}

void CreateCitizenDialog::on_okBtn2_clicked()
{
    if( ui->pwdLineEdit->text().isEmpty())  return;

    HXChain::getInstance()->postRPC( "id-unlock-CreateCitizenDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));
}

void CreateCitizenDialog::on_pwdLineEdit_textEdited(const QString &arg1)
{
    ui->tipLabel2->clear();
}
