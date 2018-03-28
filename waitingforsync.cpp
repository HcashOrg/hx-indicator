#include "waitingforsync.h"
#include "ui_waitingforsync.h"
#include "lnk.h"
#include "debug_log.h"
#include "commondialog.h"
#include "workerthreadmanager.h"
#include "websocketmanager.h"

#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QMovie>
#include <QDesktopServices>


WaitingForSync::WaitingForSync(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaitingForSync)
{
    ui->setupUi(this);

    setStyleSheet("WaitingForSync{background-image:url(:/ui/wallet_ui/loginBg.png);}");


    ui->logoLabel->setPixmap(QPixmap(":/ui/wallet_ui/logo_52x34.png"));

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/closeBtn.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/ui/wallet_ui/closeBtn_hover.png);}");

    ui->picLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);


    rotateNum = 0;
    timerForPic = new QTimer(this);
    connect(timerForPic,SIGNAL(timeout()),this,SLOT(showPic()));
    timerForPic->start(25);

    UBChain::getInstance()->initWebSocketManager();
    timerForWSConnected = new QTimer(this);
    connect(timerForWSConnected,SIGNAL(timeout()),this,SLOT(checkConnected()));
    timerForWSConnected->start(1000);
//    UBChain::getInstance()->initWorkerThreadManager();
//    connect(UBChain::getInstance()->workerManager,SIGNAL(allConnected()),this,SIGNAL(sync()));
}

WaitingForSync::~WaitingForSync()
{
    delete ui;
}


void WaitingForSync::on_closeBtn_clicked()
{
    emit closeWallet();
}

void WaitingForSync::checkConnected()
{
    if(UBChain::getInstance()->wsManager->isConnected)      emit sync();
}



void WaitingForSync::showPic()
{
    rotateNum += 9;
    if(rotateNum >= 360)    rotateNum -= 360;

    QMatrix matrix;
    matrix.rotate(rotateNum);

    ui->picLabel->setPixmap(QPixmap(":/ui/wallet_ui/loading.png").transformed(matrix,Qt::SmoothTransformation));

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
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(e);
}


