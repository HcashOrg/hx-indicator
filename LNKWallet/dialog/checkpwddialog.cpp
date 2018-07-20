#include "checkpwddialog.h"
#include "ui_checkpwddialog.h"

#include "wallet.h"
#include "../commondialog.h"


CheckPwdDialog::CheckPwdDialog(QWidget *parent) :
    QDialog(parent),
    yesOrNo(false),
    ui(new Ui::CheckPwdDialog)
{
    ui->setupUi(this);

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);


    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->pwdLineEdit->setFocus();

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->okBtn->setEnabled(false);

}

CheckPwdDialog::~CheckPwdDialog()
{
    delete ui;
}

bool CheckPwdDialog::pop()
{
    move(0,0);
    exec();

    return yesOrNo;
}

void CheckPwdDialog::jsonDataUpdated(QString id)
{
    if( id == "id-unlock-CheckPwdDialog")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")
        {
            yesOrNo = true;
            close();
        }
        else
        {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("Wrong password!") + "</font></body>" );
        }

        return;
    }
}

void CheckPwdDialog::on_okBtn_clicked()
{
    if( ui->pwdLineEdit->text().isEmpty())
    {
        return;
    }

    if(!ui->okBtn->isEnabled())     return;

    HXChain::getInstance()->postRPC( "id-unlock-CheckPwdDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));
}

void CheckPwdDialog::on_cancelBtn_clicked()
{
    yesOrNo = false;
    close();
}

void CheckPwdDialog::on_pwdLineEdit_textEdited(const QString &arg1)
{
    if( ui->pwdLineEdit->text().size() >= 8)
    {
        ui->okBtn->setEnabled(true);
    }
    else
    {
        ui->okBtn->setEnabled(false);
    }

    ui->tipLabel2->clear();
}

void CheckPwdDialog::on_pwdLineEdit_returnPressed()
{
    on_okBtn_clicked();
}

void CheckPwdDialog::on_closeBtn_clicked()
{
    on_cancelBtn_clicked();
}
