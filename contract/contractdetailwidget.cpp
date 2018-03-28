#include "contractdetailwidget.h"
#include "ui_contractdetailwidget.h"

#include <QPainter>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

#include "lnk.h"
#include "dialog/choosecontractoperationdialog.h"
#include "dialog/depositdialog.h"
#include "dialog/destroydialog.h"
#include "dialog/mintdialog.h"
#include "dialog/withdrawcontractbalancedialog.h"

ContractDetailWidget::ContractDetailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContractDetailWidget)
{
    ui->setupUi(this);

    setStyleSheet("#widget{background-color:white;border:1px solid rgb(230,230,230);border-radius:3px;}");


    ui->containerWidget->installEventFilter(this);

    followBtn = new MyCheckBtn(ui->containerWidget);
    followBtn->setCheckStyle("QToolButton{background-image:url(:/ui/wallet_ui/follow.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    followBtn->setUncheckStyle("QToolButton{background-image:url(:/ui/wallet_ui/unfollow.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    followBtn->setChecked(false);
    followBtn->setGeometry(0,0,30,30);
    connect(followBtn,SIGNAL(pressed(bool)),this,SLOT(onFollowBtnPressed()));
}

ContractDetailWidget::~ContractDetailWidget()
{
    delete ui;
}

void ContractDetailWidget::setContract(QString _contractAddress)
{
    contractAddress = _contractAddress;

    ui->priceLabel->setText(tr("unknow"));
    ui->marketValueLabel->setText(tr("unknow"));

    ui->addressLabel->setText(contractAddress);
    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);

    if( UBChain::getInstance()->isMyAddress(info.admin))
    {
        ui->adminLabel->setText(info.admin + QString(" (%1)").arg(UBChain::getInstance()->addressToName(info.admin)));
    }
    else
    {
        ui->adminLabel->setText(info.admin);
    }

    QFileInfo fileInfo(UBChain::getInstance()->walletConfigPath + info.logoPath);
    if(fileInfo.exists())
    {
        ui->logoLabel->setPixmap(QPixmap(fileInfo.absoluteFilePath()));
    }
    else
    {
        ui->logoLabel->setPixmap(QPixmap(":/ui/wallet_ui/defaultLogo.png"));
    }
qDebug() << "fffffffffff " << info.summary;
    ui->summaryLabel->setText(info.summary);


    ui->nameLabel->setText(info.contractName);
    followBtn->setChecked(UBChain::getInstance()->myConcernedContracts.contains(contractAddress));

    ui->supplyLabel->setText( getBigNumberString(info.totalSupply,info.precision));
    ui->precisionLabel->setText(QString::number(QString::number(info.precision).size() - 1));
    ui->websiteLabel->setText(QString("<a style='color: green;' href = %1> %1</a>").arg(info.website));
    ui->descriptionLabel->setText(info.description);

    if( info.isSmart)
    {
        ui->typeLabel->setText(tr("MORTGAGE"));

        QString anchorStr = "1 " + info.contractName + " = " + getBigNumberString(info.anchorRatio,info.anchorTokenPrecision) + " " + info.anchorTokenSymbol;
        ui->anchorLabel->setText( anchorStr);

        QString percentStr = QString::number( info.mortgageRate.toDouble() * 100, 'f',2);
        ui->mortgageRateLabel->setText(percentStr + "%");

        ui->anchorLabel->show();
        ui->anchorLabel2->show();

        ui->mortgageRateLabel->show();
        ui->mortgageRateLabel2->show();

        if( UBChain::getInstance()->isMyAddress(info.admin))
        {
            ui->manageBtn->show();
        }
        else
        {
            ui->manageBtn->hide();
        }

    }
    else
    {
        ui->typeLabel->setText(tr("NO MORTGAGE"));

        ui->anchorLabel->hide();
        ui->anchorLabel2->hide();

        ui->mortgageRateLabel->hide();
        ui->mortgageRateLabel2->hide();

        ui->manageBtn->hide();
    }
}

bool ContractDetailWidget::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->containerWidget)
    {
        if( e->type() == QEvent::Paint)
        {
            QPainter painter(ui->containerWidget);
            painter.setPen(QPen(QColor(240,242,248),Qt::SolidLine));
            painter.setBrush(QBrush(QColor(240,242,248),Qt::SolidPattern));
            painter.drawRoundedRect(0,90,620,330,5,5);
            painter.drawRect(0,80,620,20);

            return true;
        }
    }

    return QWidget::eventFilter(watched,e);
}

void ContractDetailWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
    painter.drawRect(0,0,680,482);
}

void ContractDetailWidget::on_backBtn_clicked()
{
    emit back();
}

void ContractDetailWidget::on_websiteLabel_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void ContractDetailWidget::onFollowBtnPressed()
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

void ContractDetailWidget::on_manageBtn_clicked()
{
    ChooseContractOperationDialog chooseContractOperationDialog(this);
    chooseContractOperationDialog.move( ui->manageBtn->mapToGlobal(QPoint(0,30)) );
    chooseContractOperationDialog.exec();

    switch (chooseContractOperationDialog.choice)
    {
    case 1:
    {
        DepositDialog depositDialog(contractAddress);
        depositDialog.pop();
        break;
    }
    case 2:
    {
        MintDialog mintDialog(contractAddress);
        mintDialog.pop();
        break;
    }
    case 3:
    { 
        DestroyDialog destroyDialog(contractAddress);
        destroyDialog.pop();
        break;
    }
    case 4:
    {
        WithdrawContractBalanceDialog withdrawContractBalanceDialog(contractAddress);
        withdrawContractBalanceDialog.pop();
        break;
    }
    default:
        break;
    }
}
