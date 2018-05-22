#include "waitingforsync.h"
#include "ui_waitingforsync.h"
#include "wallet.h"

#include "commondialog.h"
#include "websocketmanager.h"


#include <QTimer>
#include <QDebug>
#include <QMovie>
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
    if(UBChain::getInstance()->wsManager == NULL)   return;
    qDebug() << "wsManager isConnected: " << UBChain::getInstance()->wsManager->isConnected;
    if(UBChain::getInstance()->wsManager->isConnected)      emit sync();
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
}

void WaitingForSync::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/background.png").scaled(this->size())));
    setPalette(palette);

    ui->loadingLabel->setFont(QFont("黑体",12,53));
    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_version->setPalette(pa);

    ui->loadingLabel->setPalette(pa);

    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

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

    QPainter painter(this);
    painter.drawPixmap(238,130,490,320,QPixmap(":/ui/wallet_ui/login.png").scaled(490,320));


    QWidget::paintEvent(e);
}


