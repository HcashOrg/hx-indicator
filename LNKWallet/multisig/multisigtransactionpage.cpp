#include "multisigtransactionpage.h"
#include "ui_multisigtransactionpage.h"

#include <QClipboard>
#include <QListView>

#include "commondialog.h"
#include "showcontentdialog.h"
#include "control/qrcodedialog.h"

MultiSigTransactionPage::MultiSigTransactionPage(QString _multiSigAddress, QWidget *parent) :
    QWidget(parent),
    inited(false),
    multiSigAddress(_multiSigAddress),
    ui(new Ui::MultiSigTransactionPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->pageLineEdit->setText("1");
    QIntValidator *validator = new QIntValidator(1,9999,this);
    ui->pageLineEdit->setValidator( validator );
    ui->pageLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->multiSigAddressComboBox->setView(new QListView());


    currentPageIndex = 1;
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->copyBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/copy.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                               "QToolButton:hover{background-image:url(:/ui/wallet_ui/copy_hover.png);}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));

    ui->qrcodeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/qrcode.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                 "QToolButton:hover{background-image:url(:/ui/wallet_ui/qrcode_hover.png);}");

    ui->firstPageBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/firstPage.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                    "QToolButton:hover{background-image:url(:/ui/wallet_ui/firstPage_hover.png);}");
    ui->prePageBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/prePage.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                  "QToolButton:hover{background-image:url(:/ui/wallet_ui/prePage_hover.png);}");
    ui->nextPageBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/nextPage.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                   "QToolButton:hover{background-image:url(:/ui/wallet_ui/nextPage_hover.png);}");
    ui->lastPageBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/lastPage.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                   "QToolButton:hover{background-image:url(:/ui/wallet_ui/lastPage_hover.png);}");

    ui->transactionsTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->transactionsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->transactionsTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->transactionsTableWidget->setFrameShape(QFrame::NoFrame);
    ui->transactionsTableWidget->setMouseTracking(true);
    ui->transactionsTableWidget->setShowGrid(false);//隐藏表格线

    ui->transactionsTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->transactionsTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->transactionsTableWidget->horizontalHeader()->setVisible(true);
    ui->transactionsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->transactionsTableWidget->setColumnWidth(0,90);
    ui->transactionsTableWidget->setColumnWidth(1,70);
    ui->transactionsTableWidget->setColumnWidth(2,100);
    ui->transactionsTableWidget->setColumnWidth(3,100);
    ui->transactionsTableWidget->setColumnWidth(4,110);
    ui->transactionsTableWidget->setColumnWidth(5,70);
    ui->transactionsTableWidget->setColumnWidth(6,90);

    QStringList keys = UBChain::getInstance()->multiSigInfoMap.keys();
    ui->multiSigAddressComboBox->addItems( keys);
    ui->multiSigAddressComboBox->setCurrentText(multiSigAddress);

    inited = true;

    refresh();
}

MultiSigTransactionPage::~MultiSigTransactionPage()
{
    delete ui;
}

void MultiSigTransactionPage::updateTransactionsList()
{
    QStringList keys = UBChain::getInstance()->multiSigInfoMap.keys();
    foreach (QString key, keys)
    {
        UBChain::getInstance()->postRPC( "id_wallet_multisig_account_history+" + key, toJsonFormat( "wallet_multisig_account_history",
                                                       QJsonArray() << key
                                                   ));
    }
}

void MultiSigTransactionPage::updateBalance()
{
    MultiSigInfo info = UBChain::getInstance()->multiSigInfoMap.value(ui->multiSigAddressComboBox->currentText());
    unsigned long long amount = info.balanceMap.value(0);

    ui->balanceLabel->setText( "<body><font style=\"font-size:18px\" color=#18faef>" + getBigNumberString(amount,ASSET_PRECISION) + "</font><font style=\"font-size:12px\" color=#18faef> " + ASSET_NAME + "</font></body>" );
    ui->balanceLabel->adjustSize();

}

void MultiSigTransactionPage::refresh()
{
    updateTransactionsList();
    updateBalance();
}

void MultiSigTransactionPage::jsonDataUpdated(QString id)
{
    if( id.startsWith("id_wallet_multisig_account_history+"))
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        QString multiSigAddress = id.mid(QString("id_wallet_multisig_account_history+").size());
        UBChain::getInstance()->parseMultiSigTransactions(result, multiSigAddress);

        showTransactions();
        return;
    }
}

void MultiSigTransactionPage::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->multiSigAddressComboBox->currentText());

    CommonDialog commonDialog(CommonDialog::OkOnly);
    commonDialog.setText(tr("Copy to clipboard"));
    commonDialog.pop();
}

