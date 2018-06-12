#include "ContactInfoHistoryWidget.h"
#include "ui_ContactInfoHistoryWidget.h"

#include "wallet.h"
#include "poundage/PageScrollWidget.h"
#include "showcontentdialog.h"
#include <mutex>

#include "control/BlankDefaultWidget.h"


static const int ROWNUMBER = 5;
class ContactInfoHistoryWidget::ContactInfoHistoryWidgetPrivate
{
public:
    ContactInfoHistoryWidgetPrivate()
        :pageWidget(new PageScrollWidget())
        ,blankWidget(nullptr)
    {

    }
public:
    PageScrollWidget *pageWidget;
    std::mutex mutexLock;

    BlankDefaultWidget *blankWidget;
};

ContactInfoHistoryWidget::ContactInfoHistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactInfoHistoryWidget),
    _p(new ContactInfoHistoryWidgetPrivate())
{
    ui->setupUi(this);

    _p->blankWidget = new BlankDefaultWidget(ui->transferRecordTableWidget);
    _p->blankWidget->setTextTip(tr("当前账户没有历史交易纪录!"));
    InitWidget();
}

ContactInfoHistoryWidget::~ContactInfoHistoryWidget()
{
    delete _p;
    delete ui;
}

void ContactInfoHistoryWidget::showTransferRecord(QString _accountAddress, QString _assetId)
{
    accountAddress = _accountAddress;
    QVector<TransactionStruct> tsVector = UBChain::getInstance()->transactionDB.lookupTransactionStruct(_accountAddress,TRANSACTION_TYPE_NORMAL);


    // 根据区块高度排序
//    TransactionTypeIds sortedTypeIds;
    QVector<TransactionStruct> sortedTsVector;
    for(int i = 0; i < tsVector.size(); i++)
    {
        if(sortedTsVector.size() == 0)
        {
            sortedTsVector.append(tsVector.at(i));
            continue;
        }

        TransactionStruct ts = tsVector.at(i);
        for(int j = 0; j < sortedTsVector.size(); j++)
        {
            TransactionStruct ts2 = sortedTsVector.at(j);
            if(ts2.blockNum == 0)   continue;   // 未确认的交易放前面
            if(ts.blockNum >= ts2.blockNum || ts.blockNum == 0)
            {
                sortedTsVector.insert(j,ts);
                break;
            }

            if(j == sortedTsVector.size() - 1)
            {
                sortedTsVector.append(ts);
                break;
            }
        }
    }

    int size = sortedTsVector.size();
    ui->transferRecordTableWidget->setRowCount(0);
    int rowCount = 0;

    for(int i = 0; i < size; i++)
    {
        TransactionStruct ts = sortedTsVector.at(i);
        QString transactionId = ts.transactionId;
        if(ts.type == -1)
        {
            qDebug() << "can not find transaction in transactionstruct db: " << transactionId;
            continue;
        }

        QJsonObject object = QJsonDocument::fromJson(ts.operationStr.toLatin1()).object();
        QJsonObject amountObject = object.take("amount").toObject();
        unsigned long long amount = jsonValueToULL(amountObject.take("amount"));
        QString amountAssetId = amountObject.take("asset_id").toString();
        AssetInfo amountAssetInfo = UBChain::getInstance()->assetInfoMap.value(amountAssetId);

        if(_assetId != "ALL"  &&  amountAssetId != _assetId)    continue;


        ui->transferRecordTableWidget->setRowCount(rowCount + 1);
        ui->transferRecordTableWidget->setRowHeight(rowCount,40);

        ui->transferRecordTableWidget->setItem(rowCount,0, new QTableWidgetItem(QString::number(ts.blockNum)));
        ui->transferRecordTableWidget->setItem(rowCount,4, new QTableWidgetItem(transactionId));

        QString fromAddress = object.take("from_addr").toString();
        QString toAddress   = object.take("to_addr").toString();
        if(fromAddress == _accountAddress)
        {
            ui->transferRecordTableWidget->setItem(rowCount,1, new QTableWidgetItem(toAddress));

            if(toAddress == _accountAddress)    // 如果是自己转自己
            {
                QTableWidgetItem* item = new QTableWidgetItem(getBigNumberString(amount, amountAssetInfo.precision) + " " + amountAssetInfo.symbol);
                ui->transferRecordTableWidget->setItem(rowCount,2, item);
                item->setTextColor(QColor(255,255,0));
            }
            else
            {
                // 如果是转出
                QTableWidgetItem* item = new QTableWidgetItem( "- " + getBigNumberString(amount, amountAssetInfo.precision) + " " + amountAssetInfo.symbol);
                ui->transferRecordTableWidget->setItem(rowCount,2, item);
                item->setTextColor(QColor(255,0,0));
            }
        }
        else
        {
            ui->transferRecordTableWidget->setItem(rowCount,1, new QTableWidgetItem(fromAddress));

            // 如果是转入
            QTableWidgetItem* item = new QTableWidgetItem( "+ " + getBigNumberString(amount, amountAssetInfo.precision) + " " + amountAssetInfo.symbol);
            ui->transferRecordTableWidget->setItem(rowCount,2, item);
            item->setTextColor(QColor(0,255,0));
        }


        QJsonObject feeObject = object.take("fee").toObject();
        unsigned long long feeAmount = jsonValueToULL(feeObject.take("amount"));
        QString feeAssetId = feeObject.take("asset_id").toString();
        AssetInfo feeAssetInfo = UBChain::getInstance()->assetInfoMap.value(feeAssetId);
        ui->transferRecordTableWidget->setItem(rowCount,3, new QTableWidgetItem(getBigNumberString(feeAmount, feeAssetInfo.precision)));

        ui->transferRecordTableWidget->setItem(rowCount,5, new QTableWidgetItem(tr("confirmed")));

        rowCount++;
    }
    //设置表格内容居中
    for(int i = 0;i < ui->transferRecordTableWidget->rowCount();++i)
    {
        for(int j = 0;j < ui->transferRecordTableWidget->columnCount();++j)
        {
            ui->transferRecordTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }

    }
    int page = (ui->transferRecordTableWidget->rowCount()%ROWNUMBER==0 && ui->transferRecordTableWidget->rowCount() != 0) ?
                ui->transferRecordTableWidget->rowCount()/ROWNUMBER : ui->transferRecordTableWidget->rowCount()/ROWNUMBER+1;
    _p->pageWidget->SetTotalPage(page);
    _p->pageWidget->setShowTip(ui->transferRecordTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(0);

    _p->pageWidget->setVisible(0 != ui->transferRecordTableWidget->rowCount());
    _p->blankWidget->setVisible(ui->transferRecordTableWidget->rowCount() == 0);
}

void ContactInfoHistoryWidget::InitWidget()
{
    InitStyle();

    ui->assetComboBox->addItem(tr("ALL"), "ALL");
    QStringList assetIds = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
    }

    ui->stackedWidget->addWidget(_p->pageWidget);
    connect(_p->pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&ContactInfoHistoryWidget::pageChangeSlot);

    connect(ui->assetComboBox,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),this,&ContactInfoHistoryWidget::assetComboBox_currentIndexChanged);

    UBChain::getInstance()->mainFrame->installBlurEffect(ui->transferRecordTableWidget);
}

void ContactInfoHistoryWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    QFont font("\"Microsoft YaHei UI Light\"",14,63);
    font.setPixelSize(20);
    ui->label->setFont(font);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::black);
    ui->label->setPalette(pa);


    ui->transferRecordTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->transferRecordTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->transferRecordTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->transferRecordTableWidget->setFrameShape(QFrame::NoFrame);
    ui->transferRecordTableWidget->setMouseTracking(true);
    ui->transferRecordTableWidget->setShowGrid(false);//隐藏表格线

    ui->transferRecordTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->transferRecordTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->transferRecordTableWidget->horizontalHeader()->setVisible(true);
    ui->transferRecordTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


//    ui->transferRecordTableWidget->setColumnWidth(0,90);
//    ui->transferRecordTableWidget->setColumnWidth(1,70);
//    ui->transferRecordTableWidget->setColumnWidth(2,100);
//    ui->transferRecordTableWidget->setColumnWidth(3,100);
//    ui->transferRecordTableWidget->setColumnWidth(4,110);
//    ui->transferRecordTableWidget->setColumnWidth(5,70);

    ui->transferRecordTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

}

void ContactInfoHistoryWidget::assetComboBox_currentIndexChanged(const QString &arg1)
{
    std::lock_guard<std::mutex> guard(_p->mutexLock);
    showTransferRecord(accountAddress, ui->assetComboBox->currentData().toString());
}

void ContactInfoHistoryWidget::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->transferRecordTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->transferRecordTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->transferRecordTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->transferRecordTableWidget->setRowHidden(i,true);
        }
    }

}

void ContactInfoHistoryWidget::on_transferRecordTableWidget_cellPressed(int row, int column)
{
    if( column == 1 || column == 4)
    {
        ShowContentDialog showContentDialog( ui->transferRecordTableWidget->item(row, column)->text(),this);

        int x = ui->transferRecordTableWidget->columnViewportPosition(column) + ui->transferRecordTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->transferRecordTableWidget->rowViewportPosition(row) - 10 + ui->transferRecordTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->transferRecordTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}
