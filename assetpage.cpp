#include "assetpage.h"
#include "ui_assetpage.h"

#include <QDebug>
#include <QPainter>

#include "wallet.h"
#include "commondialog.h"
#include "dialog/addtokendialog.h"
#include "dialog/choosetokenoperationdialog.h"
#include "dialog/depositdialog.h"
#include "dialog/destroydialog.h"
#include "dialog/mintdialog.h"
#include "dialog/withdrawcontractbalancedialog.h"
#include "dialog/createtokendialog.h"
#include "showcontentdialog.h"

#define ASSETPAGE_ERC20TOKENBTN_SELECTED_STYLE     "QToolButton{background-color:rgb(0,138,254);color:white;border:1px solid rgb(0,138,254);border-top-left-radius: 3px;border-bottom-left-radius: 3px;}"
#define ASSETPAGE_ERC20TOKENBTN_UNSELECTED_STYLE   "QToolButton{background-color:white;color:rgb(0,138,254);border:1px solid rgb(0,138,254);border-top-left-radius: 3px;border-bottom-left-radius: 3px;}"
#define ASSETPAGE_SMARTTOKENBTN_SELECTED_STYLE        "QToolButton{background-color:rgb(0,138,254);color:white;border:1px solid rgb(0,138,254);border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
#define ASSETPAGE_SMARTTOKENBTN_UNSELECTED_STYLE      "QToolButton{background-color:white;color:rgb(0,138,254);border:1px solid rgb(0,138,254);border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"


AssetPage::AssetPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssetPage)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(244,244,242));
    setPalette(palette);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    updateAssetInfo();
    updateMyAsset();

    ui->smartTokenTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->smartTokenTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->smartTokenTableWidget->setColumnWidth(0,80);
    ui->smartTokenTableWidget->setColumnWidth(1,200);
    ui->smartTokenTableWidget->setColumnWidth(2,100);
    ui->smartTokenTableWidget->setColumnWidth(3,80);
    ui->smartTokenTableWidget->setColumnWidth(4,140);
    ui->smartTokenTableWidget->setColumnWidth(5,120);
    ui->smartTokenTableWidget->setColumnWidth(6,80);
    ui->smartTokenTableWidget->setColumnWidth(7,80);
    ui->smartTokenTableWidget->setShowGrid(true);
    ui->smartTokenTableWidget->setFrameShape(QFrame::NoFrame);
    ui->smartTokenTableWidget->setMouseTracking(true);
    ui->smartTokenTableWidget->horizontalHeader()->setVisible(true);

    ui->erc20TokenTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->erc20TokenTableWidget->setColumnWidth(0,120);
    ui->erc20TokenTableWidget->setColumnWidth(1,360);
    ui->erc20TokenTableWidget->setColumnWidth(2,120);
    ui->erc20TokenTableWidget->setColumnWidth(3,80);
    ui->erc20TokenTableWidget->setColumnWidth(4,80);
    ui->erc20TokenTableWidget->setShowGrid(true);
    ui->erc20TokenTableWidget->setFrameShape(QFrame::NoFrame);
    ui->erc20TokenTableWidget->setMouseTracking(true);
    ui->erc20TokenTableWidget->horizontalHeader()->setVisible(true);

    ui->smartTokenTableWidget->installEventFilter(this);
    ui->erc20TokenTableWidget->installEventFilter(this);

    on_erc20TokenBtn_clicked();

}

AssetPage::~AssetPage()
{
    delete ui;
}

void AssetPage::updateAssetInfo()
{
//    ui->assetInfoTableWidget->setRowCount(0);
//    ui->assetInfoTableWidget->setRowCount(UBChain::getInstance()->assetInfoMap.size());
//    foreach (int id, UBChain::getInstance()->assetInfoMap.keys())
//    {
//        ui->assetInfoTableWidget->setRowHeight(id,45);

//        AssetInfo info = UBChain::getInstance()->assetInfoMap.value(id);

//        ui->assetInfoTableWidget->setItem(id,0,new QTableWidgetItem(QString::number(id)));
//        ui->assetInfoTableWidget->setItem(id,1,new QTableWidgetItem(info.symbol));

//        if( id == 0)
//        {
//            ui->assetInfoTableWidget->setItem(id,2,new QTableWidgetItem(QString::fromLocal8Bit("系统")));
//            ui->assetInfoTableWidget->setItem(id,4,new QTableWidgetItem("-"));
//        }
//        else
//        {
//            ui->assetInfoTableWidget->setItem(id,2,new QTableWidgetItem(info.owner));
//            ui->assetInfoTableWidget->setItem(id,4,new QTableWidgetItem(QString::number(info.currentSupply * 100 / info.maxSupply,'g',16) + "%"));
//        }

//        ui->assetInfoTableWidget->setItem(id,3,new QTableWidgetItem(QString::number(info.maxSupply / info.precision,'g',16)));
//        ui->assetInfoTableWidget->setItem(id,5,new QTableWidgetItem(info.description));

//        for(int j = 0; j < 6; j++)
//        {
//            ui->assetInfoTableWidget->item(id,j)->setTextAlignment(Qt::AlignCenter);
//        }

//    }
}

