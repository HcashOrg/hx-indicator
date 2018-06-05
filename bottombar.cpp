#include "bottombar.h"
#include "ui_bottombar.h"


#include "consoledialog.h"
#include "wallet.h"
#include "commondialog.h"


#include <QTimer>
#include <QMovie>
#include <QMouseEvent>

BottomBar::BottomBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BottomBar)
{
    
    ui->setupUi(this);

    ui->nodeNumLabel->setToolTip(tr("Number of connected nodes"));
    ui->syncLabel->setToolTip(tr("Local block height / Network block height(estimated)"));

    connect(UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)),this, SLOT(jsonDataUpdated(QString)));

//    ui->networkLabel->setPixmap(QPixmap(":/ui/wallet_ui/net.png").scaled(20,20));
}

BottomBar::~BottomBar()
{
    delete ui;
}


void BottomBar::retranslator()
{
    ui->retranslateUi(this);

    ui->nodeNumLabel->setToolTip(tr("Number of connected nodes"));
    ui->syncLabel->setToolTip(tr("Local block height / Network block height(estimated)"));
}

void BottomBar::jsonDataUpdated(QString id)
{
    if( id == "id-info")
    {
//        UBChain::getInstance()->parseBalance();

        QString result = UBChain::getInstance()->jsonDataValue( id);
        qDebug()<<result;
        CheckBlockSync(result);
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

//        UBChain::getInstance()->walletInfo.activeMiners.clear();
//        QJsonArray array = object.take("active_miners").toArray();
//        foreach (QJsonValue v, array)
//        {
//            UBChain::getInstance()->walletInfo.activeMiners += v.toString();
//        }

        ui->syncLabel->setText(QString::number(UBChain::getInstance()->walletInfo.blockHeight));

        return;
    }
    else if("id-network_get_connected_peers" == id)
    {
        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )
        {
            ui->nodeNumLabel->setText("0");
            return;
        }

        result.prepend("{");
        result.append("}");
        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        ui->nodeNumLabel->setText(QString::number(jsonObject.value("result").toArray().size()));

    }


}

void BottomBar::CheckBlockSync(const QString &res)
{
    if(res.isEmpty())
    {
        UBChain::getInstance()->SetBlockSyncFinish(false);
        return;
    }
    QString data = res;
    data.prepend("{");
    data.append("}");
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
        UBChain::getInstance()->SetBlockSyncFinish(false);
        return;
    }

    QJsonObject jsonObject = parse_doucment.object();
    QJsonObject object = jsonObject.value("result").toObject();
    QString blockAge = object.take("head_block_age").toString();
    if(blockAge.contains("second"))
    {
        UBChain::getInstance()->SetBlockSyncFinish(true);
    }
    else
    {
        UBChain::getInstance()->SetBlockSyncFinish(false);
    }

}

void BottomBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(248,249,253));
    painter.setPen(QColor(248,249,253));
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
    UBChain::getInstance()->postRPC( "id-info", toJsonFormat( "info", QJsonArray()));

    UBChain::getInstance()->postRPC( "id-network_get_connected_peers", toJsonFormat( "network_get_connected_peers", QJsonArray()));

//    UBChain::getInstance()->postRPC( "id_blockchain_list_assets", toJsonFormat( "blockchain_list_assets", QJsonArray()));

//    UBChain::getInstance()->postRPC( toJsonFormat( "id_balance", "balance", QJsonArray()));

}
