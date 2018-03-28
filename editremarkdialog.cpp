#include "editremarkdialog.h"
#include "ui_editremarkdialog.h"
#include "lnk.h"

EditRemarkDialog::EditRemarkDialog(QString remark, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditRemarkDialog)
{
    ui->setupUi(this);

//    UBChain::getInstance()->appendCurrentDialogVector(this);
    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->remarkLineEdit->setText( remark);
    ui->remarkLineEdit->setFocus();
}

EditRemarkDialog::~EditRemarkDialog()
{
    delete ui;
//    UBChain::getInstance()->removeCurrentDialogVector(this);
}

void EditRemarkDialog::on_okBtn_clicked()
{
    close();
//    emit accepted();
}

QString EditRemarkDialog::pop()
{
//    QEventLoop loop;
//    show();
//    ui->remarkLineEdit->grabKeyboard();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();

    return ui->remarkLineEdit->text();
}

void EditRemarkDialog::on_remarkLineEdit_textChanged(const QString &arg1)
{
    QString remark = arg1;
    if( remark.contains("=") || remark.contains(";"))
    {
        remark.remove("=");
        remark.remove(";");
        ui->remarkLineEdit->setText( remark);
    }
}

void EditRemarkDialog::on_closeBtn_clicked()
{
    close();
}
