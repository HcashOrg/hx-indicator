#include "MultiSigPage.h"
#include "ui_MultiSigPage.h"

#include <QClipboard>

#include "CreateMultisigWidget.h"
#include "ToolButtonWidget.h"
#include "control/AssetIconItem.h"
#include "MultiSigTransferWidget.h"

MultiSigPage::MultiSigPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiSigPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->balanceTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->balanceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->balanceTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->balanceTableWidget->setFrameShape(QFrame::NoFrame);
    ui->balanceTableWidget->setMouseTracking(true);
    ui->balanceTableWidget->setShowGrid(false);//隐藏表格线

    ui->balanceTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->balanceTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->balanceTableWidget->horizontalHeader()->setVisible(true);
    ui->balanceTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->balanceTableWidget->setColumnWidth(0,140);
    ui->balanceTableWidget->setColumnWidth(1,180);
    ui->balanceTableWidget->setColumnWidth(2,160);
    ui->balanceTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->infoTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->infoTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->infoTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->infoTableWidget->setFrameShape(QFrame::NoFrame);
    ui->infoTableWidget->setMouseTracking(true);
    ui->infoTableWidget->setShowGrid(false);//隐藏表格线

    ui->infoTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->infoTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->infoTableWidget->horizontalHeader()->setVisible(true);
    ui->infoTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->infoTableWidget->setColumnWidth(0,400);
    ui->infoTableWidget->setColumnWidth(1,180);
    ui->infoTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->typeBalanceBtn->setCheckable(true);
    ui->typeInfoBtn->setCheckable(true);
    ui->typeSignBtn->setCheckable(true);
    ui->typeBalanceBtn->adjustSize();
    ui->typeBalanceBtn->resize(ui->typeBalanceBtn->width(), 18);
    ui->typeInfoBtn->adjustSize();
    ui->typeInfoBtn->resize(ui->typeInfoBtn->width(), 18);
    ui->typeInfoBtn->move(ui->typeBalanceBtn->x() + ui->typeBalanceBtn->width() + 30, ui->typeInfoBtn->y());
    ui->typeSignBtn->adjustSize();
    ui->typeSignBtn->resize(ui->typeSignBtn->width(), 18);
    ui->typeSignBtn->move(ui->typeInfoBtn->x() + ui->typeInfoBtn->width() + 30, ui->typeSignBtn->y());

    setStyleSheet(PUSHBUTTON_CHECK_STYLE);

    init();
}

MultiSigPage::~MultiSigPage()
{
    delete ui;
}

void MultiSigPage::fetchMultisigInfo()
{
    HXChain::getInstance()->postRPC( "MultiSigPage-get_multisig_address-" + ui->multiSigAddressComboBox->currentText(),
                                     toJsonFormat( "get_multisig_address", QJsonArray() << ui->multiSigAddressComboBox->currentText() ));

}

void MultiSigPage::showMultiSigInfo()
{
    MultiSigInfo info = multiSigInfoMap.value(ui->multiSigAddressComboBox->currentText());
    ui->requiredLabel->setText(tr("Required / Total   %1 / %2").arg(info.required).arg(info.pubKeys.size()));

    ui->infoTableWidget->setRowCount(0);
    ui->infoTableWidget->setRowCount(info.pubKeys.size());

    for(int i = 0; i < info.pubKeys.size(); i++)
    {
        ui->infoTableWidget->setRowHeight(i,40);

        ui->infoTableWidget->setItem( i, 0, new QTableWidgetItem( info.pubKeys.at(i)));
    }
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
    ui->balanceTableWidget->setRowCount(0);
    ui->balanceTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->balanceTableWidget->setRowHeight(i,40);

        QString assetId = assetIds.at(i);
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetId);

        //资产名
        QString symbol = assetInfo.symbol;
        ui->balanceTableWidget->setItem(i,0,new QTableWidgetItem(symbol));
        ui->balanceTableWidget->item(i,0)->setData(Qt::UserRole, assetId);

        //数量
        ui->balanceTableWidget->setItem(i,1,new QTableWidgetItem(getBigNumberString(map.value(assetId).amount, assetInfo.precision)));

        ui->balanceTableWidget->setItem(i,2,new QTableWidgetItem(tr("transfer")));

        for (int j : {2})
        {
            if(ui->balanceTableWidget->item(i,j))
            {
                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->balanceTableWidget->item(i,j)->text());
                ui->balanceTableWidget->setCellWidget(i,j,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&MultiSigPage::on_balanceTableWidget_cellClicked,this,i,j));
            }
        }


        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->balanceTableWidget->item(i,0)->text());
        ui->balanceTableWidget->setCellWidget(i, 0, assetIconItem);

    }

    tableWidgetSetItemZebraColor(ui->balanceTableWidget);
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


    if( id == "MultiSigPage-get_multisig_address-" + ui->multiSigAddressComboBox->currentText() )
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

        showMultiSigInfo();
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

    ui->typeBalanceBtn->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);

    fetchMultisigInfo();
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

void MultiSigPage::on_balanceTableWidget_cellClicked(int row, int column)
{
    if(column == 2)
    {
        if(ui->balanceTableWidget->item(row,column))
        {
            emit backBtnVisible(true);

            MultiSigTransferWidget* multiSigTransferWidget = new MultiSigTransferWidget(this);
            multiSigTransferWidget->setAttribute(Qt::WA_DeleteOnClose);
            multiSigTransferWidget->show();
            multiSigTransferWidget->raise();
            multiSigTransferWidget->setFromAddress(ui->multiSigAddressComboBox->currentText());
            multiSigTransferWidget->setAsset(ui->balanceTableWidget->item(row,column)->data(Qt::UserRole).toString());
        }

    }
}

void MultiSigPage::on_typeBalanceBtn_clicked()
{
    ui->typeBalanceBtn->setChecked(true);
    ui->typeInfoBtn->setChecked(false);
    ui->typeSignBtn->setChecked(false);

    ui->stackedWidget->setCurrentIndex(0);
}

void MultiSigPage::on_typeInfoBtn_clicked()
{
    ui->typeBalanceBtn->setChecked(false);
    ui->typeInfoBtn->setChecked(true);
    ui->typeSignBtn->setChecked(false);

    ui->stackedWidget->setCurrentIndex(1);
}

void MultiSigPage::on_typeSignBtn_clicked()
{
    ui->typeBalanceBtn->setChecked(false);
    ui->typeInfoBtn->setChecked(false);
    ui->typeSignBtn->setChecked(true);
}

void MultiSigPage::on_multiSigAddressComboBox_currentIndexChanged(const QString &arg1)
{
    fetchBalances();
}

void MultiSigPage::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->multiSigAddressComboBox->currentText());
}
