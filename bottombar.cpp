#include "bottombar.h"
#include "ui_bottombar.h"


#include "consoledialog.h"
#include "wallet.h"
#include "commontip.h"
#include "commondialog.h"
#include "extra/dynamicmove.h"


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
    jsonDataUpdated("id-info");


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

    QString result = UBChain::getInstance()->jsonDataValue("id-info");
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
    if( id == "id-info")
    {
//        UBChain::getInstance()->parseBalance();

        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;


        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonObject object = jsonObject.take("result").toObject();
        UBChain::getInstance()->walletInfo.blockHeight = object.take("head_block_num").toInt();
        UBChain::getInstance()->walletInfo.blockId = object.take("head_block_id").toString();
        UBChain::getInstance()->walletInfo.blockAge = object.take("head_block_age").toString();
        UBChain::getInstance()->walletInfo.chainId = object.take("chain_id").toString();






        int pos = result.indexOf( "\"blockchain_head_block_age\":") + 28;
        QString seconds = result.mid( pos, result.indexOf("\"blockchain_head_block_timestamp\":") - pos - 1);

        int pos2 = result.indexOf( "\"blockchain_head_block_num\":") + 28;
        QString num = result.mid( pos2, result.indexOf("\"blockchain_head_block_age\":") - pos2 - 1);

        if( seconds.toInt() < 0)  seconds = "0";
        int numToSync = seconds.toInt()/ 10;

        ui->syncLabel->setText( num + " / " + QString::number( num.toInt() + numToSync) );


        return;
    }


}

void BottomBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(40,46,66));
    painter.setPen(QColor(40,46,66));
    painter.drawRect(rect());
    //painter.drawRect(QRect(0,0,900,40));

    if(rect().width() > 680)
    {
        painter.setBrush(QColor(24,28,45));
        painter.setPen(QColor(24,28,45));
        painter.drawRect(0,0,124,40);
    }
}

void BottomBar::refresh()
{
    UBChain::getInstance()->postRPC( "id-info", toJsonFormat( "info", QStringList() ));

//    UBChain::getInstance()->postRPC( "id_blockchain_list_assets", toJsonFormat( "blockchain_list_assets", QStringList() << ""));

//    UBChain::getInstance()->postRPC( toJsonFormat( "id_balance", "balance", QStringList() << ""));

}
