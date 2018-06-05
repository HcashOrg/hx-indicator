#include "OnchainOrderPage.h"
#include "ui_OnchainOrderPage.h"

#include "wallet.h"
#include "commondialog.h"
#include "BuyOrderWidget.h"
#include "ToolButtonWidget.h"

#include <QtMath>

OnchainOrderPage::OnchainOrderPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnchainOrderPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->ordersTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->ordersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ordersTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->ordersTableWidget->setFrameShape(QFrame::NoFrame);
    ui->ordersTableWidget->setMouseTracking(true);
    ui->ordersTableWidget->setShowGrid(false);//隐藏表格线
    ui->ordersTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->ordersTableWidget->horizontalHeader()->setFixedHeight(35);
    ui->ordersTableWidget->horizontalHeader()->setVisible(true);
    ui->ordersTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->ordersTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->ordersTableWidget->verticalHeader()->setVisible(false);
    ui->ordersTableWidget->setColumnWidth(0,190);
    ui->ordersTableWidget->setColumnWidth(1,180);
    ui->ordersTableWidget->setColumnWidth(2,130);
    ui->ordersTableWidget->setColumnWidth(3,90);

    ui->ordersTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    init();
}

OnchainOrderPage::~OnchainOrderPage()
{
    delete ui;
}

void OnchainOrderPage::init()
{
    QStringList accounts = UBChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    QStringList assetIds = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
        ui->assetComboBox2->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
    }

    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    inited = true;
}

void OnchainOrderPage::onBack()
{
    if(currentWidget)
    {
        currentWidget->close();
        currentWidget = NULL;
    }
}

void OnchainOrderPage::jsonDataUpdated(QString id)
{

}

void OnchainOrderPage::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    QJsonArray  array   = object.take("result").toObject().take("data").toArray();
qDebug() << "Rrrrrrrrrrrr " << _data;
    int size = array.size();
    ui->ordersTableWidget->setRowCount(size);
    for(int i = 0; i < size; i++)
    {
        QJsonObject dataObject = array.at(i).toObject();
        QString contractAddress = dataObject.take("contract_address").toString();
        unsigned long long sellAmount = jsonValueToULL(dataObject.take("from_supply"));
        QString sellSymbol = dataObject.take("from_asset").toString();
        unsigned long long buyAmount = jsonValueToULL(dataObject.take("to_supply"));
        QString buySymbol = dataObject.take("to_asset").toString();
        int state = dataObject.take("state").toInt();

        AssetInfo sellAssetInfo = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(sellSymbol));
        AssetInfo buyAssetInfo  = UBChain::getInstance()->assetInfoMap.value(UBChain::getInstance()->getAssetId(buySymbol));

        ui->ordersTableWidget->setItem(i,0, new QTableWidgetItem(getBigNumberString(sellAmount,sellAssetInfo.precision)));
        ui->ordersTableWidget->item(i,0)->setData(Qt::UserRole,sellAmount);

        ui->ordersTableWidget->setItem(i,1, new QTableWidgetItem(getBigNumberString(buyAmount,buyAssetInfo.precision)));
        ui->ordersTableWidget->item(i,1)->setData(Qt::UserRole,buyAmount);

        double price = (double)sellAmount / qPow(10,sellAssetInfo.precision) / buyAmount * qPow(10,buyAssetInfo.precision);
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(price,'g',8));
        item->setData(Qt::UserRole,contractAddress);
        ui->ordersTableWidget->setItem(i,2,item);

        ui->ordersTableWidget->setItem(i,3, new QTableWidgetItem(tr("buy")));

        for(int j = 0; j < 4; j++)
        {
            ui->ordersTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
            if(i%2)
            {
                ui->ordersTableWidget->item(i,j)->setBackgroundColor(QColor(255,255,255));
            }
            else
            {
                ui->ordersTableWidget->item(i,j)->setBackgroundColor(QColor(252,253,255));
            }
        }

        for(int j = 3;j < 4;++j)
        {
            ToolButtonWidgetItem *toolButtonItem = new ToolButtonWidgetItem(i,j);
            toolButtonItem->setButtonFixSize(80,20);

            if(UBChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText()) == contractAddress)
            {
                toolButtonItem->setBtnEnabled(false);
                toolButtonItem->setText(tr("my order"));
            }
            else
            {
                toolButtonItem->setBtnEnabled(true);
                toolButtonItem->setText(ui->ordersTableWidget->item(i,j)->text());
            }

            ui->ordersTableWidget->setCellWidget(i,j,toolButtonItem);
            connect(toolButtonItem,SIGNAL(itemClicked(int,int)),this,SLOT(onItemClicked(int,int)));
        }
    }
}

void OnchainOrderPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}

void OnchainOrderPage::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    ui->ordersTableWidget->setRowCount(0);
    updateTableHeaders();
    if(ui->assetComboBox->currentText().isEmpty() || ui->assetComboBox2->currentText().isEmpty()
            || ui->assetComboBox->currentText() == ui->assetComboBox2->currentText())       return;

    queryContractOrders();
}

void OnchainOrderPage::on_assetComboBox2_currentIndexChanged(const QString &arg1)
{
    ui->ordersTableWidget->setRowCount(0);
    updateTableHeaders();
    if(ui->assetComboBox->currentText().isEmpty() || ui->assetComboBox2->currentText().isEmpty()
            || ui->assetComboBox->currentText() == ui->assetComboBox2->currentText())       return;

    queryContractOrders();
}

void OnchainOrderPage::queryContractOrders()
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","Zchain.Exchange.queryContracts");
    QJsonObject paramObject;
    paramObject.insert("from_asset",ui->assetComboBox->currentText());
    paramObject.insert("to_asset",ui->assetComboBox2->currentText());
    paramObject.insert("limit",10);
    object.insert("params",paramObject);
    httpManager.post(UBChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());
}

void OnchainOrderPage::updateTableHeaders()
{
    if(ui->assetComboBox->currentText() == ui->assetComboBox2->currentText())
    {
        ui->ordersTableWidget->horizontalHeaderItem(0)->setText(tr("Sell"));
        ui->ordersTableWidget->horizontalHeaderItem(1)->setText(tr("Buy"));
        ui->ordersTableWidget->horizontalHeaderItem(2)->setText(tr("Price"));
    }
    else
    {
        ui->ordersTableWidget->horizontalHeaderItem(0)->setText(tr("Sell / %1").arg(ui->assetComboBox->currentText()));
        ui->ordersTableWidget->horizontalHeaderItem(1)->setText(tr("Buy / %1").arg(ui->assetComboBox2->currentText()));
        ui->ordersTableWidget->horizontalHeaderItem(2)->setText(tr("Price (%1/%2)").arg(ui->assetComboBox->currentText()).arg(ui->assetComboBox2->currentText()));
    }

}

void OnchainOrderPage::onItemClicked(int _row, int _column)
{
    if(_column == 3)
    {
        if(UBChain::getInstance()->accountInfoMap.empty())
        {
            CommonDialog dia(CommonDialog::OkOnly);
            dia.setText(tr("Please Import Or Create Account First!"));
            dia.pop();
            UBChain::getInstance()->mainFrame->ShowMainPageSlot();
            return;
        }


        if(!UBChain::getInstance()->ValidateOnChainOperation()) return;

        BuyOrderWidget* buyOrderWidget = new BuyOrderWidget(this);
        buyOrderWidget->setAttribute(Qt::WA_DeleteOnClose);
        buyOrderWidget->move(0,0);
        buyOrderWidget->show();
        buyOrderWidget->raise();

        buyOrderWidget->setAccount(ui->accountComboBox->currentText());
        QString contractAddress = ui->ordersTableWidget->item(_row,2)->data(Qt::UserRole).toString();
        buyOrderWidget->setContractAddress(contractAddress);
        buyOrderWidget->setOrderInfo(ui->ordersTableWidget->item(_row,0)->data(Qt::UserRole).toULongLong(), ui->assetComboBox->currentText()
                                     ,ui->ordersTableWidget->item(_row,1)->data(Qt::UserRole).toULongLong(), ui->assetComboBox2->currentText());

        currentWidget = buyOrderWidget;

        emit backBtnVisible(true);

        return;
    }
}

void OnchainOrderPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)  return;

    UBChain::getInstance()->currentAccount = ui->accountComboBox->currentText();

    on_assetComboBox_currentIndexChanged(ui->assetComboBox->currentText());
}
