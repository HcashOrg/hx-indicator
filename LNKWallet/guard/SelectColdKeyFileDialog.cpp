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
    QString file = QFileDialog::getOpenFileName(this, tr("Choose your private key file."),"","(*.ckey)");
#ifdef WIN32
    file.replace("\\","/");
#endif
    ui->pkFileLineEdit->setText(file);
}