void MultiSigTransactionPage::on_qrcodeBtn_clicked()
{
    QRCodeDialog qrcodeDialog(ui->multiSigAddressComboBox->currentText());
    qrcodeDialog.move(ui->qrcodeBtn->mapToGlobal( QPoint(20,0)));
    qrcodeDialog.exec();
}

void MultiSigTransactionPage::on_prePageBtn_clicked()
{
    ui->transactionsTableWidget->scrollToTop();
    if( currentPageIndex == 1) return;
    currentPageIndex--;
    showTransactions();
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

}

void MultiSigTransactionPage::on_nextPageBtn_clicked()
{
    int totalPageNum = ui->pageLabel->text().remove("/").toInt();
    if(  currentPageIndex >= totalPageNum )  return;

    currentPageIndex++;
    showTransactions();
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->transactionsTableWidget->scrollToTop();
}

void MultiSigTransactionPage::on_firstPageBtn_clicked()
{
    if( currentPageIndex == 1) return;
    currentPageIndex = 1;
    showTransactions();
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->transactionsTableWidget->scrollToTop();
}

void MultiSigTransactionPage::on_lastPageBtn_clicked()
{
    int totalPageNum = ui->pageLabel->text().remove("/").remove(" ").toInt();
    if(  currentPageIndex >= totalPageNum )  return;

    currentPageIndex = totalPageNum;
    showTransactions();
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->transactionsTableWidget->scrollToTop();
}


void MultiSigTransactionPage::on_pageLineEdit_editingFinished()
{
    currentPageIndex = ui->pageLineEdit->text().toInt();
    showTransactions();
}

void MultiSigTransactionPage::on_pageLineEdit_textEdited(const QString &arg1)
{
    if( arg1.at(0) == '0')
    {
        ui->pageLineEdit->setText( arg1.mid(1));
        return;
    }
    int totalPageNum = ui->pageLabel->text().remove("/").toInt();

    if( arg1.toInt() > totalPageNum)
    {
        ui->pageLineEdit->setText( QString::number( totalPageNum));
        return;
    }
}

void MultiSigTransactionPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
    painter.drawRect(0,0,680,482);
}

