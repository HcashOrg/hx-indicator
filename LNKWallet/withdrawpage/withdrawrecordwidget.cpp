#include "withdrawrecordwidget.h"
#include "ui_withdrawrecordwidget.h"

#include "wallet.h"
#include "poundage/PageScrollWidget.h"
#include "showcontentdialog.h"
#include "control/BlankDefaultWidget.h"

static const int ROWNUMBER = 7;

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
//    ui->withdrawRecordTableWidget->horizontalHeader()->setFixedHeight(30);
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

    blankWidget = new BlankDefaultWidget(ui->withdrawRecordTableWidget);
    blankWidget->setTextTip(tr("There's no withdraw record!"));

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->withdrawRecordTableWidget);
    init();
}

WithdrawRecordWidget::~WithdrawRecordWidget()
{
    delete ui;
}

void WithdrawRecordWidget::init()
{
    ui->assetComboBox->addItem(tr("ALL"), "ALL");
    QStringList assetIds = HXChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        if(ASSET_NAME == HXChain::getInstance()->assetInfoMap.value(assetId).symbol) continue;
        ui->assetComboBox->addItem( revertERCSymbol(HXChain::getInstance()->assetInfoMap.value(assetId).symbol), assetId);
    }
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&WithdrawRecordWidget::pageChangeSlot);

}

void WithdrawRecordWidget::showWithdrawRecord(QString _accountAddress, QString _assetId)
{
    accountAddress = _accountAddress;
    TransactionTypeIds typeIds = HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(_accountAddress,TRANSACTION_TYPE_WITHDRAW);

    // 根据区块高度排序
    TransactionTypeIds sortedTypeIds;
    for(int i = 0; i < typeIds.size(); i++)
    {
        if(sortedTypeIds.size() == 0)
        {
            sortedTypeIds.append(typeIds.at(i));
            continue;
        }

        TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct(typeIds.at(i).transactionId);
        for(int j = 0; j < sortedTypeIds.size(); j++)
        {
            TransactionStruct ts2 = HXChain::getInstance()->transactionDB.getTransactionStruct(sortedTypeIds.at(j).transactionId);
            if(ts2.blockNum == 0)   continue;   // 未确认的交易放前面
            if(ts.blockNum >= ts2.blockNum || ts.blockNum == 0)
            {
                sortedTypeIds.insert(j,typeIds.at(i));
                break;
            }

            if(j == sortedTypeIds.size() - 1)
            {
                sortedTypeIds.append(typeIds.at(i));
                break;
            }
        }
    }

    int size = sortedTypeIds.size();
    ui->withdrawRecordTableWidget->setRowCount(0);
    int rowCount = 0;

    for(int i = 0; i < size; i++)
    {
        QString transactionId = typeIds.at(size - i - 1).transactionId;
        TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct(transactionId);
        if(ts.type == -1)
        {
            qDebug() << "can not find transaction in transactionstruct db: " << transactionId;
            continue;
        }

        QJsonObject object = QJsonDocument::fromJson(ts.operationStr.toLatin1()).object();
        QString amountStr = object.take("amount").toString();
        QString amountAssetId = object.take("asset_id").toString();
        QString crossChainAddress = object.take("crosschain_account").toString();
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(amountAssetId);

        if(_assetId != "ALL"  &&  amountAssetId != _assetId)    continue;


        ui->withdrawRecordTableWidget->setRowCount(rowCount + 1);
        ui->withdrawRecordTableWidget->setRowHeight(rowCount,40);

        ui->withdrawRecordTableWidget->setItem(rowCount,0, new QTableWidgetItem(QString::number(ts.blockNum)));
        ui->withdrawRecordTableWidget->setItem(rowCount,4, new QTableWidgetItem(transactionId));



        ui->withdrawRecordTableWidget->setItem(rowCount,1, new QTableWidgetItem(crossChainAddress));

        // 如果是转出
        QTableWidgetItem* item = new QTableWidgetItem( "-" + amountStr + " " + revertERCSymbol(assetInfo.symbol));
        ui->withdrawRecordTableWidget->setItem(rowCount,2, item);
        item->setTextColor(QColor(255,0,0));


        QJsonObject feeObject = object.take("fee").toObject();
        unsigned long long feeAmount = jsonValueToULL(feeObject.take("amount"));
        QString feeAssetId = feeObject.take("asset_id").toString();
        AssetInfo feeAssetInfo = HXChain::getInstance()->assetInfoMap.value(feeAssetId);
        ui->withdrawRecordTableWidget->setItem(rowCount,3, new QTableWidgetItem(getBigNumberString(feeAmount, feeAssetInfo.precision)));

        if(ts.trxState == "withdraw_without_sign_trx_uncreate")
        {
            ui->withdrawRecordTableWidget->setItem(rowCount,5, new QTableWidgetItem(tr("waiting for signature")));
        }
        else if( ts.trxState == "withdraw_without_sign_trx_create" || ts.trxState == "withdraw_combine_trx_create")
        {
            ui->withdrawRecordTableWidget->setItem(rowCount,5, new QTableWidgetItem(tr("being signed")));
        }
        else if( ts.trxState == "withdraw_transaction_confirm")
        {
            ui->withdrawRecordTableWidget->setItem(rowCount,5, new QTableWidgetItem(tr("confirmed")));
        }

        rowCount++;
    }
    ui->withdrawRecordTableWidget->hideColumn(3);
    ui->withdrawRecordTableWidget->hideColumn(5);
    tableWidgetSetItemZebraColor(ui->withdrawRecordTableWidget);

    int page = (ui->withdrawRecordTableWidget->rowCount()%ROWNUMBER==0 && ui->withdrawRecordTableWidget->rowCount() != 0) ?
                ui->withdrawRecordTableWidget->rowCount()/ROWNUMBER : ui->withdrawRecordTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(ui->withdrawRecordTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(pageWidget->GetCurrentPage());
    pageWidget->setVisible(ui->withdrawRecordTableWidget->rowCount()>0);
    blankWidget->setVisible(ui->withdrawRecordTableWidget->rowCount() == 0);

}

void WithdrawRecordWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));
    painter.drawRect(0,0,770,510);
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
    if( column == 1 || column == 4 )
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
