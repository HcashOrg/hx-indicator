#include <QDebug>
#include <QPainter>
#include <QKeyEvent>
#ifdef WIN32
#include <windows.h>
#endif

#include "firstlogin.h"
#include "ui_firstlogin.h"
#include "wallet.h"
#include "debug_log.h"
#include "commondialog.h"

FirstLogin::FirstLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FirstLogin)
{
    ui->setupUi(this);

    connect(UBChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    setStyleSheet("FirstLogin{background-image:url(:/ui/wallet_ui/loginBg.png);}");


#ifdef TARGET_OS_MAC
    ui->pwdLineEdit->setAttribute(Qt::WA_MacShowFocusRect,false);
    ui->pwdLineEdit2->setAttribute(Qt::WA_MacShowFocusRect,false);
#endif
    ui->pwdLineEdit->setFocus();
    ui->pwdLineEdit->installEventFilter(this);
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit2->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->pwdLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->pwdLineEdit->setStyleSheet("color:white;background:transparent;border-width:0;border-style:outset;");
    ui->pwdLineEdit2->setStyleSheet("color:white;background:transparent;border-width:0;border-style:outset;");

    ui->logoLabel->setPixmap(QPixmap(":/ui/wallet_ui/logo_52x34.png"));
    ui->label->setPixmap(QPixmap(":/ui/wallet_ui/passwordLineEditBg.png"));

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/closeBtn.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/ui/wallet_ui/closeBtn_hover.png);}");

//    ui->correctLabel->setPixmap( QPixmap(":/pic/pic2/correct2.png"));
//    ui->correctLabel2->setPixmap( QPixmap(":/pic/pic2/correct2.png"));
//    ui->correctLabel->hide();
//    ui->correctLabel2->hide();


//    ui->tipLabel2->setPixmap(QPixmap(":/pic/pic2/pwdTip_En.png"));
//    ui->tipLabel2->setGeometry(602,148,319,124);

#ifdef WIN32
    if( GetKeyState(VK_CAPITAL) )
    {
        ui->tipLabel->setText( tr("Caps lock opened!") );
    }
#endif

}

FirstLogin::~FirstLogin()
{
    delete ui;

}

void FirstLogin::on_createBtn_clicked()
{

    if( ui->pwdLineEdit->text().isEmpty() || ui->pwdLineEdit2->text().isEmpty() )
    {
        ui->tipLabel->setText(tr("Empty!"));
        return;
    }

    if( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text())
    {
        if( ui->pwdLineEdit->text().size() < 8)
        {
            ui->tipLabel->setText(tr("Too short! At least 8 bits."));
            return;
        }

        emit showShadowWidget();
        ui->pwdLineEdit->setEnabled(false);
        ui->pwdLineEdit2->setEnabled(false);

//        RpcThread* rpcThread = new RpcThread;
//        connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//        rpcThread->setLogin("a","b");
//        rpcThread->setWriteData( toJsonFormat( "id_create", "create", QStringList() << "wallet" << ui->pwdLineEdit->text() ));
//        rpcThread->start();

        UBChain::getInstance()->postRPC( "id-set_password", toJsonFormat( "set_password", QStringList() <<  ui->pwdLineEdit->text() ));
    }
    else
    {
        ui->tipLabel->setText(tr("Not consistent!"));
        return;
    }

}


void FirstLogin::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(e);

}



void FirstLogin::on_pwdLineEdit2_returnPressed()
{
    on_createBtn_clicked();
}


void FirstLogin::on_closeBtn_clicked()
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

//bool FirstLogin::eventFilter(QObject *watched, QEvent *e)
//{
//    if(watched == ui->pwdLineEdit)
//    {
//        if(e->type() == QEvent::FocusIn)
//        {
//            ui->tipLabel2->show();
//        }
//        else if( e->type() == QEvent::FocusOut)
//        {
//            ui->tipLabel2->hide();
//        }
//    }

//    return QWidget::eventFilter(watched,e);

//}

void FirstLogin::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
        return;
    }

    ui->tipLabel->clear();

//    if( arg1.size() > 7 )
//    {
//        ui->correctLabel->show();

//        if( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text() )
//        {
//            ui->correctLabel2->show();
//        }
//        else
//        {
//            ui->correctLabel2->hide();
//        }
//    }
//    else
//    {
//        ui->correctLabel->hide();
//        ui->correctLabel2->hide();
//    }

}

void FirstLogin::on_pwdLineEdit2_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit2->setText( ui->pwdLineEdit2->text().remove(' '));
        return;
    }

    ui->tipLabel->clear();

//    if( arg1.size() > 7 && ( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text() ) )
//    {
//        ui->correctLabel2->show();
//    }
//    else
//    {
//        ui->correctLabel2->hide();
//    }
}


void FirstLogin::jsonDataUpdated(QString id)
{
    if( id == "id-set_password")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << "id-set_password" << result;
        if( result == "\"result\":null" )
        {
            UBChain::getInstance()->postRPC( "id-unlock-firstlogin", toJsonFormat( "unlock", QStringList() << ui->pwdLineEdit->text() ));
            return;
        }
        else
        {
            emit hideShadowWidget();
            return;
        }

    }

    if( id == "id-unlock-firstlogin")
    {
        emit hideShadowWidget();
        ui->pwdLineEdit->setEnabled(true);
        ui->pwdLineEdit2->setEnabled(true);

        QString result = UBChain::getInstance()->jsonDataValue(id);


        qDebug() << id << result;
        if( result == "\"result\":null")
        {
            emit login();
            this->close();
        }
        return;
    }

}

void FirstLogin::keyPressEvent(QKeyEvent *e)
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
