#include "withdrawrecordwidget.h"
#include "ui_withdrawrecordwidget.h"

#include "wallet.h"

WithdrawRecordWidget::WithdrawRecordWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WithdrawRecordWidget)
{
    ui->setupUi(this);

    ui->withdrawRecordTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->withdrawRecordTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->withdrawRecordTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->withdrawRecordTableWidget->setFrameShape(QFrame::NoFrame);
    ui->withdrawRecordTableWidget->setMouseTracking(true);
    ui->withdrawRecordTableWidget->setShowGrid(false);//隐藏表格线

    ui->withdrawRecordTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->withdrawRecordTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->withdrawRecordTableWidget->horizontalHeader()->setVisible(true);
    ui->withdrawRecordTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);


//    ui->withdrawRecordTableWidget->setColumnWidth(0,90);
//    ui->withdrawRecordTableWidget->setColumnWidth(1,70);
//    ui->withdrawRecordTableWidget->setColumnWidth(2,100);
//    ui->withdrawRecordTableWidget->setColumnWidth(3,100);
//    ui->withdrawRecordTableWidget->setColumnWidth(4,110);
//    ui->withdrawRecordTableWidget->setColumnWidth(5,70);

    init();
}

WithdrawRecordWidget::~WithdrawRecordWidget()
{
    delete ui;
}

void WithdrawRecordWidget::init()
{
    ui->assetComboBox->addItem(tr("ALL"), "ALL");
    QStringList assetIds = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
    }
}

void WithdrawRecordWidget::showWithdrawRecord(QString _accountAddress, QString _assetId)
{
    accountAddress = _accountAddress;
    TransactionTypeIds typeIds = UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(_accountAddress,TRANSACTION_TYPE_WITHDRAW);

    int size = typeIds.size();
    ui->withdrawRecordTableWidget->setRowCount(0);
    int rowCount = 0;

    for(int i = 0; i < size; i++)
    {
        QString transactionId = typeIds.at(size - i - 1).transactionId;
        TransactionStruct ts = UBChain::getInstance()->transactionDB.getTransactionStruct(transactionId);
        if(ts.type == -1)
        {
            qDebug() << "can not find transaction in transactionstruct db: " << transactionId;
            continue;
        }

        QJsonObject object = QJsonDocument::fromJson(ts.operationStr.toLatin1()).object();
        QString amountStr = object.take("amount").toString();
        QString amountAssetId = object.take("asset_id").toString();
        QString crossChainAddress = object.take("crosschain_account").toString();
        AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(amountAssetId);

        if(_assetId != "ALL"  &&  amountAssetId != _assetId)    continue;


        ui->withdrawRecordTableWidget->setRowCount(rowCount + 1);

        ui->withdrawRecordTableWidget->setItem(rowCount,0, new QTableWidgetItem(QString::number(ts.blockNum)));
        ui->withdrawRecordTableWidget->setItem(rowCount,4, new QTableWidgetItem(transactionId));



        ui->withdrawRecordTableWidget->setItem(rowCount,1, new QTableWidgetItem(crossChainAddress));

        // 如果是转出
        QTableWidgetItem* item = new QTableWidgetItem( amountStr + " " + assetInfo.symbol);
        ui->withdrawRecordTableWidget->setItem(rowCount,2, item);
        item->setTextColor(QColor(255,0,0));


        QJsonObject feeObject = object.take("fee").toObject();
        unsigned long long feeAmount = jsonValueToULL(feeObject.take("amount"));
        QString feeAssetId = feeObject.take("asset_id").toString();
        AssetInfo feeAssetInfo = UBChain::getInstance()->assetInfoMap.value(feeAssetId);
        ui->withdrawRecordTableWidget->setItem(rowCount,3, new QTableWidgetItem(getBigNumberString(feeAmount, feeAssetInfo.precision)));

        ui->withdrawRecordTableWidget->setItem(rowCount,5, new QTableWidgetItem(tr("confirmed")));

        rowCount++;
    }
}

void WithdrawRecordWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));
    painter.drawRect(0,0,770,530);
}

void WithdrawRecordWidget::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    showWithdrawRecord(accountAddress, ui->assetComboBox->currentData().toString());

}
