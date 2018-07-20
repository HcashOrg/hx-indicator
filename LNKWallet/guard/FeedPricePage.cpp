#include "FeedPricePage.h"
#include "ui_FeedPricePage.h"

#include "wallet.h"
#include "control/AssetIconItem.h"
#include "ToolButtonWidget.h"
#include "FeedPriceDialog.h"

FeedPricePage::FeedPricePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeedPricePage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->assetPriceTableWidget->installEventFilter(this);
    ui->assetPriceTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->assetPriceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->assetPriceTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->assetPriceTableWidget->setFrameShape(QFrame::NoFrame);
    ui->assetPriceTableWidget->setMouseTracking(true);
    ui->assetPriceTableWidget->setShowGrid(false);//隐藏表格线

    ui->assetPriceTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->assetPriceTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->assetPriceTableWidget->horizontalHeader()->setVisible(true);
    ui->assetPriceTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->assetPriceTableWidget->setColumnWidth(0,140);
    ui->assetPriceTableWidget->setColumnWidth(1,210);
    ui->assetPriceTableWidget->setColumnWidth(2,210);
    ui->assetPriceTableWidget->setColumnWidth(3,100);
    ui->assetPriceTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    init();
}

FeedPricePage::~FeedPricePage()
{
    delete ui;
}

void FeedPricePage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->getMyFormalGuards();
    if(accounts.size() > 0)
    {
        ui->accountComboBox->addItems(accounts);

        if(accounts.contains(HXChain::getInstance()->currentAccount))
        {
            ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
        }
    }
    else
    {
        ui->label->hide();
        ui->accountComboBox->hide();

        QLabel* label = new QLabel(this);
        label->setGeometry(QRect(ui->accountComboBox->pos(), QSize(300,30)));
        label->setText(tr("There are no guard accounts in the wallet."));
    }

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->assetPriceTableWidget);


    showAssetsPrice();
}

void FeedPricePage::showAssetsPrice()
{
    QStringList keys = HXChain::getInstance()->assetInfoMap.keys();
    keys.removeAll("1.3.0");
    int size = keys.size();
    ui->assetPriceTableWidget->setRowCount(0);
    ui->assetPriceTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->assetPriceTableWidget->setRowHeight(i,40);

        AssetInfo info = HXChain::getInstance()->assetInfoMap.value(keys.at(i));

        ui->assetPriceTableWidget->setItem(i, 0, new QTableWidgetItem(info.symbol));
        ui->assetPriceTableWidget->setItem(i, 1, new QTableWidgetItem(info.currentFeedTime.replace("T"," ")));
        ui->assetPriceTableWidget->setItem(i, 3, new QTableWidgetItem(tr("quote")));

        QString str = QString("%1 : %2").arg( getBigNumberString(info.quoteAmount.amount, ASSET_PRECISION))
                .arg( getBigNumberString(info.baseAmount.amount, info.precision));
        ui->assetPriceTableWidget->setItem(i, 2, new QTableWidgetItem(str));


        QString itemColor = (i % 2) ?"rgb(252,253,255)":"white";

        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->assetPriceTableWidget->item(i,0)->text());
        assetIconItem->setBackgroundColor(itemColor);
        ui->assetPriceTableWidget->setCellWidget(i, 0, assetIconItem);

        ToolButtonWidget *toolButton = new ToolButtonWidget();
        toolButton->setText(ui->assetPriceTableWidget->item(i,3)->text());
        toolButton->setBackgroundColor(itemColor);
        ui->assetPriceTableWidget->setCellWidget(i,3,toolButton);
        connect(toolButton,&ToolButtonWidget::clicked,std::bind(&FeedPricePage::on_assetPriceTableWidget_cellClicked,this,i,3));


        for (int j : {1,2})
        {
            if(i%2)
            {
                ui->assetPriceTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                ui->assetPriceTableWidget->item(i,j)->setBackgroundColor(QColor(252,253,255));
            }
            else
            {
                ui->assetPriceTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                ui->assetPriceTableWidget->item(i,j)->setBackgroundColor(QColor("white"));
            }
        }
    }
}

void FeedPricePage::refresh()
{
    showAssetsPrice();
}

void FeedPricePage::jsonDataUpdated(QString id)
{

}

void FeedPricePage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}

void FeedPricePage::on_assetPriceTableWidget_cellClicked(int row, int column)
{
    if(column == 3)
    {
        FeedPriceDialog feedPriceDialog(ui->assetPriceTableWidget->item(row,0)->text());
        feedPriceDialog.pop();

        refresh();
    }
}
