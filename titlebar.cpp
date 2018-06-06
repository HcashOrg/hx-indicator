#include <QDebug>


#include "titlebar.h"
#include "ui_titlebar.h"

#include "wallet.h"
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
    ui->label->setPixmap(QPixmap(":/ui/wallet_ui/Linklogo.png").scaled(130,30));
    ui->backBtn->setVisible(false);
    ui->backBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/back.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));
    connect(ui->backBtn,&QToolButton::clicked,this,&TitleBar::back);


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

void TitleBar::backBtnVis(bool isVisible)
{
    ui->backBtn->setVisible(isVisible);

}

void TitleBar::jsonDataUpdated(QString id)
{


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


void TitleBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(255,255,255));
    painter.setPen(QColor(255,255,255));
    painter.drawRect(rect());

    painter.setBrush(QColor(38,62,181));
    painter.setPen(QColor(38,62,181));
    painter.drawRect(QRect(0,0,189,49));
}
