#include "waitingforsync.h"
#include "ui_waitingforsync.h"
#include "wallet.h"

#include "commondialog.h"
#include "websocketmanager.h"
#include "wallet.h"

#include <QTimer>
#include <QDebug>
#include <QMovie>
#include <QScrollBar>
#include <QDesktopServices>


WaitingForSync::WaitingForSync(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaitingForSync)
{
    ui->setupUi(this);

    InitWidget();

    rotateNum = 0;
    timerForPic = new QTimer(this);
    connect(timerForPic,SIGNAL(timeout()),this,SLOT(showPic()));
    timerForPic->start(25);


    timerForWSConnected = new QTimer(this);
    connect(timerForWSConnected,SIGNAL(timeout()),this,SLOT(checkConnected()));
    timerForWSConnected->start(1000);


    connect(HXChain::getInstance(),&HXChain::exeOutputMessage,this,&WaitingForSync::outputMessage);

//    HXChain::getInstance()->initWorkerThreadManager();
//    connect(HXChain::getInstance()->workerManager,SIGNAL(allConnected()),this,SIGNAL(sync()));
}

WaitingForSync::~WaitingForSync()
{
    delete ui;
}


void WaitingForSync::on_closeBtn_clicked()
{
    qDebug()<<"waitforclose";
    emit closeWallet();
}

void WaitingForSync::outputMessage(const QString &message)
{
    if(!message.isEmpty())
    {
        ui->waitSync->append(message);
        QScrollBar *scrollbar = ui->waitSync->verticalScrollBar();
        if(scrollbar)
        {
            scrollbar->setSliderPosition(scrollbar->maximum());
        }

    }
}

void WaitingForSync::checkConnected()
{
    if(HXChain::getInstance()->wsManager == NULL)   return;
    qDebug() << "wsManager isConnected: " << HXChain::getInstance()->wsManager->isConnected;
    if(HXChain::getInstance()->wsManager->isConnected)      emit sync();
}



void WaitingForSync::showPic()
{
    rotateNum += 9;
    if(rotateNum >= 360)    rotateNum -= 360;

    QMatrix matrix;
    matrix.rotate(rotateNum);

}

void WaitingForSync::InitWidget()
{
    InitStyle();

    ui->label_version->setText(QString("v") + WALLET_VERSION);
}

void WaitingForSync::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/cover.png").scaled(this->size())));
    setPalette(palette);

    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(243,241,250));
    ui->label_version->setPalette(pa);

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/cover_close.png);background-repeat: no-repeat;background-position: center;border: none;}");

    ui->waitSync->setStyleSheet("QTextEdit#waitSync{background-color: transparent;color: rgb(255,255,255);\
                                        font: 14px \"Microsoft YaHei UI Light\";border:none;padding: 0px 10px 0px 6px;}");
}

// 比较版本号 若 a > b返回 1, a = b返回 0, a < b 返回 -1
int compareVersion( QString a, QString b)
{
    if( a == b)  return 0;

    QStringList aList = a.split(".");
    QStringList bList = b.split(".");

    if( aList.at(0).toInt() > bList.at(0).toInt() )
    {
        return 1;
    }
    else if( aList.at(0) < bList.at(0))
    {
        return -1;
    }
    else
    {
        if( aList.at(1).toInt() > bList.at(1).toInt() )
        {
            return 1;
        }
        else if( aList.at(1) < bList.at(1))
        {
            return -1;
        }
        else
        {
            if( aList.at(2).toInt() > bList.at(2).toInt() )
            {
                return 1;
            }
            else if( aList.at(2) < bList.at(2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
}


void WaitingForSync::paintEvent(QPaintEvent *e)
{
//    QPainter painter(this);
//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QBrush(QColor(243,241,250)));
//    painter.drawRect(0,0,228,24);
//    painter.drawPixmap(7,5,32,12,QPixmap(":/ui/wallet_ui/hx_label_logo.png").scaled(32,12,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
//    painter.drawPixmap(94,38,36,36,QPixmap(":/ui/wallet_ui/logo_center.png").scaled(36,36,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QWidget::paintEvent(e);
}


