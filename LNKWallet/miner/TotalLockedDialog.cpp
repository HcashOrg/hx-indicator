#include "TotalLockedDialog.h"
#include "ui_TotalLockedDialog.h"

#include "wallet.h"
#include "control/AssetIconItem.h"

TotalLockedDialog::TotalLockedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TotalLockedDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);

    setObjectName("totalLockedDialog");
    setStyleSheet("#totalLockedDialog{border:1px solid rgb(173,173,173);background-color:rgb(243,241,250);}");

    ui->tableWidget->installEventFilter(this);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->tableWidget->setFrameShape(QFrame::NoFrame);
    ui->tableWidget->setMouseTracking(true);
    ui->tableWidget->setShowGrid(false);//隐藏表格线

    ui->tableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->tableWidget->horizontalHeader()->setFixedHeight(30);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->setColumnWidth(0,140);
    ui->tableWidget->setColumnWidth(1,180);

    ui->tableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    calculateTotalLocked();
}

TotalLockedDialog::~TotalLockedDialog()
{
    delete ui;
}

void TotalLockedDialog::calculateTotalLocked()
{
    QMap<QString,unsigned long long>    map;
    QStringList keys = HXChain::getInstance()->minerMap.keys();
    foreach (QString key, keys)
    {
        MinerInfo info = HXChain::getInstance()->minerMap.value(key);
        for(AssetAmount aa : info.lockBalances)
        {
            map[aa.assetId] += aa.amount;
        }
    }

    int size = map.size();
    QStringList assetIds = map.keys();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setRowCount(size);
    for(int i = 0; i < size; i++)
    {
        ui->tableWidget->setRowHeight(i,40);

        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetIds.at(i));
        ui->tableWidget->setItem( i, 0, new QTableWidgetItem(assetInfo.symbol));
        ui->tableWidget->setItem( i, 1, new QTableWidgetItem( getBigNumberString(map.value(assetIds.at(i)), assetInfo.precision)));

        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->tableWidget->item(i,0)->text());
        ui->tableWidget->setCellWidget(i, 0, assetIconItem);
    }

    tableWidgetSetItemZebraColor(ui->tableWidget);
}