void MultiSigTransactionPage::showTransactions()
{
//    ui->transactionsTableWidget->setColumnHidden(6,false);
//    ui->transactionsTableWidget->setColumnWidth(0,90);
//    ui->transactionsTableWidget->setColumnWidth(1,90);
//    ui->transactionsTableWidget->setColumnWidth(2,300);
//    ui->transactionsTableWidget->setColumnWidth(3,100);
//    ui->transactionsTableWidget->setColumnWidth(4,100);
//    ui->transactionsTableWidget->setColumnWidth(5,90);
//    ui->transactionsTableWidget->setColumnWidth(6,120);

    TransactionsInfoVector vector = UBChain::getInstance()->multiSigTransactionsMap.value(ui->multiSigAddressComboBox->currentText());

    ui->transactionsTableWidget->setRowCount(0);
    if( vector.size() < 1)
    {
        ui->prePageBtn->hide();
        ui->numberLabel->hide();
        ui->pageLineEdit->hide();
        ui->pageLabel->hide();
        ui->nextPageBtn->hide();
        ui->initLabel->show();
        return;
    }
    ui->prePageBtn->show();
    ui->numberLabel->show();
    ui->pageLineEdit->show();
    ui->pageLabel->show();
    ui->nextPageBtn->show();
    ui->initLabel->hide();

    int size = vector.size();
    ui->numberLabel->setText( tr("total ") + QString::number( size) + tr(" ,"));
    ui->pageLabel->setText( "/" + QString::number( (size - 1)/10 + 1 ) );

    int rowCount = size - (currentPageIndex - 1) * 10;
    if( rowCount > 10 )  rowCount = 10;  // 一页最多显示10行
    ui->transactionsTableWidget->setRowCount(rowCount);

    for(int i = rowCount - 1; i > -1; i--)
    {
        ui->transactionsTableWidget->setRowHeight(i,57);
        TransactionInfo transactionInfo = vector.at(  size - ( i + 1) - (currentPageIndex - 1) * 10 );

        // 时间戳
        QString date = transactionInfo.timeStamp;
        date.replace(QString("T"),QString(" "));
        QDateTime time = QDateTime::fromString(date, "yyyy-MM-dd hh:mm:ss");
        time = time.addSecs(28800);       // 时间加8小时
        QString currentDateTime = time.toString("yyyy-MM-dd\r\nhh:mm:ss");
        ui->transactionsTableWidget->setItem(i,0,new QTableWidgetItem(currentDateTime));
        ui->transactionsTableWidget->item(i,0)->setTextColor(QColor(192,196,212));

        // 区块高度
        ui->transactionsTableWidget->setItem(i,1,new QTableWidgetItem( QString::number(transactionInfo.blockNum) ));
        ui->transactionsTableWidget->item(i,1)->setTextColor(QColor(192,196,212));

        // 交易ID
        ui->transactionsTableWidget->setItem(i,2,new QTableWidgetItem( transactionInfo.trxId));
        ui->transactionsTableWidget->item(i,2)->setTextColor(QColor(192,196,212));


        TransactionDetail detail = getDetail(transactionInfo);
        // 对方账户
        ui->transactionsTableWidget->setItem(i,3,new QTableWidgetItem( UBChain::getInstance()->addressToName(detail.opposite)));
        ui->transactionsTableWidget->item(i,3)->setTextColor(QColor(192,196,212));

//        // 金额
//        AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(detail.assetAmount.assetId);
//        unsigned long long amount = 0;
//        if( !detail.includeFee || transactionInfo.isConfirmed == false)
//        {
//            amount = detail.assetAmount.amount;
//        }
//        else
//        {
//            amount = detail.assetAmount.amount - transactionInfo.fee;
//        }

//        if( detail.type == 1)
//        {
//            ui->transactionsTableWidget->setItem(i,4,new QTableWidgetItem( "-" + getBigNumberString(amount,assetInfo.precision) + " "
//                                                                                  + assetInfo.symbol));
//            ui->transactionsTableWidget->item(i,4)->setTextColor(QColor(239,104,60));
//        }
//        else if( detail.type == 2)
//        {
//            ui->transactionsTableWidget->setItem(i,4,new QTableWidgetItem( "+" + getBigNumberString(amount,assetInfo.precision) + " "
//                                                                                  + assetInfo.symbol));
//            ui->transactionsTableWidget->item(i,4)->setTextColor(QColor(113,203,90));
//        }
//        else
//        {
//            ui->transactionsTableWidget->setItem(i,4,new QTableWidgetItem( getBigNumberString(amount,assetInfo.precision) + " "
//                                                                                  + assetInfo.symbol));
//        }

//        if( transactionInfo.isConfirmed == false)
//        {
//            ui->transactionsTableWidget->item(i,4)->setTextColor(QColor(200,200,200));
//        }

        // 手续费
        ui->transactionsTableWidget->setItem(i,5,new QTableWidgetItem( getBigNumberString(transactionInfo.fee,ASSET_PRECISION)));
        ui->transactionsTableWidget->item(i,5)->setTextColor(QColor(255,34,76));

        // 备注
        ui->transactionsTableWidget->setItem(i,6,new QTableWidgetItem( detail.memo));
        ui->transactionsTableWidget->item(i,6)->setTextColor(QColor(192,196,212));

        for(int j = 0; j < 7; j++)
        {
            ui->transactionsTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);

            if(i % 2)
            {
                ui->transactionsTableWidget->item(i,j)->setBackgroundColor(QColor(43,49,69));
            }
            else
            {
                ui->transactionsTableWidget->item(i,j)->setBackgroundColor(QColor(40,46,66));
            }
        }

    }
}

MultiSigTransactionPage::TransactionDetail MultiSigTransactionPage::getDetail(TransactionInfo info)
{
    TransactionDetail result;
    if( info.trxId.isEmpty())   return result;
    if( !info.isMarket)
    {
        if( info.entries.size() == 2)
        {
            Entry entry = info.entries.at(1);


            if( entry.fromAccount == ui->multiSigAddressComboBox->currentText())
            {
                result.opposite = entry.toAccount;
                result.type     = 1;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
            }
            else
            {
                result.opposite = entry.fromAccount;
                result.type     = 2;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
            }
        }
        else if( info.entries.size() == 1)      //   本地没有多签owner账户的时候只有一个entry
        {
            Entry entry = info.entries.at(0);

            if( entry.fromAccount == ui->multiSigAddressComboBox->currentText())
            {
                result.opposite = entry.toAccount;
                result.type     = 1;
                result.assetAmount.amount = entry.amount.amount - info.fee;
                result.assetAmount.assetId = entry.amount.assetId;
                result.memo     = entry.memo;
            }
            else
            {
                result.opposite = entry.fromAccount;
                result.type     = 2;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
            }
        }





    }


    return result;
}

void MultiSigTransactionPage::on_multiSigAddressComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;

    currentPageIndex = 1;
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    updateTransactionsList();
    updateBalance();
    showTransactions();
}

void MultiSigTransactionPage::on_backBtn_clicked()
{
    emit back();
}

void MultiSigTransactionPage::on_transactionsTableWidget_cellPressed(int row, int column)
{
    if( column == 2 || column == 3 || column == 6)
    {
        ShowContentDialog showContentDialog( ui->transactionsTableWidget->item(row, column)->text(),this);

        int x = ui->transactionsTableWidget->columnViewportPosition(column) + ui->transactionsTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->transactionsTableWidget->rowViewportPosition(row) - 10 + ui->transactionsTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->transactionsTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

