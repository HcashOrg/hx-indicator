#include "MultiSigPage.h"
#include "ui_MultiSigPage.h"


#include "CreateMultisigWidget.h"
#include "ToolButtonWidget.h"
#include "control/AssetIconItem.h"

MultiSigPage::MultiSigPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiSigPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->multisigTableWidget->installEventFilter(this);
    ui->multisigTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->multisigTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->multisigTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->multisigTableWidget->setFrameShape(QFrame::NoFrame);
    ui->multisigTableWidget->setMouseTracking(true);
    ui->multisigTableWidget->setShowGrid(false);//隐藏表格线

    ui->multisigTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->multisigTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->multisigTableWidget->horizontalHeader()->setVisible(true);
    ui->multisigTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->multisigTableWidget->setColumnWidth(0,140);
    ui->multisigTableWidget->setColumnWidth(1,180);
    ui->multisigTableWidget->setColumnWidth(2,80);
    ui->multisigTableWidget->setColumnWidth(3,80);
    ui->multisigTableWidget->horizontalHeader()->setStretchLastSection(true);


    init();
}

MultiSigPage::~MultiSigPage()
{
    delete ui;
}

void MultiSigPage::fetchMultisigInfo()
{
    HXChain::getInstance()->configFile->beginGroup("/multisigAddresses");
    QStringList keys = HXChain::getInstance()->configFile->childKeys();
    HXChain::getInstance()->configFile->endGroup();

    foreach (QString key, keys)
    {
        HXChain::getInstance()->postRPC( "MultiSigPage-get_multisig_address-" + key,
                                         toJsonFormat( "get_multisig_address", QJsonArray() << key ));

    }
    HXChain::getInstance()->postRPC( "finish-get_multisig_address",
                                     toJsonFormat( "finish-get_multisig_address", QJsonArray()  ));

}


void MultiSigPage::fetchBalances()
{
    HXChain::getInstance()->postRPC( "MultiSigPage-get_addr_balances-" + ui->multiSigAddressComboBox->currentText(),
                                     toJsonFormat( "get_addr_balances", QJsonArray() << ui->multiSigAddressComboBox->currentText() ));
}

void MultiSigPage::showBalances()
{
    AssetAmountMap map = multiSigBalancesMap.value(ui->multiSigAddressComboBox->currentText());
    QStringList assetIds = map.keys();
    int size = assetIds.size();
    ui->multisigTableWidget->setRowCount(0);
    ui->multisigTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->multisigTableWidget->setRowHeight(i,40);

        QString assetId = assetIds.at(i);
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetId);

        //资产名
        QString symbol = assetInfo.symbol;
        ui->multisigTableWidget->setItem(i,0,new QTableWidgetItem(symbol));

        //数量
        ui->multisigTableWidget->setItem(i,1,new QTableWidgetItem(getBigNumberString(map.value(assetId).amount, assetInfo.precision)));

        ui->multisigTableWidget->setItem(i,2,new QTableWidgetItem(tr("details")));
        ui->multisigTableWidget->setItem(i,3,new QTableWidgetItem(tr("transfer")));

        for (int j : {2,3})
        {
            if(ui->multisigTableWidget->item(i,j))
            {
                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->multisigTableWidget->item(i,j)->text());
                ui->multisigTableWidget->setCellWidget(i,j,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&MultiSigPage::on_multisigTableWidget_cellClicked,this,i,j));
            }
        }


        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->multisigTableWidget->item(i,0)->text());
        ui->multisigTableWidget->setCellWidget(i, 0, assetIconItem);

    }

    tableWidgetSetItemZebraColor(ui->multisigTableWidget);
}

void MultiSigPage::jsonDataUpdated(QString id)
{
        if( id.startsWith( "MultiSigPage-get_addr_balances-") )
        {
            QString result = HXChain::getInstance()->jsonDataValue(id);
            QString address = id.mid(QString("MultiSigPage-get_addr_balances-").size());
            qDebug() << id << result;
            result.prepend("{");
            result.append("}");
            QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
            QJsonArray array = object.value("result").toArray();
            AssetAmountMap map;
            foreach (QJsonValue v, array)
            {
                QJsonObject assetObject = v.toObject();
                AssetAmount aa;
                aa.amount = jsonValueToULL(assetObject.value("amount"));
                aa.assetId = assetObject.value("asset_id").toString();

                map.insert(aa.assetId, aa);
            }

            multiSigBalancesMap.insert(address, map);

            showBalances();
            return;
        }


//    if( id == "MultiSigPage-get_pubkey_from_account-" + ui->accountComboBox->currentText() )
//    {
//        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;
//        result.prepend("{");
//        result.append("}");
//        QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
//        QString pubKey = object.value("result").toString();
//        ui->pubKeyLabel->setText(pubKey);
//        return;
//    }


    if( id.startsWith("MultiSigPage-get_multisig_address-") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        result.prepend("{");
        result.append("}");
        QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
        QJsonObject resultObject = object.value("result").toObject();
        MultiSigInfo info;
        info.owner = resultObject.value("owner").toString();

        QJsonArray array = resultObject.value("multisignatures").toArray().at(0).toArray();
        info.required = array.at(0).toInt();

        QJsonArray array2 = array.at(1).toArray();
        foreach (QJsonValue v, array2)
        {
            info.pubKeys += v.toString();
        }
        multiSigInfoMap.insert(info.owner, info);
        return;
    }

    if( id == "finish-get_multisig_address")
    {


        return;
    }

}

void MultiSigPage::on_createMultiSigBtn_clicked()
{
    emit backBtnVisible(true);

    CreateMultisigWidget* createMultisigWidget = new CreateMultisigWidget( this);
    createMultisigWidget->setAttribute(Qt::WA_DeleteOnClose);
    createMultisigWidget->show();
    createMultisigWidget->raise();
}

void MultiSigPage::init()
{
    ui->multiSigAddressComboBox->clear();
    HXChain::getInstance()->configFile->beginGroup("/multisigAddresses");
    QStringList keys = HXChain::getInstance()->configFile->childKeys();
    HXChain::getInstance()->configFile->endGroup();
    ui->multiSigAddressComboBox->addItems(keys);

//    fetchMultisigInfo();
    fetchBalances();
}

void MultiSigPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

//void MultiSigPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
//{
//    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
//    ui->addressLabel->setText(info.address);

//    HXChain::getInstance()->postRPC( "MultiSigPage-get_pubkey_from_account-" + ui->accountComboBox->currentText(),
//                                     toJsonFormat( "get_pubkey_from_account", QJsonArray() << ui->accountComboBox->currentText() ));
//}

void MultiSigPage::on_multisigTableWidget_cellClicked(int row, int column)
{

}
