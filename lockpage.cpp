
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

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

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


    UBChain::getInstance()->postRPC( "id-unlock-lockpage", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text() ));
 qDebug() << "id_unlock_lockpage" ;
    emit showShadowWidget();
    ui->pwdLineEdit->setEnabled(false);

	
}


void LockPage::on_pwdLineEdit_returnPressed()
{
	

    on_enterBtn_clicked();

	
}


void LockPage::on_closeBtn_clicked()
{
    if( UBChain::getInstance()->closeToMinimize)
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
        emit hideShadowWidget();
        ui->pwdLineEdit->clear();
        ui->pwdLineEdit->setEnabled(true);
        ui->pwdLineEdit->setFocus();

        QString  result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

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
}

void LockPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/background.png").scaled(this->size())));
    setPalette(palette);

    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_wel->setPalette(pa);
    ui->label_bloc->setPalette(pa);
    ui->label_version->setPalette(pa);

    ui->pwdLineEdit->setStyleSheet("color:blue;background:transparent;border-width:0;border-style:outset;lineedit-password-mask-delay: 1000;");

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/closeBtn.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/ui/wallet_ui/closeBtn_hover.png);}");
    ui->enterBtn->setStyleSheet("QToolButton{background-color:#5474EB; border:none;border-radius:15px;color: rgb(255, 255, 255);}"
                                "QToolButton:hover{background-color:#00D2FF;}");
    ui->forgetBtn->setStyleSheet("QToolButton{background-color:transparent;border:none;border-radius:15px;color: #C6CAD4;}"
                                 "QToolButton:hover{background-color:#00D2FF;}");
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
    QPainter painter(this);
    painter.drawPixmap(255,150,450,280,QPixmap(":/ui/wallet_ui/login.png").scaled(450,280));

    QWidget::paintEvent(e);
}
