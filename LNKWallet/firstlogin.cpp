#include <QDebug>

#include <QKeyEvent>
#ifdef WIN32
#include <windows.h>
#endif

#include "firstlogin.h"
#include "ui_firstlogin.h"
#include "wallet.h"

#include "commondialog.h"

FirstLogin::FirstLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FirstLogin)
{
    ui->setupUi(this);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    InitWidget();
    //setStyleSheet("FirstLogin{background-image:url(:/ui/wallet_ui/loginBg.png);}");


#ifdef TARGET_OS_MAC
    ui->pwdLineEdit->setAttribute(Qt::WA_MacShowFocusRect,false);
    ui->pwdLineEdit2->setAttribute(Qt::WA_MacShowFocusRect,false);
#endif
    //ui->pwdLineEdit->setFocus();
    ui->pwdLineEdit->installEventFilter(this);
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit2->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->pwdLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);

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
//        rpcThread->setWriteData( toJsonFormat( "id_create", "create", QJsonArray() << "wallet" << ui->pwdLineEdit->text() ));
//        rpcThread->start();

        HXChain::getInstance()->postRPC( "id-set_password", toJsonFormat( "set_password", QJsonArray() <<  ui->pwdLineEdit->text() ));
    }
    else
    {
        ui->tipLabel->setText(tr("Not consistent!"));
        return;
    }

}


void FirstLogin::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(243,241,250)));
    painter.drawRect(0,0,228,24);
    painter.drawPixmap(7,5,32,12,QPixmap(":/ui/wallet_ui/hx_label_logo.png").scaled(32,12,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    painter.drawPixmap(94,38,36,36,QPixmap(":/ui/wallet_ui/logo_center.png").scaled(36,36,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));


    QWidget::paintEvent(e);

}



void FirstLogin::on_pwdLineEdit2_returnPressed()
{
    on_createBtn_clicked();
}


void FirstLogin::on_closeBtn_clicked()
{
//    if( HXChain::getInstance()->closeToMinimize)
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
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << "id-set_password" << result;
        if( result == "\"result\":null" )
        {
            HXChain::getInstance()->postRPC( "id-unlock-firstlogin", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text() ));
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

        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")
        {
            emit login();
            this->close();
        }
        return;
    }

}

void FirstLogin::InitWidget()
{
    InitStyle();

    ui->label_version->setText(QString("v") + WALLET_VERSION);
}

void FirstLogin::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/login_back.png").scaled(this->size())));
    setPalette(palette);


    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(243,241,250));
    ui->label_version->setPalette(pa);

    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->createBtn->setStyleSheet(CANCELBTN_STYLE);
    //welcome
    ui->welcome->setPixmap(QPixmap(":/ui/wallet_ui/Welcome.png").scaled(ui->welcome->width(), ui->welcome->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

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
