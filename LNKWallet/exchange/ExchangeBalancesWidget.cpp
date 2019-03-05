#include "ExchangeBalancesWidget.h"
#include "ui_ExchangeBalancesWidget.h"

#include "ExchangeModePage.h"
#include "control/AssetIconItem.h"
#include "ToolButtonWidget.h"
#include "depositexchangecontractdialog.h"
#include "withdrawexchangecontractdialog.h"

ExchangeBalancesWidget::ExchangeBalancesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangeBalancesWidget)
{
    ui->setupUi(this);

    ui->balancesTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->balancesTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->balancesTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->balancesTableWidget->setFrameShape(QFrame::NoFrame);
    ui->balancesTableWidget->setMouseTracking(true);
    ui->balancesTableWidget->setShowGrid(false);//隐藏表格线
    ui->balancesTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->balancesTableWidget->horizontalHeader()->setVisible(true);
    ui->balancesTableWidget->verticalHeader()->setVisible(false);
    ui->balancesTableWidget->setColumnWidth(0,140);
    ui->balancesTableWidget->setColumnWidth(1,130);
    ui->balancesTableWidget->setColumnWidth(2,130);
    ui->balancesTableWidget->setColumnWidth(3,80);
    ui->balancesTableWidget->setColumnWidth(4,80);
    ui->balancesTableWidget->setColumnWidth(5,80);

    init();
}

ExchangeBalancesWidget::~ExchangeBalancesWidget()
{
    delete ui;
}

void ExchangeBalancesWidget::init()
{
    showBalances();
}

void ExchangeBalancesWidget::showBalances()
{
    QStringList assets = HXChain::getInstance()->getAllExchangeAssets();

    int size = assets.size();
    ui->balancesTableWidget->setRowCount(0);
    ui->balancesTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->balancesTableWidget->setRowHeight( i, 40);

        QString symbol = assets.at(i);
        ui->balancesTableWidget->setItem( i, 0, new QTableWidgetItem(symbol));
qDebug() << "ssssssssss " << symbol;
        const AssetInfo& assetInfo = HXChain::getInstance()->assetInfoMap.value( HXChain::getInstance()->getAssetId(symbol));
        ExchangeBalance balance = HXChain::getInstance()->assetExchangeBalanceMap.value(symbol);
        ui->balancesTableWidget->setItem( i, 1, new QTableWidgetItem( getBigNumberString( balance.available, assetInfo.precision)));
        ui->balancesTableWidget->setItem( i, 2, new QTableWidgetItem( getBigNumberString( balance.locked, assetInfo.precision)));
        ui->balancesTableWidget->setItem( i, 3, new QTableWidgetItem( tr("deposit")));
        ui->balancesTableWidget->setItem( i, 4, new QTableWidgetItem( tr("withdraw")));
        ui->balancesTableWidget->setItem( i, 5, new QTableWidgetItem( tr("history")));


        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->balancesTableWidget->item(i,0)->text());
        ui->balancesTableWidget->setCellWidget(i, 0, assetIconItem);

        for(int j : {3,4,5})
        {
            if(ui->balancesTableWidget->item(i,j))
            {
                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->balancesTableWidget->item(i,j)->text());
                ui->balancesTableWidget->setCellWidget(i,j,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ExchangeBalancesWidget::on_balancesTableWidget_cellClicked,this,i,j));

                if(j == 5)  toolButton->setEnabled(false);
            }
        }
    }

    tableWidgetSetItemZebraColor(ui->balancesTableWidget);

}

void ExchangeBalancesWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void ExchangeBalancesWidget::on_balancesTableWidget_cellClicked(int row, int column)
{
    if(column == 3)
    {
        if(ui->balancesTableWidget->item(row,0))
        {
            DepositExchangeContractDialog depositExchangeContractDialog(true);
            depositExchangeContractDialog.setCurrentAsset( ui->balancesTableWidget->item(row,0)->text());
            depositExchangeContractDialog.pop();
        }

        return;
    }

    if(column == 4)
    {
        if(ui->balancesTableWidget->item(row,0))
        {
            WithdrawExchangeContractDialog withdrawExchangeContractDialog(true);
            withdrawExchangeContractDialog.setCurrentAsset(ui->balancesTableWidget->item(row,0)->text());
            withdrawExchangeContractDialog.pop();
        }

    }
}

void ExchangeBalancesWidget::on_hideZeroCheckBox_stateChanged(int arg1)
{
    int rowCount = ui->balancesTableWidget->rowCount();
    if(ui->hideZeroCheckBox->isChecked())
    {
        for(int i = 0; i < rowCount; i++)
        {
            if( ui->balancesTableWidget->item( i, 1)->text().toDouble() == 0
                    & ui->balancesTableWidget->item( i, 2)->text().toDouble() == 0)
            {
                ui->balancesTableWidget->hideRow(i);
            }
            else
            {
                ui->balancesTableWidget->showRow(i);
            }
        }
    }
    else
    {
        for(int i = 0; i < rowCount; i++)
        {
            ui->balancesTableWidget->showRow(i);
        }
    }
}
