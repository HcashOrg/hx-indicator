#include "SelectColdKeyFileDialog.h"
#include "ui_SelectColdKeyFileDialog.h"

#include "wallet.h"

SelectColdKeyFileDialog::SelectColdKeyFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectColdKeyFileDialog)
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

    checkOkBtnEnabled();
}

SelectColdKeyFileDialog::~SelectColdKeyFileDialog()
{
    delete ui;
}

void SelectColdKeyFileDialog::pop()
{
    move(0,0);
    exec();
}

void SelectColdKeyFileDialog::on_okBtn_clicked()
{
    filePath = ui->pkFileLineEdit->text();
    pwd = ui->pwdLineEdit->text();
    close();
}

void SelectColdKeyFileDialog::on_cancelBtn_clicked()
{
    close();
}

void SelectColdKeyFileDialog::on_closeBtn_clicked()
{
    close();
}

void SelectColdKeyFileDialog::on_pathBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select your private key file of the cold address"),"","(*.ckey)");
#ifdef WIN32
    file.replace("\\","/");
#endif
    ui->pkFileLineEdit->setText(file);
}

void SelectColdKeyFileDialog::on_pkFileLineEdit_textChanged(const QString &arg1)
{
    checkOkBtnEnabled();
}

void SelectColdKeyFileDialog::on_pwdLineEdit_textChanged(const QString &arg1)
{
    checkOkBtnEnabled();
}

void SelectColdKeyFileDialog::checkOkBtnEnabled()
{
    if(ui->pkFileLineEdit->text().isEmpty() || ui->pwdLineEdit->text().isEmpty())
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}
