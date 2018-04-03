#include "addcontactdialog.h"
#include "ui_addcontactdialog.h"


#include "wallet.h"
#include "dialog/checkpwddialog.h"

#include <QDebug>
#include <QPainter>


AddContactDialog::AddContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddContactDialog)
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

    ui->containerWidget->installEventFilter(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    QRegExp regx("[a-zA-Z0-9\-\.\ \n]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->addressLineEdit->setValidator( validator );

    ui->addressLineEdit->setPlaceholderText( tr("Please enter an account address."));
    ui->addressLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->addressLineEdit->setFocus();

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->okBtn->setEnabled(false);


}

AddContactDialog::~AddContactDialog()
{
    delete ui;
//    UBChain::getInstance()->removeCurrentDialogVector(this);
}

void AddContactDialog::pop()
{
//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();
}

void AddContactDialog::on_cancelBtn_clicked()
{
    close();
//    emit accepted();
}

void AddContactDialog::on_addressLineEdit_textChanged(const QString &arg1)
{
    ui->addressLineEdit->setText( ui->addressLineEdit->text().remove(" "));
    ui->addressLineEdit->setText( ui->addressLineEdit->text().remove("\n"));
    if( ui->addressLineEdit->text().isEmpty())
    {
        ui->tipLabel->setText("");
        return;
    }

    if( checkAddress(ui->addressLineEdit->text()))
    {
        ui->tipLabel->setText("");
        ui->okBtn->setEnabled(true);
        return;
    }


    QString str;
    if( ui->addressLineEdit->text().toInt() == 0)   // 防止输入数字
    {
        str = ui->addressLineEdit->text();
    }
    else
    {
        str = "WRONG";
    }


}

void AddContactDialog::jsonDataUpdated(QString id)
{

}

void AddContactDialog::on_okBtn_clicked()
{
    if( ui->addressLineEdit->text().simplified().isEmpty())
    {
        ui->tipLabel->setText(tr("can not be empty"));
        return;
    }

    if( !UBChain::getInstance()->contactsFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "contact.dat not exist";
        return;
    }

    CheckPwdDialog checkPwdDialog;
    if(!checkPwdDialog.pop())
    {
        UBChain::getInstance()->contactsFile->close();
        return;
    }

    QByteArray ba = QByteArray::fromBase64( UBChain::getInstance()->contactsFile->readAll());
    QString str(ba);

    QStringList strList = str.split(";");
    strList.removeLast();
    foreach (QString ss, strList)
    {
        if( (ss.mid(0, ss.indexOf('=')) == ui->addressLineEdit->text())
//             && ss.mid( ss.indexOf('=') + 1) ==  ui->remarkLineEdit->text()
          )
        {
            ui->tipLabel->setText(tr("Already existed"));
            UBChain::getInstance()->contactsFile->close();
            return;
        }
    }




    ba += ui->addressLineEdit->text().toUtf8() + '=' + ui->remarkLineEdit->text().toUtf8() + QString(";").toUtf8();
    ba = ba.toBase64();
    UBChain::getInstance()->contactsFile->resize(0);
    QTextStream ts(UBChain::getInstance()->contactsFile);
    ts << ba;

    UBChain::getInstance()->contactsFile->close();

    close();
//    emit accepted();
}

void AddContactDialog::on_remarkLineEdit_textChanged(const QString &arg1)
{
    QString remark = arg1;
    if( remark.contains("=") || remark.contains(";"))
    {
        remark.remove("=");
        remark.remove(";");
        ui->remarkLineEdit->setText( remark);
    }
}


bool AddContactDialog::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->containerWidget)
    {
        if( e->type() == QEvent::Paint)
        {

            return true;
        }
    }

    return QWidget::eventFilter(watched,e);
}

void AddContactDialog::on_closeBtn_clicked()
{
    on_cancelBtn_clicked();
}
