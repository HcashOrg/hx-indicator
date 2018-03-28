#include "bottombar.h"
#include "ui_bottombar.h"


#include "consoledialog.h"
#include "lnk.h"
#include "commontip.h"
#include "commondialog.h"
#include "extra/dynamicmove.h"

#include <QPainter>
#include <QTimer>
#include <QMovie>
#include <QMouseEvent>

BottomBar::BottomBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BottomBar)
{
    
    ui->setupUi(this);

//    ui->syncLabel->setToolTip(QString::fromLocal8Bit("本地最新区块"));
    ui->syncLabel->setToolTip(tr("Local block height / Network block height(estimated)"));

    connect(UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)),this, SLOT(jsonDataUpdated(QString)));
    jsonDataUpdated("id_info");


    timer = new QTimer(this);
    timer->start(5000);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateNumOfConnections()));
    updateNumOfConnections();

    connectionTip = new CommonTip;

    
}

BottomBar::~BottomBar()
{
    delete ui;
    delete timer;  
}



void BottomBar::updateNumOfConnections()
{
//    

    QString result = UBChain::getInstance()->jsonDataValue("id_info");
    if( result.isEmpty())  return;

    int pos = result.indexOf("\"network_num_connections\"") + 26;
    QString num = result.mid( pos, result.indexOf("," , pos) - pos);

    ui->nodeNumLabel->setText(num);


//    
}


void BottomBar::retranslator()
{
    ui->retranslateUi(this);

    ui->syncLabel->setToolTip(tr("Local block height / Network block height(estimated)"));

}

void BottomBar::jsonDataUpdated(QString id)
{
    if( id == "id_info")
    {
        UBChain::getInstance()->parseBalance();

        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;

        int pos = result.indexOf( "\"blockchain_head_block_age\":") + 28;
        QString seconds = result.mid( pos, result.indexOf("\"blockchain_head_block_timestamp\":") - pos - 1);

        int pos2 = result.indexOf( "\"blockchain_head_block_num\":") + 28;
        QString num = result.mid( pos2, result.indexOf("\"blockchain_head_block_age\":") - pos2 - 1);

        if( seconds.toInt() < 0)  seconds = "0";
        int numToSync = seconds.toInt()/ 10;

        ui->syncLabel->setText( num + " / " + QString::number( num.toInt() + numToSync) );
        UBChain::getInstance()->currentBlockHeight = num.toInt();

        int pos3 = result.indexOf( "\"wallet_scan_progress\":") + 23;
        QString scanProgress = result.mid( pos3, result.indexOf(",\"wallet_block_production_enabled\":") - pos3);

        scanProgress.remove('\"');
        double scanPercent = scanProgress.toDouble();

        if( UBChain::getInstance()->needToScan && scanPercent > 0.99999)
        {
            UBChain::getInstance()->postRPC( "id_scan", toJsonFormat( "scan", QStringList() << "0"));
            UBChain::getInstance()->needToScan = false;
        }

        return;
    }


}

void BottomBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(40,46,66));
    painter.setPen(QColor(40,46,66));
    painter.drawRect(QRect(0,0,680,53));
}

void BottomBar::refresh()
{
    UBChain::getInstance()->postRPC( "id_info", toJsonFormat( "info", QStringList() << ""));

    UBChain::getInstance()->postRPC( "id_blockchain_list_assets", toJsonFormat( "blockchain_list_assets", QStringList() << ""));

//    UBChain::getInstance()->postRPC( toJsonFormat( "id_balance", "balance", QStringList() << ""));

}
