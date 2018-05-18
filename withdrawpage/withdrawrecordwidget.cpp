#include "withdrawrecordwidget.h"
#include "ui_withdrawrecordwidget.h"

#include "wallet.h"
#include "poundage/PageScrollWidget.h"
#include "showcontentdialog.h"

static const int ROWNUMBER = 5;

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
    ui->withdrawRecordTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->withdrawRecordTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);


//    ui->withdrawRecordTableWidget->setColumnWidth(0,90);
//    ui->withdrawRecordTableWidget->setColumnWidth(1,70);
//    ui->withdrawRecordTableWidget->setColumnWidth(2,100);
//    ui->withdrawRecordTableWidget->setColumnWidth(3,100);
//    ui->withdrawRecordTableWidget->setColumnWidth(4,110);
//    ui->withdrawRecordTableWidget->setColumnWidth(5,70);

    pageWidget = new PageScrollWidget();
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
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&WithdrawRecordWidget::pageChangeSlot);

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
        QTableWidgetItem* item = new QTableWidgetItem( "-" + amountStr + " " + assetInfo.symbol);
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
    //设置表格内容居中
    for(int i = 0;i < ui->withdrawRecordTableWidget->rowCount();++i)
    {
        for(int j = 0;j < ui->withdrawRecordTableWidget->columnCount();++j)
        {
            ui->withdrawRecordTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }

    }
    int page = (ui->withdrawRecordTableWidget->rowCount()%ROWNUMBER==0 && ui->withdrawRecordTableWidget->rowCount() != 0) ?
                ui->withdrawRecordTableWidget->rowCount()/ROWNUMBER : ui->withdrawRecordTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageChangeSlot(0);

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

void WithdrawRecordWidget::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->withdrawRecordTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->withdrawRecordTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->withdrawRecordTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->withdrawRecordTableWidget->setRowHidden(i,true);
        }
    }

}


void WithdrawRecordWidget::on_withdrawRecordTableWidget_cellPressed(int row, int column)
{
    if( column == 1 )
    {
        ShowContentDialog showContentDialog( ui->withdrawRecordTableWidget->item(row, column)->text(),this);

        int x = ui->withdrawRecordTableWidget->columnViewportPosition(column) + ui->withdrawRecordTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->withdrawRecordTableWidget->rowViewportPosition(row) - 10 + ui->withdrawRecordTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->withdrawRecordTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if( column == 4 )
    {
        ShowContentDialog showContentDialog( ui->withdrawRecordTableWidget->item(row, column)->text(),this);

        int x = ui->withdrawRecordTableWidget->columnViewportPosition(column) + ui->withdrawRecordTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->withdrawRecordTableWidget->rowViewportPosition(row) - 10 + ui->withdrawRecordTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->withdrawRecordTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}
