#include "WithdrawOrderDialog.h"
#include "ui_WithdrawOrderDialog.h"

#include "wallet.h"
#include "FeeChooseWidget.h"

WithdrawOrderDialog::WithdrawOrderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WithdrawOrderDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->closeBtn->setIconSize(QSize(12,12));
    ui->closeBtn->setIcon(QIcon(":/ui/wallet_ui/close.png"));
    ui->closeBtn->setStyleSheet("QToolButton{background-color:transparent;border:none;}"
                                "QToolButton:hover{background-color:rgb(208,228,255);}");

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);

    feeChoose = new FeeChooseWidget(0,"LNK");
    ui->stackedWidget->addWidget(feeChoose);
    ui->stackedWidget->setCurrentIndex(0);

    ui->feeLabel->setVisible(false);
    ui->label_3->setVisible(false);
    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

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

void WithdrawOrderDialog::setContractFee(QString _feeStr)
{
    ui->feeLabel->setText(_feeStr);
}

void WithdrawOrderDialog::setContractFee(double _fee)
{
    feeChoose->updateFeeNumberSlots(_fee);
}

void WithdrawOrderDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id-unlock-WithdrawOrderDialog") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
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

    UBChain::getInstance()->postRPC( "id-unlock-WithdrawOrderDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));

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
