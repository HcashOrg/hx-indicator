#include "OnchainOrderPage.h"
#include "ui_OnchainOrderPage.h"

#include "wallet.h"
#include "BuyOrderWidget.h"

#include <QtMath>

OnchainOrderPage::OnchainOrderPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnchainOrderPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

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
}

void OnchainOrderPage::jsonDataUpdated(QString id)
{

}

void OnchainOrderPage::httpReplied(QByteArray _data, int _status)
{
    qDebug() << "rrrrrrrrrrrrrr " << _data << _status;

    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    QJsonArray  array   = object.take("result").toObject().take("data").toArray();

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
        ui->ordersTableWidget->setItem(i,1, new QTableWidgetItem(getBigNumberString(buyAmount,buyAssetInfo.precision)));

        double price = (double)sellAmount / qPow(10,sellAssetInfo.precision) / buyAmount * qPow(10,buyAssetInfo.precision);
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(price,'g',8));
        item->setData(Qt::UserRole,contractAddress);
        ui->ordersTableWidget->setItem(i,2,item);

        ui->ordersTableWidget->setItem(i,3, new QTableWidgetItem(tr("buy")));
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
    if(ui->assetComboBox->currentText().isEmpty() || ui->assetComboBox2->currentText().isEmpty()
            || ui->assetComboBox->currentText() == ui->assetComboBox2->currentText())       return;

    queryContractOrders();
}

void OnchainOrderPage::on_assetComboBox2_currentIndexChanged(const QString &arg1)
{
    ui->ordersTableWidget->setRowCount(0);
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
    httpManager.post("http://192.168.1.121:5005/api",QJsonDocument(object).toJson());
}


void OnchainOrderPage::on_ordersTableWidget_cellPressed(int row, int column)
{
    if(column == 3)
    {
        BuyOrderWidget* buyOrderWidget = new BuyOrderWidget(this);
        buyOrderWidget->setAccount(ui->accountComboBox->currentText());
        buyOrderWidget->setSellAsset(ui->assetComboBox->currentText());
        buyOrderWidget->setBuyAsset(ui->assetComboBox2->currentText());
        buyOrderWidget->move(0,0);
        buyOrderWidget->show();
        buyOrderWidget->raise();

        return;
    }
}
