#include "OnchainOrderPage.h"
#include "ui_OnchainOrderPage.h"

#include "wallet.h"

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
    QStringList assetIds = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
        ui->assetComboBox2->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
    }

    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","Zchain.Exchange.queryContracts");
    QJsonObject paramObject;
    paramObject.insert("from_asset","BTC");
    paramObject.insert("to_asset","LNK");
    paramObject.insert("limit",10);
    object.insert("params",paramObject);
    httpManager.post("http://192.168.1.121:5005/api",QJsonDocument(object).toJson());
}

void OnchainOrderPage::jsonDataUpdated(QString id)
{

}

void OnchainOrderPage::httpReplied(QByteArray _data, int _status)
{
//    qDebug() << "rrrrrrrrrrrrrr " << _data << _status;

//    QJsonObject object  = QJsonDocument::fromJson(_data).object();
//    QJsonArray  array   = object.take("result").toObject().take("data").toArray();

//    int size = array.size();
//    ui->ordersTableWidget->setRowCount(size);
//    for(int i = 0; i < size; i++)
//    {
//        QJsonObject dataObject = array.at(i);

//    }
}

void OnchainOrderPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}
