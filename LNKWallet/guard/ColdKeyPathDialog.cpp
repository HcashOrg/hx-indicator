#include "ColdKeyPathDialog.h"
#include "ui_ColdKeyPathDialog.h"

#include "wallet.h"

ColdKeyPathDialog::ColdKeyPathDialog(QString _accountName, QWidget *parent) :
    QDialog(parent),
    accountName(_accountName),
    ui(new Ui::ColdKeyPathDialog)
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

ColdKeyPathDialog::~ColdKeyPathDialog()
{
    delete ui;
}

void ColdKeyPathDialog::pop()
{
    move(0,0);
    exec();
}

void ColdKeyPathDialog::on_okBtn_clicked()
{
    filePath = ui->pkFileLineEdit->text();
    pwd = ui->pwdLineEdit->text();
    close();
}

void ColdKeyPathDialog::on_cancelBtn_clicked()
{
    close();
}

void ColdKeyPathDialog::on_closeBtn_clicked()
{
    close();
}

void ColdKeyPathDialog::on_pathBtn_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this,tr( "Select the path"), QDir::currentPath());
    if( !file.isEmpty())
    {
#ifdef WIN32
        file.replace("\\","/");
#endif
        ui->pkFileLineEdit->setText( file + "/"  + accountName + ".ckey");
    }
}
