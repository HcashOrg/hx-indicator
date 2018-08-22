#include "CitizenAccountPage.h"
#include "ui_CitizenAccountPage.h"

#include "wallet.h"
#include "control/AssetIconItem.h"

CitizenAccountPage::CitizenAccountPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CitizenAccountPage)
{
    ui->setupUi(this);

    ui->lockBalanceTableWidget->installEventFilter(this);
    ui->lockBalanceTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->lockBalanceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->lockBalanceTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->lockBalanceTableWidget->setFrameShape(QFrame::NoFrame);
    ui->lockBalanceTableWidget->setMouseTracking(true);
    ui->lockBalanceTableWidget->setShowGrid(false);//隐藏表格线

    ui->lockBalanceTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->lockBalanceTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->lockBalanceTableWidget->horizontalHeader()->setVisible(true);
    ui->lockBalanceTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->lockBalanceTableWidget->setColumnWidth(0,140);
    ui->lockBalanceTableWidget->setColumnWidth(1,490);
    ui->lockBalanceTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    init();
}

CitizenAccountPage::~CitizenAccountPage()
{
    delete ui;
}

void CitizenAccountPage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->getMyCitizens();
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
        ui->idLabel->hide();
        ui->idLabel2->hide();

        QLabel* label = new QLabel(this);
        label->setGeometry(QRect(ui->label->pos(), QSize(300,30)));
        label->setText(tr("There are no citizen accounts in the wallet."));
    }
}

void CitizenAccountPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void CitizenAccountPage::showLockBalance()
{
    MinerInfo minerInfo = HXChain::getInstance()->minerMap.value(ui->accountComboBox->currentText());
    int size = minerInfo.lockBalances.size();
    ui->lockBalanceTableWidget->setRowCount(0);
    ui->lockBalanceTableWidget->setRowCount(size);
    for(int i = 0; i < size; i++)
    {
        AssetAmount aa = minerInfo.lockBalances.at(i);
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(aa.assetId);
        ui->lockBalanceTableWidget->setItem( i, 0, new QTableWidgetItem( assetInfo.symbol));
        ui->lockBalanceTableWidget->setItem( i, 1, new QTableWidgetItem( getBigNumberString(aa.amount, assetInfo.precision)));

        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->lockBalanceTableWidget->item(i,0)->text());
        ui->lockBalanceTableWidget->setCellWidget(i, 0, assetIconItem);
    }

    tableWidgetSetItemZebraColor(ui->lockBalanceTableWidget);

}

void CitizenAccountPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    MinerInfo minerInfo = HXChain::getInstance()->minerMap.value(ui->accountComboBox->currentText());
    ui->idLabel->setText(minerInfo.minerId);

    showLockBalance();
}