void AssetPage::updateMyAsset()
{
    ui->smartTokenTableWidget->setRowCount(0);
    ui->erc20TokenTableWidget->setRowCount(0);

    int smartTokenCount = 0;
    int erc20TokenCount = 0;
    foreach (QString contractAddress, UBChain::getInstance()->ERC20TokenInfoMap.keys())
    {
        ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);

        if( info.isSmart)
        {
            ui->smartTokenTableWidget->setRowCount(smartTokenCount + 1);
            ui->smartTokenTableWidget->setRowHeight(smartTokenCount,45);

            ui->smartTokenTableWidget->setItem(smartTokenCount,0,new QTableWidgetItem(info.contractName));
            ui->smartTokenTableWidget->setItem(smartTokenCount,1,new QTableWidgetItem(info.contractAddress));
            ui->smartTokenTableWidget->setItem(smartTokenCount,2,new QTableWidgetItem(getBigNumberString(info.totalSupply,info.precision)));
            QString percentStr = QString::number( info.mortgageRate.toDouble() * 100, 'f',2);
            ui->smartTokenTableWidget->setItem(smartTokenCount,3,new QTableWidgetItem(percentStr + "%"));
            QString anchorStr = "1 " + info.contractName + " = " + getBigNumberString(info.anchorRatio,info.anchorTokenPrecision) + " " + info.anchorTokenSymbol;
            ui->smartTokenTableWidget->setItem(smartTokenCount,4,new QTableWidgetItem( anchorStr));
            ui->smartTokenTableWidget->setItem(smartTokenCount,5,new QTableWidgetItem(getBigNumberString(info.contractBalance,ASSET_PRECISION)));
            ui->smartTokenTableWidget->setItem(smartTokenCount,6,new QTableWidgetItem(QString::fromLocal8Bit("取消关注")));
            ui->smartTokenTableWidget->item(smartTokenCount,6)->setTextColor(QColor(65,205,82));

            if(info.state == "RETIRE")
            {
                ui->smartTokenTableWidget->setItem(smartTokenCount,7,new QTableWidgetItem(QString::fromLocal8Bit("清退中")));
                ui->smartTokenTableWidget->item(smartTokenCount,7)->setTextColor(QColor(230,53,59));
            }
            else
            {
                if( UBChain::getInstance()->isMyAddress(info.admin))
                {
                    ui->smartTokenTableWidget->setItem(smartTokenCount,7,new QTableWidgetItem(QString::fromLocal8Bit("管理员")));
                    ui->smartTokenTableWidget->item(smartTokenCount,7)->setTextColor(QColor(65,205,82));
                }
                else
                {
                    ui->smartTokenTableWidget->setItem(smartTokenCount,7,new QTableWidgetItem(QString::fromLocal8Bit("正常")));
                    ui->smartTokenTableWidget->item(smartTokenCount,7)->setTextColor(QColor(65,205,82));
                }
            }


            for(int j = 0; j < 8; j++)
            {
                ui->smartTokenTableWidget->item(smartTokenCount,j)->setTextAlignment(Qt::AlignCenter);
            }

            smartTokenCount++;

        }
        else
        {
            ui->erc20TokenTableWidget->setRowCount(erc20TokenCount + 1);
            ui->erc20TokenTableWidget->setRowHeight(erc20TokenCount,45);

            ui->erc20TokenTableWidget->setItem(erc20TokenCount,0,new QTableWidgetItem(info.contractName));
            ui->erc20TokenTableWidget->setItem(erc20TokenCount,1,new QTableWidgetItem(info.contractAddress));
            ui->erc20TokenTableWidget->setItem(erc20TokenCount,2,new QTableWidgetItem(getBigNumberString(info.totalSupply,info.precision)));
            ui->erc20TokenTableWidget->setItem(erc20TokenCount,3,new QTableWidgetItem(QString::fromLocal8Bit("取消关注")));
            ui->erc20TokenTableWidget->item(erc20TokenCount,3)->setTextColor(QColor(65,205,82));
            ui->erc20TokenTableWidget->setItem(erc20TokenCount,4,new QTableWidgetItem(info.state));
            ui->erc20TokenTableWidget->item(erc20TokenCount,4)->setTextColor(QColor(65,205,82));



            for(int j = 0; j < 5; j++)
            {
                ui->erc20TokenTableWidget->item(erc20TokenCount,j)->setTextAlignment(Qt::AlignCenter);
            }

            erc20TokenCount++;
        }
    }

}

