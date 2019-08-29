﻿
#include <QKeyEvent>
#include <QDebug>
#include <QFileInfo>
#ifdef WIN32
#include <windows.h>
#endif

#include "lockpage.h"
#include "ui_lockpage.h"
#include "wallet.h"

#include "commondialog.h"

LockPage::LockPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LockPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

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

    ui->syncLabel->setText("Block: " + QString::number(HXChain::getInstance()->walletInfo.blockHeight));
    QTimer* timer = new QTimer(this);
    timer->start(1000);
    connect(timer, &QTimer::timeout, [this](){
        ui->syncLabel->setText("Block: " + QString::number(HXChain::getInstance()->walletInfo.blockHeight));
    });
}

LockPage::~LockPage()
{
	
    delete ui;

	
}

void LockPage::on_enterBtn_clicked()
{
	

    if( ui->pwdLineEdit->text().isEmpty() )
    {
        ui->tipLabel->setText( tr("Empty!") );
        return;
    }

    if( ui->pwdLineEdit->text().size() < 8)
    {
        ui->tipLabel->setText(tr("At least 8 letters!"));
        ui->pwdLineEdit->clear();
        return;
    }


    HXChain::getInstance()->postRPC( "id-unlock-lockpage", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text() ), -1);

    logToFile( QStringList() << "lockpage unlock");
    emit showShadowWidget();
    ui->pwdLineEdit->setEnabled(false);

    unlockTimer->start(20000);
}


void LockPage::on_pwdLineEdit_returnPressed()
{
	

    on_enterBtn_clicked();

	
}


void LockPage::on_closeBtn_clicked()
{
    if( HXChain::getInstance()->closeToMinimize)
    {
        emit tray();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText( tr( "Sure to close the Wallet?"));
        bool choice = commonDialog.pop();

        if( choice)
        {
            emit closeWallet();
        }
        else
        {
            return;
        }

    }
}

void LockPage::on_pwdLineEdit_textChanged(const QString &arg1)
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

void LockPage::jsonDataUpdated(QString id)
{
    if( id == "id-unlock-lockpage")
    {
        unlockTimer->stop();

        emit hideShadowWidget();
        ui->pwdLineEdit->clear();
        ui->pwdLineEdit->setEnabled(true);
        ui->pwdLineEdit->setFocus();

        QString  result = HXChain::getInstance()->jsonDataValue(id);

        logToFile( QStringList() << "lockpage unlock result:" << result);


        if( result == "\"result\":null")
        {
            emit unlock();
        }
        else if( result.mid(0,8) == "\"error\":")
        {
            ui->tipLabel->setText(tr("Wrong password!"));
            ui->pwdLineEdit->clear();
        }

        return;
    }

}

void LockPage::InitWidget()
{
    InitStyle();

    ui->label_version->setText(QString("v") + WALLET_VERSION);

    unlockTimer = new QTimer(this);
    connect(unlockTimer, &QTimer::timeout, this, &LockPage::onTimer);
    unlockTimer->setSingleShot(true);
}

void LockPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/cover.png").scaled(this->size())));
    setPalette(palette);

    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(243,241,250));
    ui->label_version->setPalette(pa);

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/cover_close.png);background-repeat: no-repeat;background-position: center;border: none;}");
    ui->miniBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/cover_min.png);background-repeat: no-repeat;background-position: center;border: none;}");

    ui->enterBtn->setStyleSheet("QToolButton{font: 12px \"Microsoft YaHei UI Light\";background-color:rgb(255,255,255); border:none;border-radius:3px;color: rgb(59, 22, 136);}" \
                                );

    ui->pwdLineEdit->setStyleSheet("QLineEdit{max-height:32px;background: transparent;color: rgb(243,241,250);font: 10px \"Microsoft YaHei UI Light\";border:none;border-radius:0px;border-bottom:1px solid rgb(243,241,250);padding: 0px 10px 0px 6px;}"
                                   "QLineEdit:disabled{color: rgb(151,151,151);}");
    ui->pwdLineEdit->setPlaceholderText(tr("Password"));
    //welcome
//    ui->welcome->setPixmap(QPixmap(":/ui/wallet_ui/Welcome.png").scaled(ui->welcome->width(), ui->welcome->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

}

void LockPage::keyPressEvent(QKeyEvent *e)
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

void LockPage::paintEvent(QPaintEvent *e)
{
//    QPainter painter(this);
//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QBrush(QColor(243,241,250)));
//    painter.drawRect(0,0,228,24);
//    painter.drawPixmap(7,5,32,12,QPixmap(":/ui/wallet_ui/hx_label_logo.png").scaled(32,12,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
//    painter.drawPixmap(94,38,36,36,QPixmap(":/ui/wallet_ui/logo_center.png").scaled(36,36,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QWidget::paintEvent(e);
}

void LockPage::on_miniBtn_clicked()
{
    emit minimum();
}

void LockPage::onTimer()
{
    emit hideShadowWidget();
    ui->pwdLineEdit->setEnabled(true);
}
