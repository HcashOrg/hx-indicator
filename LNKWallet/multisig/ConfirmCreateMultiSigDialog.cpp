#include "ConfirmCreateMultiSigDialog.h"
#include "ui_ConfirmCreateMultiSigDialog.h"

#include "wallet.h"
#include "FeeChooseWidget.h"

ConfirmCreateMultiSigDialog::ConfirmCreateMultiSigDialog(QString account, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfirmCreateMultiSigDialog)
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
    ui->closeBtn->setStyleSheet(CANCELBTN_STYLE);




    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);
    ui->accountComboBox->setCurrentText(account);

    feeWidget = new FeeChooseWidget( 0.002,HXChain::getInstance()->feeType,
                                                 ui->accountComboBox->currentText(), ui->containerWidget);
    connect(feeWidget,&FeeChooseWidget::feeSufficient,ui->okBtn,&QToolButton::setEnabled);
    feeWidget->move(30,120);
    feeWidget->resize(260,120);

    connect(ui->accountComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAccountComboBoxCurrentIndexChanged(QString)));
    onAccountComboBoxCurrentIndexChanged(ui->accountComboBox->currentText());
}

ConfirmCreateMultiSigDialog::~ConfirmCreateMultiSigDialog()
{
    delete ui;
}

void ConfirmCreateMultiSigDialog::pop()
{
    move(0,0);
    exec();
}

void ConfirmCreateMultiSigDialog::jsonDataUpdated(QString id)
{
    if( id == "ConfirmCreateMultiSigDialog-unlock")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result == "\"result\":null")
        {


        }
        else if(result.startsWith("\"error\":"))
        {
            ui->tipLabel->setText("<body><font style=\"font-size:12px\" color=#EB005E>" + tr("Wrong password!") + "</font></body>" );
        }
        return;
    }
}

void ConfirmCreateMultiSigDialog::on_okBtn_clicked()
{
    if( ui->pwdLineEdit->text().isEmpty())  return;

    HXChain::getInstance()->postRPC( "ConfirmCreateMultiSigDialog-unlock", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));
}

void ConfirmCreateMultiSigDialog::on_closeBtn_clicked()
{
    close();
}

void ConfirmCreateMultiSigDialog::on_pwdLineEdit_textChanged(const QString &arg1)
{
    ui->tipLabel->clear();
}

void ConfirmCreateMultiSigDialog::onAccountComboBoxCurrentIndexChanged(const QString &arg1)
{
    feeWidget->updateAccountNameSlots(ui->accountComboBox->currentText(), true);
}