void AssetPage::jsonDataUpdated(QString id)
{

}

void AssetPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(228,228,228),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(244,244,242),Qt::SolidPattern));
    painter.drawRect(-1,-1,978,66);
}

void AssetPage::on_addTokenBtn_clicked()
{
    AddTokenDialog addTokenDialog;
    addTokenDialog.pop();
}

void AssetPage::on_erc20TokenBtn_clicked()
{
    ui->erc20TokenTableWidget->show();
    ui->smartTokenTableWidget->hide();

    ui->erc20TokenBtn->setStyleSheet(ASSETPAGE_ERC20TOKENBTN_SELECTED_STYLE);
    ui->smartTokenBtn->setStyleSheet(ASSETPAGE_SMARTTOKENBTN_UNSELECTED_STYLE);
}

void AssetPage::on_smartTokenBtn_clicked()
{
    ui->erc20TokenTableWidget->hide();
    ui->smartTokenTableWidget->show();

    ui->erc20TokenBtn->setStyleSheet(ASSETPAGE_ERC20TOKENBTN_UNSELECTED_STYLE);
    ui->smartTokenBtn->setStyleSheet(ASSETPAGE_SMARTTOKENBTN_SELECTED_STYLE);
}

void AssetPage::on_smartTokenTableWidget_cellPressed(int row, int column)
{
    if( column == 1 )
    {
        ShowContentDialog showContentDialog( ui->smartTokenTableWidget->item(row, column)->text(),this);

        int x = ui->smartTokenTableWidget->columnViewportPosition(column) + ui->smartTokenTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->smartTokenTableWidget->rowViewportPosition(row) - 10 + ui->smartTokenTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->smartTokenTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if( column == 6)
    {
        QString symbol = ui->smartTokenTableWidget->item(row,0)->text();
        QString contractAddress = ui->smartTokenTableWidget->item(row,1)->text();

        CommonDialog tipDialog(CommonDialog::YesOrNo);
        tipDialog.setText( QString::fromLocal8Bit("是否取消关注 ") + symbol + " ?");
        if( tipDialog.pop() )
        {
            UBChain::getInstance()->ERC20TokenInfoMap.remove(contractAddress);
            UBChain::getInstance()->configFile->remove("/AddedContractToken/" + contractAddress);

            foreach (QString key, UBChain::getInstance()->accountContractTransactionMap.keys())
            {
                if( key.contains(contractAddress))
                {
                    UBChain::getInstance()->accountContractTransactionMap.remove(key);
                }
            }

            updateMyAsset();
        }

        return;
    }

    if( column == 7)
    {
        QString contractAddress = ui->smartTokenTableWidget->item(row,1)->text();
        ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
        if( info.state != "COMMON")     return;
        if( UBChain::getInstance()->isMyAddress(info.admin))
        {
            ChooseTokenOperationDialog chooseTokenOperationDialog;
            int choice = chooseTokenOperationDialog.pop();

            switch (choice)
            {
            case 1:
            {
                DepositDialog depositDialog(contractAddress);
                depositDialog.pop();
                break;
            }
            case 2:
            {
                DestroyDialog destroyDialog(contractAddress);
                destroyDialog.pop();
                break;
            }
            case 3:
            {
                MintDialog mintDialog(contractAddress);
                mintDialog.pop();
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


        return;
    }
}

void AssetPage::on_erc20TokenTableWidget_cellPressed(int row, int column)
{
    if( column == 1 )
    {
        ShowContentDialog showContentDialog( ui->erc20TokenTableWidget->item(row, column)->text(),this);

        int x = ui->erc20TokenTableWidget->columnViewportPosition(column) + ui->erc20TokenTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->erc20TokenTableWidget->rowViewportPosition(row) - 10 + ui->erc20TokenTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->erc20TokenTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if( column == 3)
    {
        QString symbol = ui->erc20TokenTableWidget->item(row,0)->text();
        QString contractAddress = ui->erc20TokenTableWidget->item(row,1)->text();

        CommonDialog tipDialog(CommonDialog::YesOrNo);
        tipDialog.setText( QString::fromLocal8Bit("是否取消关注 ") + symbol + " ?");
        if( tipDialog.pop() )
        {
            UBChain::getInstance()->ERC20TokenInfoMap.remove(contractAddress);
            UBChain::getInstance()->configFile->remove("/AddedContractToken/" + contractAddress);

            foreach (QString key, UBChain::getInstance()->accountContractTransactionMap.keys())
            {
                if( key.contains(contractAddress))
                {
                    UBChain::getInstance()->accountContractTransactionMap.remove(key);
                }
            }

            updateMyAsset();
        }

        return;
    }
}

void AssetPage::on_issueTokenBtn_clicked()
{
    CreateTokenDialog createTokenDialog;
    createTokenDialog.pop();
}
