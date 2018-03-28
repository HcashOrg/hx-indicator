#include "singlecontractwidget.h"
#include "ui_singlecontractwidget.h"

#include <QPainter>
#include <QDebug>

#include "lnk.h"

SingleContractWidget::SingleContractWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleContractWidget)
{
    ui->setupUi(this);

    setStyleSheet("#widget{background-color:white;border:1px solid rgb(230,230,230);border-radius:3px;font: 9pt \"微软雅黑\";}");

    ui->widget->installEventFilter(this);

    followBtn = new MyCheckBtn(this);
    followBtn->setCheckStyle("QToolButton{background-image:url(:/ui/wallet_ui/follow.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    followBtn->setUncheckStyle("QToolButton{background-image:url(:/ui/wallet_ui/unfollow.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    followBtn->setChecked(false);
    followBtn->setGeometry(0,0,30,30);
    connect(followBtn,SIGNAL(pressed(bool)),this,SLOT(onFollowBtnPressed()));

    ui->logoLabel->raise();
}

SingleContractWidget::~SingleContractWidget()
{
    delete ui;
}

void SingleContractWidget::setContract(QString _contractAddress)
{
    contractAddress = _contractAddress;


    ui->priceLabel->setText(tr("unkown"));


    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);

    QFileInfo fileInfo(UBChain::getInstance()->walletConfigPath + info.logoPath);
    if(fileInfo.exists())
    {
        ui->logoLabel->setPixmap(QPixmap(fileInfo.absoluteFilePath()));
    }
    else
    {
        ui->logoLabel->setPixmap(QPixmap(":/ui/wallet_ui/defaultLogo.png"));
    }


    ui->nameLabel->setText(info.contractName);
    followBtn->setChecked(UBChain::getInstance()->myConcernedContracts.contains(contractAddress));

    ui->supplyLabel->setText( getBigNumberString(info.totalSupply,info.precision));

    if( info.isSmart)
    {
        ui->typeLabel->setText(tr("MORTGAGE"));

        QString anchorStr = "1 " + info.contractName + " = " + getBigNumberString(info.anchorRatio,info.anchorTokenPrecision) + " " + info.anchorTokenSymbol;
        ui->anchorLabel->setText( anchorStr);

    }
    else
    {
        ui->typeLabel->setText(tr("NO MORTGAGE"));

        ui->anchorLabel->hide();
        ui->anchorLabel2->hide();

    }

}


bool SingleContractWidget::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->widget)
    {
        if( e->type() == QEvent::Paint)
        {
            QPainter painter(ui->widget);
            painter.setPen(QPen(QColor(240,242,248),Qt::SolidLine));
            painter.setBrush(QBrush(QColor(240,242,248),Qt::SolidPattern));
            painter.drawRoundedRect(0,90,200,70,5,5);
            painter.drawRect(0,80,200,20);
            return true;
        }
    }

    return QWidget::eventFilter(watched,e);
}


void SingleContractWidget::on_detailBtn_clicked()
{
    emit showDetail(contractAddress);
}

void SingleContractWidget::onFollowBtnPressed()
{
    followBtn->setChecked(!followBtn->isChecked);

    if(followBtn->isChecked)
    {
        UBChain::getInstance()->addConcernedContract(contractAddress);
    }
    else
    {
        UBChain::getInstance()->removeConcernedContract(contractAddress);
    }

    emit refresh();
}
