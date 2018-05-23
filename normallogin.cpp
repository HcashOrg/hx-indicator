#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#ifdef WIN32
#include <windows.h>
#endif

#include "normallogin.h"
#include "ui_normallogin.h"
#include "wallet.h"

#include "commondialog.h"

NormalLogin::NormalLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalLogin)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(pwdConfirmed(QString)));

    InitWidget();

#ifdef TARGET_OS_MAC
    ui->pwdLineEdit->setAttribute(Qt::WA_MacShowFocusRect,false);
#endif	
    ui->pwdLineEdit->setFocus();
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

#ifdef WIN32
    if( GetKeyState(VK_CAPITAL) )
    {
        ui->tipLabel->setText( tr("Caps lock opened!") );
    }
#endif



}

NormalLogin::~NormalLogin()
{
    delete ui;
}


void NormalLogin::on_enterBtn_clicked()
{


    if( ui->pwdLineEdit->text().isEmpty() )
    {
        ui->tipLabel->setText( tr("Empty!") );
        return;
    }

    if( ui->pwdLineEdit->text().size() < 8)
    {
//        ui->tipLabel->setText( QString::fromLocal8Bit("密码至少为8位！") );
        ui->tipLabel->setText( tr("At least 8 letters!") );
        ui->pwdLineEdit->clear();
        return;
    }

    UBChain::getInstance()->postRPC( "id-unlock-normallogin", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text() ));

    emit showShadowWidget();
    ui->pwdLineEdit->setEnabled(false);


}


void NormalLogin::on_pwdLineEdit_returnPressed()
{
    on_enterBtn_clicked();
}


void NormalLogin::on_closeBtn_clicked()
{
//    if( UBChain::getInstance()->closeToMinimize)
//    {
//        emit tray();
//    }
//    else
//    {
//        CommonDialog commonDialog(CommonDialog::OkAndCancel);
//        commonDialog.setText( tr( "Sure to close the Wallet?"));
//        bool choice = commonDialog.pop();

//        if( choice)
//        {
            emit closeWallet();
//        }
//        else
//        {
//            return;
//        }

//    }
}

void NormalLogin::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( !arg1.isEmpty())
    {
        ui->tipLabel->clear();
    }

    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
    }
}

void NormalLogin::pwdConfirmed(QString id)
{
    if( id == "id-unlock-normallogin" )
    {

        emit hideShadowWidget();
        ui->pwdLineEdit->setEnabled(true);
        ui->pwdLineEdit->setFocus();
        QString result = UBChain::getInstance()->jsonDataValue( id);

        if( result == "\"result\":null")
        {
            emit login();
            this->close();
        }
        else
        {
            ui->tipLabel->setText(tr("Wrong password!"));
            ui->pwdLineEdit->clear();
        }

        return;
    }
}

void NormalLogin::InitWidget()
{
    InitStyle();
}

void NormalLogin::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/background.png").scaled(this->size())));
    setPalette(palette);

    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_version->setPalette(pa);

    ui->line->setVisible(false);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->enterBtn->setStyleSheet("QToolButton{background-color:#5474EB; border:none;border-radius:15px;color: rgb(255, 255, 255);}"
                                "QToolButton:hover{background-color:#00D2FF;}");
}


void NormalLogin::keyPressEvent(QKeyEvent *e)
{
#ifdef WIN32
    if( e->key() == Qt::Key_CapsLock)
    {
        if( GetKeyState(VK_CAPITAL) == -127 )  // 不知道为什么跟构造函数中同样的调用返回的short不一样
        {
            ui->tipLabel->setText( tr("Caps lock opened!") );
        }
        else
        {

            ui->tipLabel->setText( tr("Caps lock closed!") );
        }

    }
#endif

    QWidget::keyPressEvent(e);
}

void NormalLogin::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(238,130,490,320,QPixmap(":/ui/wallet_ui/login.png").scaled(490,320));

    QWidget::paintEvent(e);
}
