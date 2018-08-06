#include "DepositRecrdWideget.h"
#include "ui_DepositRecrdWideget.h"

#include "wallet.h"
#include "poundage/PageScrollWidget.h"
#include "showcontentdialog.h"
#include "control/BlankDefaultWidget.h"

static const int ROWNUMBER = 7;

DepositRecrdWideget::DepositRecrdWideget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DepositRecrdWideget)
{
    ui->setupUi(this);

    ui->depositRecordTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->depositRecordTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->depositRecordTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->depositRecordTableWidget->setFrameShape(QFrame::NoFrame);
    ui->depositRecordTableWidget->setMouseTracking(true);
    ui->depositRecordTableWidget->setShowGrid(false);//隐藏表格线

    ui->depositRecordTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->depositRecordTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->depositRecordTableWidget->horizontalHeader()->setVisible(true);
    ui->depositRecordTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->depositRecordTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

//    ui->depositRecordTableWidget->setColumnWidth(0,90);
//    ui->depositRecordTableWidget->setColumnWidth(1,70);
//    ui->depositRecordTableWidget->setColumnWidth(2,100);
//    ui->depositRecordTableWidget->setColumnWidth(3,100);
//    ui->depositRecordTableWidget->setColumnWidth(4,110);
//    ui->depositRecordTableWidget->setColumnWidth(5,70);

    pageWidget = new PageScrollWidget();
    blankWidget = new BlankDefaultWidget(ui->depositRecordTableWidget);
    blankWidget->setTextTip(tr("There's no deposit record!"));

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->depositRecordTableWidget);
    init();
}

DepositRecrdWideget::~DepositRecrdWideget()
{
    delete ui;
}

void DepositRecrdWideget::init()
{

    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&DepositRecrdWideget::pageChangeSlot);

}

void DepositRecrdWideget::showDepositRecord(QString _tunnelAddress)
{
    tunnelAddress = _tunnelAddress;
    TransactionTypeIds typeIds = HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(_tunnelAddress,TRANSACTION_TYPE_DEPOSIT);

    int size = typeIds.size();
    ui->depositRecordTableWidget->setRowCount(0);
    ui->depositRecordTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->depositRecordTableWidget->setRowHeight(i,40);

        QString transactionId = typeIds.at(size - i - 1).transactionId;
        TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct(transactionId);
        if(ts.type == -1)
        {
            qDebug() << "can not find transaction in transactionstruct db: " << transactionId;
            continue;
        }

        QJsonObject object = QJsonDocument::fromJson(ts.operationStr.toLatin1()).object();
        QJsonObject crossChainTxObject  = object.take("cross_chain_trx").toObject();
        QString     crossChainTxId      = crossChainTxObject.take("trx_id").toString();
        QString     amount              = crossChainTxObject.take("amount").toString();
        QString     assetSymbol         = crossChainTxObject.take("asset_symbol").toString();


        ui->depositRecordTableWidget->setItem(i,0, new QTableWidgetItem(QString::number(ts.blockNum)));

        QTableWidgetItem* item = new QTableWidgetItem(amount + " " + assetSymbol);
        ui->depositRecordTableWidget->setItem(i,1, item);
        item->setTextColor(QColor(0,170,0));

        ui->depositRecordTableWidget->setItem(i,2, new QTableWidgetItem(transactionId));

        ui->depositRecordTableWidget->setItem(i,3, new QTableWidgetItem(crossChainTxId));

        ui->depositRecordTableWidget->setItem(i,4, new QTableWidgetItem(tr("confirmed")));

    }
    tableWidgetSetItemZebraColor(ui->depositRecordTableWidget);

    int page = (ui->depositRecordTableWidget->rowCount()%ROWNUMBER==0 && ui->depositRecordTableWidget->rowCount() != 0) ?
                ui->depositRecordTableWidget->rowCount()/ROWNUMBER : ui->depositRecordTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(ui->depositRecordTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(0);
    pageWidget->setVisible(0 != size);
    blankWidget->setVisible(0 == size);

}

void DepositRecrdWideget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));
    painter.drawRect(0,0,770,510);
    QWidget::paintEvent(e);
}

void DepositRecrdWideget::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->depositRecordTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->depositRecordTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->depositRecordTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->depositRecordTableWidget->setRowHidden(i,true);
        }
    }

}

void DepositRecrdWideget::on_depositRecordTableWidget_cellPressed(int row, int column)
{
    if( column == 2 || column == 3)
    {
        ShowContentDialog showContentDialog( ui->depositRecordTableWidget->item(row, column)->text(),this);

        int x = ui->depositRecordTableWidget->columnViewportPosition(column) + ui->depositRecordTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->depositRecordTableWidget->rowViewportPosition(row) - 10 + ui->depositRecordTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->depositRecordTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}
