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

    connect(UBChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    InitWidget();
    //setStyleSheet("FirstLogin{background-image:url(:/ui/wallet_ui/loginBg.png);}");


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

        UBChain::getInstance()->postRPC( "id-set_password", toJsonFormat( "set_password", QJsonArray() <<  ui->pwdLineEdit->text() ));
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

    QPainter painter(this);
    painter.drawPixmap(238,130,490,320,QPixmap(":/ui/wallet_ui/whitebord.png").scaled(490,320));


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
            UBChain::getInstance()->postRPC( "id-unlock-firstlogin", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text() ));
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
}

void FirstLogin::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/background.png").scaled(this->size())));
    setPalette(palette);

    QFont font("黑体",14,70);
    ui->label_wel->setFont(font);

    QFont fontb("黑体",21,100);
    ui->label_bloc->setFont(fontb);

    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_wel->setPalette(pa);
    ui->label_bloc->setPalette(pa);
    ui->pwdLineEdit->setStyleSheet("color:blue;background:transparent;border-width:0;border-style:outset;lineedit-password-mask-delay: 1000;");
    ui->pwdLineEdit2->setStyleSheet("color:blue;background:transparent;border-width:0;border-style:outset;lineedit-password-mask-delay: 1000;");
    ui->closeBtn->setIconSize(QSize(12,12));
    ui->closeBtn->setIcon(QIcon(":/ui/wallet_ui/close.png"));
    ui->closeBtn->setStyleSheet("QToolButton{background-color:transparent;border:none;}"
                                "QToolButton:hover{background-color:rgb(208,228,255);}");
    ui->createBtn->setStyleSheet("QToolButton{background-color:#5474EB; border:none;border-radius:15px;color: rgb(255, 255, 255);}"
                                "QToolButton:hover{background-color:#00D2FF;}");

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
