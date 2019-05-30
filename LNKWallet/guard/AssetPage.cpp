#include "AssetPage.h"
#include "ui_AssetPage.h"

#include "wallet.h"
#include "control/AssetIconItem.h"
#include "showcontentdialog.h"
#include "ToolButtonWidget.h"

static const int ROWNUMBER = 7;
AssetPage::AssetPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssetPage)
{
    ui->setupUi(this);

    ui->assetTableWidget->installEventFilter(this);
    ui->assetTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->assetTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->assetTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->assetTableWidget->setFrameShape(QFrame::NoFrame);
    ui->assetTableWidget->setMouseTracking(true);
    ui->assetTableWidget->setShowGrid(false);//隐藏表格线

    ui->assetTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->assetTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->assetTableWidget->horizontalHeader()->setVisible(true);
    ui->assetTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->assetTableWidget->setColumnWidth(0,120);
    ui->assetTableWidget->setColumnWidth(1,80);
    ui->assetTableWidget->setColumnWidth(2,140);
    ui->assetTableWidget->setColumnWidth(3,140);
    ui->assetTableWidget->setColumnWidth(4,80);
    ui->assetTableWidget->setColumnWidth(5,80);
    ui->assetTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->assetTableWidget);

    pageWidget = new PageScrollWidget();
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&AssetPage::pageChangeSlot);

    blankWidget = new BlankDefaultWidget(ui->assetTableWidget);
    blankWidget->setTextTip(tr("There are no records currently!"));

    showAssetsInfo();
}

AssetPage::~AssetPage()
{
    delete ui;
}

void AssetPage::showAssetsInfo()
{
    QStringList assetIds = HXChain::getInstance()->assetInfoMap.keys();
    assetIds.removeAll("1.3.0");    // 不显示HX

    int size = assetIds.size();
    ui->assetTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->assetTableWidget->setRowHeight(i,40);

        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetIds.at(i));

        //资产名
        QString symbol = assetInfo.symbol;
        ui->assetTableWidget->setItem(i,0,new QTableWidgetItem(symbol));
        ui->assetTableWidget->setItem(i,1,new QTableWidgetItem(QString::number(assetInfo.precision)));
        ui->assetTableWidget->setItem(i,2,new QTableWidgetItem(assetInfo.hotAddress));
        ui->assetTableWidget->setItem(i,3,new QTableWidgetItem(assetInfo.coldAddress));
        ui->assetTableWidget->setItem(i,4,new QTableWidgetItem(HXChain::getInstance()->guardAccountIdToName(assetInfo.issuer)));
        ui->assetTableWidget->setItem(i,5,new QTableWidgetItem(getBigNumberString(assetInfo.fee, assetInfo.precision) + " " + symbol));

        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->assetTableWidget->item(i,0)->text());
        ui->assetTableWidget->setCellWidget(i, 0, assetIconItem);
    }

    tableWidgetSetItemZebraColor(ui->assetTableWidget);
    int page = (ui->assetTableWidget->rowCount()%ROWNUMBER==0 && ui->assetTableWidget->rowCount() != 0) ?
                ui->assetTableWidget->rowCount()/ROWNUMBER : ui->assetTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(ui->assetTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(pageWidget->GetCurrentPage());

    pageWidget->setVisible(0 != ui->assetTableWidget->rowCount());
    blankWidget->setVisible(ui->assetTableWidget->rowCount() == 0);
}

void AssetPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void AssetPage::on_assetTableWidget_cellPressed(int row, int column)
{
    if( column == 2 || column == 3 )
    {
        ShowContentDialog showContentDialog( ui->assetTableWidget->item(row, column)->text(),this);

        int x = ui->assetTableWidget->columnViewportPosition(column) + ui->assetTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->assetTableWidget->rowViewportPosition(row) - 10 + ui->assetTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->assetTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void AssetPage::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->assetTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->assetTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->assetTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->assetTableWidget->setRowHidden(i,true);
        }
    }
}
