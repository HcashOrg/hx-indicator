#include <QDebug>
#include <QPainter>

#include "titlebar.h"
#include "ui_titlebar.h"

#include "lnk.h"
#include "commondialog.h"

TitleBar::TitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleBar)
{

    ui->setupUi(this);    

    ui->minBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/minimizeBtn.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/closeBtn.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/ui/wallet_ui/closeBtn_hover.png);");

//    ui->divLineLabel->setPixmap(QPixmap("pic2/divLine.png"));
//    ui->divLineLabel->setScaledContents(true);


    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    timer->setInterval(10000);
    timer->start();

    onTimeOut();
	
}

TitleBar::~TitleBar()
{
    delete ui;
}

void TitleBar::on_minBtn_clicked()
{
    if( UBChain::getInstance()->minimizeToTray)
    {
        emit tray();
    }
    else
    {  
        emit minimum();
    }
}

void TitleBar::on_closeBtn_clicked()
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

void TitleBar::retranslator()
{
    ui->retranslateUi(this);
}

void TitleBar::jsonDataUpdated(QString id)
{
    if( id == "id_blockchain_list_pending_transactions")
    {
        QString pendingTransactions = UBChain::getInstance()->jsonDataValue(id);
        // 查询一遍 config中记录的交易ID
        mutexForConfigFile.lock();
        UBChain::getInstance()->configFile->beginGroup("/recordId");
        QStringList keys = UBChain::getInstance()->configFile->childKeys();
        UBChain::getInstance()->configFile->endGroup();

        int numOfNews = 0;
        foreach (QString key, keys)
        {
            if( UBChain::getInstance()->configFile->value("/recordId/" + key).toInt() == 2)
            {
                // 失效的交易
                numOfNews++;
                continue;
            }

            if( !pendingTransactions.contains(key))  // 如果不在pending区, 看看是否在链上
            {
                UBChain::getInstance()->postRPC( "id_blockchain_get_transaction_" + key, toJsonFormat( "blockchain_get_transaction", QStringList() << key  ));
            }

            if( UBChain::getInstance()->configFile->value("/recordId/" + key).toInt() == 1)
            {
                numOfNews++;
            }
        }
        mutexForConfigFile.unlock();

        return;
    }

    if( id.startsWith("id_blockchain_get_transaction"))
    {

        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.mid(0,22).contains("exception") || result.mid(0,22).contains("error"))
        {
            // 若不在pending区也不在链上  则为失效交易  recordId置为2
            mutexForConfigFile.lock();

            UBChain::getInstance()->configFile->setValue("/recordId/" + id.right(40), 2);

            mutexForConfigFile.unlock();

            return;
        }
        else   //  如果已经被打包进区块，则将config中记录置为1
        {
            mutexForConfigFile.lock();

            UBChain::getInstance()->configFile->setValue("/recordId/" + id.right(40), 1);

            mutexForConfigFile.unlock();
        }

        return;
    }

}

void TitleBar::onTimeOut()
{
    UBChain::getInstance()->postRPC( "id_blockchain_list_pending_transactions", toJsonFormat( "blockchain_list_pending_transactions", QStringList() << "" ));

    mutexForConfigFile.lock();
    UBChain::getInstance()->configFile->beginGroup("/applyingForDelegateAccount");
    QStringList keys = UBChain::getInstance()->configFile->childKeys();
    UBChain::getInstance()->configFile->endGroup();
    foreach (QString key, keys)
    {
        // 如果申请代理的recordId 被删除了 或者被确认了（=1）或者失效了（=2） 则 删除applyingForDelegateAccount的记录
        if( !UBChain::getInstance()->configFile->contains("/recordId/" + UBChain::getInstance()->configFile->value("/applyingForDelegateAccount/" + key).toString())
            ||  UBChain::getInstance()->configFile->value("/recordId/" + UBChain::getInstance()->configFile->value("/applyingForDelegateAccount/" + key).toString()).toInt() != 0 )
        {
            UBChain::getInstance()->configFile->remove("/applyingForDelegateAccount/" + key);
        }
    }

    UBChain::getInstance()->configFile->beginGroup("/registeringAccount");
    keys = UBChain::getInstance()->configFile->childKeys();
    UBChain::getInstance()->configFile->endGroup();
    foreach (QString key, keys)
    {
        // 如果注册升级的recordId 被删除了 或者被确认了（=1）或者失效了（=2） 则 删除registeringAccount的记录
        if( !UBChain::getInstance()->configFile->contains("/recordId/" + UBChain::getInstance()->configFile->value("/registeringAccount/" + key).toString())
            ||  UBChain::getInstance()->configFile->value("/recordId/" + UBChain::getInstance()->configFile->value("/registeringAccount/" + key).toString()).toInt() != 0 )
        {
            UBChain::getInstance()->configFile->remove("/registeringAccount/" + key);
        }
    }

    mutexForConfigFile.unlock();
}

void TitleBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(40,46,66));
    painter.setPen(QColor(40,46,66));
    painter.drawRect(QRect(0,0,680,35));
}
