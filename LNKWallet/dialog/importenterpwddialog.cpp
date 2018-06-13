#include "importenterpwddialog.h"
#include "ui_importenterpwddialog.h"

#include <QDebug>

#include "wallet.h"
#include "../commondialog.h"

ImportEnterPwdDialog::ImportEnterPwdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportEnterPwdDialog)
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

    ui->pkPwdLineEdit->setMaxLength(14);

    ui->pkPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pkPwdLineEdit->setFocus();

    ui->okBtn->setEnabled(false);
}

ImportEnterPwdDialog::~ImportEnterPwdDialog()
{
    delete ui;
}

bool ImportEnterPwdDialog::pop()
{
    move(0,0);
    exec();

    return yesOrNO;
}

void ImportEnterPwdDialog::on_okBtn_clicked()
{
    if(ui->pkPwdLineEdit->text().isEmpty())     return;
    pwd = ui->pkPwdLineEdit->text();
    yesOrNO = true;
    close();
}

void ImportEnterPwdDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

void ImportEnterPwdDialog::on_pkPwdLineEdit_textEdited(const QString &arg1)
{
    checkOkBtn();
}

void ImportEnterPwdDialog::checkOkBtn()
{
    if( ui->pkPwdLineEdit->text().size() < 8 || ui->pkPwdLineEdit->text().size() > 14)
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    ui->okBtn->setEnabled(true);
}

void ImportEnterPwdDialog::on_closeBtn_clicked()
{
    close();
}
