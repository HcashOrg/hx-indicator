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

    connectionTip = new CommonTip;

    
}

BottomBar::~BottomBar()
{
    delete ui;
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

        ui->syncLabel->setText(QString::number(UBChain::getInstance()->walletInfo.blockHeight));

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
