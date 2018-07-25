#include "WithdrawOrderDialog.h"
#include "ui_WithdrawOrderDialog.h"

#include "wallet.h"
#include "FeeChooseWidget.h"

WithdrawOrderDialog::WithdrawOrderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WithdrawOrderDialog)
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

    feeChoose = new FeeChooseWidget(0,HXChain::getInstance()->feeType);
    ui->stackedWidget->addWidget(feeChoose);
    feeChoose->resize(ui->stackedWidget->size());
    ui->stackedWidget->setCurrentIndex(0);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

}

WithdrawOrderDialog::~WithdrawOrderDialog()
{
    delete ui;
}

bool WithdrawOrderDialog::pop()
{
    move(0,0);
    exec();

    return yesOrNo;
}

void WithdrawOrderDialog::setText(QString _text)
{
    ui->textLabel->setText(_text);
}

void WithdrawOrderDialog::setContractFee(double _fee)
{
    feeChoose->updateFeeNumberSlots(_fee);
}

void WithdrawOrderDialog::setAccountName(const QString &accountName)
{
    feeChoose->updateAccountNameSlots(accountName,true);
}

void WithdrawOrderDialog::updatePoundageID()
{
    feeChoose->updatePoundageID();
}

void WithdrawOrderDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id-unlock-WithdrawOrderDialog") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result == "\"result\":null")
        {
            yesOrNo = true;
            close();
        }
        else if(result.startsWith("\"error\":"))
        {
            ui->tipLabel->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("Wrong password!") + "</font></body>" );
        }

        return;
    }
}

void WithdrawOrderDialog::on_okBtn_clicked()
{
    if(ui->pwdLineEdit->text().isEmpty())           return;

    HXChain::getInstance()->postRPC( "id-unlock-WithdrawOrderDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));
    feeChoose->updatePoundageID();

}

void WithdrawOrderDialog::on_cancelBtn_clicked()
{
    yesOrNo = false;
    close();
}

void WithdrawOrderDialog::on_closeBtn_clicked()
{
    close();
}
