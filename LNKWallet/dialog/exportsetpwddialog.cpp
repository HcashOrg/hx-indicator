#include "exportsetpwddialog.h"
#include "ui_exportsetpwddialog.h"

#include <QDebug>

#include "wallet.h"
#include "../commondialog.h"

ExportSetPwdDialog::ExportSetPwdDialog(QWidget *parent) :
    QDialog(parent),
    yesOrNO(false),
    ui(new Ui::ExportSetPwdDialog)
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

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->pkPwdLineEdit->setMaxLength(14);
    ui->confirmPwdLineEdit->setMaxLength(14);

    ui->pkPwdLineEdit->setPlaceholderText( tr("Set the password from 8 to 14 digits"));
    ui->pkPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pkPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->pkPwdLineEdit->setFocus();
    ui->pkPwdLineEdit->grabKeyboard();          // 不知道为什么可能输入不了  先这么处理
    ui->pkPwdLineEdit->releaseKeyboard();

    ui->confirmPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->confirmPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);


    ui->okBtn->setEnabled(false);

}

ExportSetPwdDialog::~ExportSetPwdDialog()
{
    delete ui;
}

bool ExportSetPwdDialog::pop()
{
    move(0,0);
    exec();

    return yesOrNO;
}

void ExportSetPwdDialog::jsonDataUpdated(QString id)
{


}

void ExportSetPwdDialog::on_okBtn_clicked()
{
    if(ui->pkPwdLineEdit->text().isEmpty())     return;
    if(ui->pkPwdLineEdit->text() != ui->confirmPwdLineEdit->text())     return;
    pwd = ui->pkPwdLineEdit->text();
    yesOrNO = true;
    close();
}

void ExportSetPwdDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

void ExportSetPwdDialog::on_pkPwdLineEdit_textEdited(const QString &arg1)
{
    checkOkBtn();
}

void ExportSetPwdDialog::on_confirmPwdLineEdit_textEdited(const QString &arg1)
{
    checkOkBtn();
}

void ExportSetPwdDialog::checkOkBtn()
{
    if( ui->pkPwdLineEdit->text() != ui->confirmPwdLineEdit->text())
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    if( ui->pkPwdLineEdit->text().size() < 8 || ui->pkPwdLineEdit->text().size() > 14)
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    ui->okBtn->setEnabled(true);
}

void ExportSetPwdDialog::on_closeBtn_clicked()
{
    close();
}
