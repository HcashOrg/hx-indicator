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
    ui->syncLabel->setToolTip(tr("Local block height / Network block height"));

    connect(HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)),this, SLOT(jsonDataUpdated(QString)));

    this->setAttribute(Qt::WA_TranslucentBackground);

    ui->syncLabel->installEventFilter(this);
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
    ui->syncLabel->setToolTip(tr("Local block height / Network block height"));
}

void BottomBar::jsonDataUpdated(QString id)
{
    if( id == "id-info")
    {
//        HXChain::getInstance()->parseBalance();

        QString result = HXChain::getInstance()->jsonDataValue( id);

        if( result.isEmpty() )  return;


        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonObject object = jsonObject.take("result").toObject();
//        HXChain::getInstance()->walletInfo.blockHeight = object.take("head_block_num").toInt();
        HXChain::getInstance()->walletInfo.blockId = object.take("head_block_id").toString();
        HXChain::getInstance()->walletInfo.blockAge = object.take("head_block_age").toString();
        HXChain::getInstance()->walletInfo.chainId = object.take("chain_id").toString();
        HXChain::getInstance()->walletInfo.participation = object.take("participation").toString();

        HXChain::getInstance()->postRPC("get_current_block_time",toJsonFormat("get_block",QJsonArray()<<HXChain::getInstance()->walletInfo.blockHeight));
//        HXChain::getInstance()->walletInfo.activeMiners.clear();
//        QJsonArray array = object.take("active_miners").toArray();
//        foreach (QJsonValue v, array)
//        {
//            HXChain::getInstance()->walletInfo.activeMiners += v.toString();
//        }


        return;
    }
    else if("get_current_block_time" == id)
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;


        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonObject object = jsonObject.take("result").toObject();

        HXChain::getInstance()->currentBlockTime = object.value("timestamp").toString();
    }
    else if("id-network_get_info" == id)
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )
        {
            ui->nodeNumLabel->setText("0");
            return;
        }

        result.prepend("{");
        result.append("}");
        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonObject resultObject = jsonObject.value("result").toObject();
        ui->nodeNumLabel->setText(QString::number(resultObject.value("connections").toInt()));
        HXChain::getInstance()->walletInfo.blockHeight = resultObject.value("current_block_height").toInt();
        HXChain::getInstance()->walletInfo.targetBlockHeight = resultObject.value("target_block_height").toInt();

        ui->syncLabel->setText(QString::number(HXChain::getInstance()->walletInfo.blockHeight) + " / "
                               + QString::number(HXChain::getInstance()->walletInfo.targetBlockHeight));

        CheckBlockSync();

        Q_EMIT walletInfoUpdated();
    }


}

void BottomBar::CheckBlockSync()
{
    if(HXChain::getInstance()->walletInfo.blockHeight == 0 || !HXChain::getInstance()->walletInfo.blockAge.contains("second"))
    {
        HXChain::getInstance()->SetBlockSyncFinish(false);
        return;
    }

    if(HXChain::getInstance()->walletInfo.blockHeight + 50 > HXChain::getInstance()->walletInfo.targetBlockHeight)
    {
        HXChain::getInstance()->SetBlockSyncFinish(true);
    }
    else
    {
        HXChain::getInstance()->SetBlockSyncFinish(false);
    }

}

void BottomBar::paintEvent(QPaintEvent *)
{
//    QPainter painter(this);
//    painter.setBrush(QColor(248,249,253));
//    painter.setPen(QColor(248,249,253));
//    painter.drawRect(rect());
//    //painter.drawRect(QRect(0,0,900,40));

//    if(rect().width() > 680)
//    {
//        painter.setBrush(QColor(24,28,45));
//        painter.setPen(QColor(24,28,45));
//        painter.drawRect(0,0,124,40);
    //    }
}

bool BottomBar::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->syncLabel)
    {
        if( e->type() == QEvent::MouseButtonPress)
        {
//            if(!HXChain::getInstance()->GetBlockSyncFinish())
//            {
                Q_EMIT showCoverWidget();
//            }
            return true;
        }
        return false;
    }
}

void BottomBar::refresh()
{
    HXChain::getInstance()->postRPC( "id-info", toJsonFormat( "info", QJsonArray()));

    HXChain::getInstance()->postRPC( "id-network_get_info", toJsonFormat( "network_get_info", QJsonArray()));


}
