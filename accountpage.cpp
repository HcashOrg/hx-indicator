#include <QDebug>
#include <QPainter>
#include <QHelpEvent>
#include <QDateTime>
#include <QTextCodec>
#include <QScrollBar>
#include <QListView>

#include "accountpage.h"
#include "ui_accountpage.h"
#include "debug_log.h"
#include <QClipboard>
#include "commondialog.h"
#include "showcontentdialog.h"
#include "control/remarkcellwidget.h"
#include "control/qrcodedialog.h"
#include "dialog/checkpwddialog.h"

AccountPage::AccountPage(QString name, QWidget *parent) :
    QWidget(parent),
    accountName(name),
    transactionType(0),
    address(""),
    inited(false),
    assetUpdating(false),
    ui(new Ui::AccountPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    mutexForAddressMap.lock();
    if( accountName.isEmpty())  // 如果是点击账单跳转
    {
        if( UBChain::getInstance()->addressMap.size() > 0)
        {
            accountName = UBChain::getInstance()->addressMap.keys().at(0);
        }
        else  // 如果还没有账户
        {
            mutexForAddressMap.unlock();
            emit back();
            return;
        }
    }

    // 账户下拉框按字母顺序排序
    QStringList keys = UBChain::getInstance()->addressMap.keys();
    ui->accountComboBox->addItems( keys);
    if( accountName.isEmpty() )
    {
        ui->accountComboBox->setCurrentIndex(0);
    }
    else
    {
        ui->accountComboBox->setCurrentText( accountName);
    }
    mutexForAddressMap.unlock();


     QIntValidator *validator = new QIntValidator(1,9999,this);
    ui->pageLineEdit->setValidator( validator );
    ui->pageLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

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

    ui->accountComboBox->setView(new QListView());


    ui->accountTransactionsTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->accountTransactionsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountTransactionsTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->accountTransactionsTableWidget->setFrameShape(QFrame::NoFrame);
    ui->accountTransactionsTableWidget->setMouseTracking(true);
    ui->accountTransactionsTableWidget->setShowGrid(false);//隐藏表格线

    ui->accountTransactionsTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->accountTransactionsTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->accountTransactionsTableWidget->horizontalHeader()->setVisible(true);
    ui->accountTransactionsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    ui->accountTransactionsTableWidget->setColumnWidth(0,90);
    ui->accountTransactionsTableWidget->setColumnWidth(1,70);
    ui->accountTransactionsTableWidget->setColumnWidth(2,100);
    ui->accountTransactionsTableWidget->setColumnWidth(3,100);
    ui->accountTransactionsTableWidget->setColumnWidth(4,110);
    ui->accountTransactionsTableWidget->setColumnWidth(5,70);
    ui->accountTransactionsTableWidget->setColumnWidth(6,90);


    ui->initLabel->hide();


    init();
    updateTransactionsList();

    inited = true;
}


AccountPage::~AccountPage()
{
    delete ui;

}

void AccountPage::init()
{
//    inited = false;
    QString showName = UBChain::getInstance()->addressMapValue(accountName).ownerAddress;
    address = showName;
    showName = showName.left(8) + "..." + showName.right(3);
    ui->addressLabel->setText( showName);
    ui->addressLabel->adjustSize();
    ui->addressLabel->setGeometry(ui->addressLabel->x(),ui->addressLabel->y(),ui->addressLabel->width(),24);

//    ui->accountComboBox->setCurrentText(accountName);

//    inited = true;

//    jsonDataUpdated("id_balance");


}

void AccountPage::updateTransactionsList()
{

    UBChain::getInstance()->postRPC(  "id_history_" + accountName + "_" + ASSET_NAME, toJsonFormat("history",
                                                QStringList() << accountName << ASSET_NAME  << "9999999" ));

}


void AccountPage::updateContractFee()
{
    UBChain::getInstance()->postRPC( "id_wallet_get_account_contract_fee_" + accountName, toJsonFormat( "wallet_get_account_contract_fee", QStringList() << accountName));
}


void AccountPage::refresh()
{


    init();
    updateTransactionsList();
    showTransactions();

    updateContractFee();


}

bool isExistInWallet(QString);

void AccountPage::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(address);

    CommonDialog commonDialog(CommonDialog::OkOnly);
    commonDialog.setText(tr("Copy to clipboard"));
    commonDialog.pop();
}


void AccountPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
    painter.drawRect(0,0,680,482);
}

void AccountPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if( inited)  // 防止accountComboBox当前值改变的时候触发
    {
        accountName = arg1;
        UBChain::getInstance()->mainFrame->setCurrentAccount(accountName);
        currentPageIndex = 1;
        ui->pageLineEdit->setText( QString::number(currentPageIndex));
        ui->contractFeeLabel->setText("0 UB");
        init();
        updateTransactionsList();

        showTransactions();
        updateContractFee();
    }
}


void AccountPage::jsonDataUpdated(QString id)
{
    if( id == "id_history_" + accountName + "_" + ASSET_NAME)
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        UBChain::getInstance()->parseTransactions(result,accountName + "_" + ASSET_NAME);

        showTransactions();
        return;
    }

    if( id == "id_wallet_get_account_contract_fee_" + accountName)
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            int pos = result.indexOf("\"fee_collector\":{\"amount\":") + 26;
            QString contractFeeAmount = result.mid(pos, result.indexOf(",", pos) - pos);
            contractFeeAmount.remove("\"");
            ui->contractFeeLabel->setText(getBigNumberString(contractFeeAmount.toULongLong(),ASSET_PRECISION) + " UB");
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);
        }
        return;
    }

    if( id.startsWith("id_wallet_get_contract_fee_"))
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        QString name = id.mid(QString("id_wallet_get_contract_fee_").size());

        if( result.startsWith("\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Receive the fee successfully!"));
            commonDialog.pop();
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Fail to receive the fee: ") + errorMessage);
            commonDialog.pop();
        }
        return;
    }
}



void AccountPage::showTransactions()
{
    if( UBChain::getInstance()->currentTokenAddress.isEmpty())
    {
        showNormalTransactions();
    }
    else
    {
        showContractTransactions();
    }
}

void AccountPage::showNormalTransactions()
{
    ui->accountTransactionsTableWidget->setColumnHidden(6,false);
    ui->accountTransactionsTableWidget->setColumnWidth(0,90);
    ui->accountTransactionsTableWidget->setColumnWidth(1,70);
    ui->accountTransactionsTableWidget->setColumnWidth(2,100);
    ui->accountTransactionsTableWidget->setColumnWidth(3,100);
    ui->accountTransactionsTableWidget->setColumnWidth(4,110);
    ui->accountTransactionsTableWidget->setColumnWidth(5,70);
    ui->accountTransactionsTableWidget->setColumnWidth(6,90);

    TransactionsInfoVector vector = UBChain::getInstance()->transactionsMap.value(accountName + "_" + ASSET_NAME);

    ui->accountTransactionsTableWidget->setRowCount(0);
    if( vector.size() < 1)
    {
        ui->firstPageBtn->hide();
        ui->prePageBtn->hide();
        ui->numberLabel->hide();
        ui->lastPageBtn->hide();
        ui->pageLineEdit->hide();
        ui->pageLabel->hide();
        ui->nextPageBtn->hide();
        ui->initLabel->show();
        return;
    }
    ui->firstPageBtn->show();
    ui->prePageBtn->show();
    ui->numberLabel->show();
    ui->lastPageBtn->show();
    ui->pageLineEdit->show();
    ui->pageLabel->show();
    ui->nextPageBtn->show();
    ui->initLabel->hide();

    int size = vector.size();
    ui->numberLabel->setText( tr("Total ") + QString::number( size));
    ui->pageLabel->setText( "/ " + QString::number( (size - 1)/10 + 1 ));

    int rowCount = size - (currentPageIndex - 1) * 10;
    if( rowCount > 10 )  rowCount = 10;  // 一页最多显示10行
    ui->accountTransactionsTableWidget->setRowCount(rowCount);

    for(int i = rowCount - 1; i > -1; i--)
    {
        ui->accountTransactionsTableWidget->setRowHeight(i,57);
        TransactionInfo transactionInfo = vector.at(  size - ( i + 1) - (currentPageIndex - 1) * 10 );

        // 时间戳
        QString date = transactionInfo.timeStamp;
        date.replace(QString("T"),QString(" "));
        QDateTime time = QDateTime::fromString(date, "yyyy-MM-dd hh:mm:ss");
        time = time.addSecs(28800);       // 时间加8小时
        QString currentDateTime = time.toString("yyyy-MM-dd\r\nhh:mm:ss");
        ui->accountTransactionsTableWidget->setItem(i,0,new QTableWidgetItem(currentDateTime));
        ui->accountTransactionsTableWidget->item(i,0)->setTextColor(QColor(192,196,212));


        // 区块高度
        ui->accountTransactionsTableWidget->setItem(i,1,new QTableWidgetItem( QString::number(transactionInfo.blockNum) ));
        ui->accountTransactionsTableWidget->item(i,1)->setTextColor(QColor(192,196,212));

        // 交易ID
        ui->accountTransactionsTableWidget->setItem(i,2,new QTableWidgetItem( transactionInfo.trxId));
        ui->accountTransactionsTableWidget->item(i,2)->setTextColor(QColor(192,196,212));

        TransactionDetail detail = getDetail(transactionInfo);
        // 对方账户
        ui->accountTransactionsTableWidget->setItem(i,3,new QTableWidgetItem( UBChain::getInstance()->addressToName(detail.opposite)));
        ui->accountTransactionsTableWidget->item(i,3)->setTextColor(QColor(192,196,212));

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

//        if( detail.type == 1 || detail.type == 7 || detail.type == 9)
//        {
//            if(amount > 0)
//            {
//                ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( "-" + getBigNumberString(amount,assetInfo.precision) + " "
//                                                                                      + assetInfo.symbol));
//                ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(255,34,76));
//            }
//            else
//            {
//                ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( getBigNumberString(amount,assetInfo.precision) + " "
//                                                                                      + assetInfo.symbol));
//                ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(192,196,212));
//            }
//        }
//        else if( detail.type == 2 || detail.type == 8 || detail.type == 3 || detail.type == 4)
//        {
//            ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( "+" + getBigNumberString(amount,assetInfo.precision) + " "
//                                                                                  + assetInfo.symbol));
//            ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(113,203,90));
//        }
//        else
//        {
//            ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( getBigNumberString(amount,assetInfo.precision) + " "
//                                                                                  + assetInfo.symbol));
//            ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(192,196,212));
//        }

//        if( transactionInfo.isConfirmed == false)
//        {
//            ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(200,200,200));
//        }

        // 手续费
        if( detail.type == 9)
        {
            ui->accountTransactionsTableWidget->setItem(i,5,new QTableWidgetItem( getBigNumberString(transactionInfo.fee - 100000,ASSET_PRECISION)));
            ui->accountTransactionsTableWidget->item(i,5)->setTextColor(QColor(239,104,60));
        }
        else
        {
            ui->accountTransactionsTableWidget->setItem(i,5,new QTableWidgetItem( getBigNumberString(transactionInfo.fee,ASSET_PRECISION)));
            ui->accountTransactionsTableWidget->item(i,5)->setTextColor(QColor(239,104,60));
        }

        // 备注
        ui->accountTransactionsTableWidget->setItem(i,6,new QTableWidgetItem( detail.memo));
        ui->accountTransactionsTableWidget->item(i,6)->setTextColor(QColor(192,196,212));

        for(int j = 0; j < 7; j++)
        {
            ui->accountTransactionsTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);

            if(i % 2)
            {
                ui->accountTransactionsTableWidget->item(i,j)->setBackgroundColor(QColor(43,49,69));
            }
            else
            {
                ui->accountTransactionsTableWidget->item(i,j)->setBackgroundColor(QColor(40,46,66));
            }
        }

    }


}

AccountPage::TransactionDetail AccountPage::getDetail(TransactionInfo info)
{
    TransactionDetail result;    
    if( info.trxId.isEmpty())   return result;
    if( !info.isMarket)
    {
        Entry entry;
        if( info.entries.size() >= 3)
        {
            for( int i = 2; i < info.entries.size(); i++)
            {
                entry = info.entries.at(i);
                if( entry.toAccount == accountName) break;
            }

            result.includeFee = false;
            result.opposite = tr("contract refund");
            result.type     = 8;
            result.assetAmount = entry.amount;
            result.memo     = entry.memo;
            return result;
        }
        else if( info.entries.size() == 2)
        {
            if( info.entries.at(0).memo == "Contract get fee")
            {
                entry = info.entries.at(0);
                result.includeFee = true;
                result.opposite = tr("receive contract fee");
                result.type     = 3;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
                return result;
            }
            else if( info.entries.at(0).memo == "withdraw pay")
            {
                entry = info.entries.at(0);
                result.includeFee = true;
                result.opposite = tr("delegate receive salary");
                result.type     = 4;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
                return result;
            }
            else if( info.entries.at(0).memo == "withdraw exec cost" && info.entries.at(1).memo == "withdraw contract")
            {
                entry = info.entries.at(1);
                result.includeFee = false;
            }
            else
            {
                entry = info.entries.at(0);
                result.includeFee = true;
            }
        }
        else
        {
            entry = info.entries.at(0);
            result.includeFee = true;
        }

        if(entry.toAccount.isEmpty())
        {
            if( entry.memo == "withdraw contract")          // 如果是upgrade 合约
            {
                result.opposite = tr("call contract");
                result.type     = 9;
                result.assetAmount.amount = 0;
                result.assetAmount.assetId = entry.amount.assetId;
                result.memo     = entry.memo;
            }
            else
            {
                result.opposite = tr("call contract");
                result.type     = 7;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
            }

        }
        else if( entry.fromAccount == accountName || entry.fromAccount == UBChain::getInstance()->addressMap.value(accountName).ownerAddress)
        {
            if( entry.toAccount == accountName)
            {
                result.opposite = entry.toAccount;
                result.type     = 0;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
            }
            else
            {
                result.opposite = entry.toAccount;
                result.type     = 1;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
            }
        }
        else
        {
            if( entry.toAccount == accountName || entry.toAccount == UBChain::getInstance()->addressMap.value(accountName).ownerAddress)
            {
                if(info.entries.size() > 1)     entry = info.entries.at(1);

                result.opposite = entry.fromAccount;
                result.type     = 2;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
            }
        }

    }


    return result;
}

void AccountPage::showContractTransactions()
{
    ui->accountTransactionsTableWidget->setColumnHidden(6,true);
    ui->accountTransactionsTableWidget->setColumnWidth(0,90);
    ui->accountTransactionsTableWidget->setColumnWidth(1,90);
    ui->accountTransactionsTableWidget->setColumnWidth(2,120);
    ui->accountTransactionsTableWidget->setColumnWidth(3,120);
    ui->accountTransactionsTableWidget->setColumnWidth(4,120);
    ui->accountTransactionsTableWidget->setColumnWidth(5,90);
    ui->accountTransactionsTableWidget->setColumnWidth(6,0);

    QString contractAddress = UBChain::getInstance()->currentTokenAddress;
    QString accountAddress = UBChain::getInstance()->addressMap.value(accountName).ownerAddress;

    ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);

    ContractTransactionVector vector = UBChain::getInstance()->accountContractTransactionMap.value(accountAddress + "-" + contractAddress).transactionVector;

    ui->accountTransactionsTableWidget->setRowCount(0);
    if( vector.size() < 1)
    {
        ui->firstPageBtn->hide();
        ui->prePageBtn->hide();
        ui->numberLabel->hide();
        ui->lastPageBtn->hide();
        ui->pageLineEdit->hide();
        ui->pageLabel->hide();
        ui->nextPageBtn->hide();
        ui->initLabel->show();
        return;
    }
    ui->firstPageBtn->show();
    ui->prePageBtn->show();
    ui->numberLabel->show();
    ui->lastPageBtn->show();
    ui->pageLineEdit->show();
    ui->pageLabel->show();
    ui->nextPageBtn->show();
    ui->initLabel->hide();

    int size = vector.size();
    ui->numberLabel->setText( tr("Total ") + QString::number( size) );
    ui->pageLabel->setText( "/ " + QString::number( (size - 1)/10 + 1 ) );

    int rowCount = size - (currentPageIndex - 1) * 10;
    if( rowCount > 10 )  rowCount = 10;  // 一页最多显示10行
    ui->accountTransactionsTableWidget->setRowCount(rowCount);

    for(int i = rowCount - 1; i > -1; i--)
    {
        ui->accountTransactionsTableWidget->setRowHeight(i,57);
        ContractTransaction transaction = vector.at(  size - ( i + 1) - (currentPageIndex - 1) * 10 );

        // 时间戳
        QString date = transaction.timeStamp;
        date.replace(QString("T"),QString(" "));
        QDateTime time = QDateTime::fromString(date, "yyyy-MM-dd hh:mm:ss");
        time = time.addSecs(28800);       // 时间加8小时
        QString currentDateTime = time.toString("yyyy-MM-dd\r\nhh:mm:ss");
        ui->accountTransactionsTableWidget->setItem(i,0,new QTableWidgetItem(currentDateTime));
        ui->accountTransactionsTableWidget->item(i,0)->setTextColor(QColor(192,196,212));

        // 区块高度
        ui->accountTransactionsTableWidget->setItem(i,1,new QTableWidgetItem( QString::number(transaction.blockNum) ));
        ui->accountTransactionsTableWidget->item(i,1)->setTextColor(QColor(192,196,212));

        // 交易ID
        ui->accountTransactionsTableWidget->setItem(i,2,new QTableWidgetItem( transaction.trxId));
        ui->accountTransactionsTableWidget->item(i,2)->setTextColor(QColor(192,196,212));

        // 对方账户
        QString opposite;
        int type = 0;   // 发送 0    接收 1  自己转自己 2    初始化 3   增发 4
        if( transaction.method == "transfer")
        {
            if( transaction.fromAddress == accountAddress)
            {
                if( transaction.toAddress == accountAddress)
                {
                    type = 2;
                }
                else
                {
                    type = 0;
                }
                opposite = transaction.toAddress;
            }
            else
            {
                opposite = transaction.fromAddress;
                type = 1;
            }
        }
        else if( transaction.method == "init_token")
        {
            opposite = tr("init contract");
            type = 3;
        }
        else if( transaction.method == "mint")
        {
            opposite = tr("mint");
            type = 4;
        }
        else if( transaction.method == "destroy")
        {
            opposite = tr("destroy");
            type = 0;
        }
        else if( transaction.method == "retire")
        {
            opposite = tr("retire");
            type = 0;
        }

        ui->accountTransactionsTableWidget->setItem(i,3,new QTableWidgetItem( UBChain::getInstance()->addressToName(opposite)));
        ui->accountTransactionsTableWidget->item(i,3)->setTextColor(QColor(192,196,212));

        // 金额
        if( type == 0)
        {
            if( transaction.method == "retire")
            {
                ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( "-"));
            }
            else
            {
                ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( "-" + getBigNumberString(transaction.amount,info.precision) + " "
                                                                                      + info.contractName));
            }
            ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(239,104,60));
        }
        else if( type == 1 || type == 3 || type == 4)
        {
            ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( "+" + getBigNumberString(transaction.amount,info.precision) + " "
                                                                                  + info.contractName));
            ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(113,203,90));
        }
        else
        {
            ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( getBigNumberString(transaction.amount,info.precision) + " "
                                                                                  + info.contractName));
        }


        // 手续费
        if( type == 1)
        {
            ui->accountTransactionsTableWidget->setItem(i,5,new QTableWidgetItem( QString::number(0)));
            ui->accountTransactionsTableWidget->item(i,5)->setTextColor(QColor(255,34,76));
        }
        else
        {
            ui->accountTransactionsTableWidget->setItem(i,5,new QTableWidgetItem( getBigNumberString(transaction.fee,ASSET_PRECISION)));
            ui->accountTransactionsTableWidget->item(i,5)->setTextColor(QColor(255,34,76));
        }


        for(int j = 0; j < 6; j++)
        {
            ui->accountTransactionsTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);

            if(i % 2)
            {
                ui->accountTransactionsTableWidget->item(i,j)->setBackgroundColor(QColor(43,49,69));
            }
            else
            {
                ui->accountTransactionsTableWidget->item(i,j)->setBackgroundColor(QColor(40,46,66));
            }
        }

    }


    ui->accountTransactionsTableWidget->sortByColumn(0,Qt::DescendingOrder);
}


//void AccountPage::on_accountTransactionsTableWidget_cellClicked(int row, int column)
//{
//    if( column == 1 )
//    {
//        ShowContentDialog showContentDialog( ui->accountTransactionsTableWidget->item(row, column)->text(),this);
//        int scrollBarValue = ui->accountTransactionsTableWidget->verticalScrollBar()->sliderPosition();
//        showContentDialog.move( ui->accountTransactionsTableWidget->mapToGlobal( QPoint(120, 57 * (row - scrollBarValue) + 42)));
//        showContentDialog.exec();

//        return;
//    }

//    if( column == 5)
//    {

//        QString remark = ui->accountTransactionsTableWidget->item(row, column)->text();
//        remark.remove(' ');
//        if( remark.isEmpty() )  return;
//        ShowContentDialog showContentDialog( ui->accountTransactionsTableWidget->item(row, column)->text(),this);
//        int scrollBarValue = ui->accountTransactionsTableWidget->verticalScrollBar()->sliderPosition();
//        showContentDialog.move( ui->accountTransactionsTableWidget->mapToGlobal( QPoint(640, 57 * (row - scrollBarValue) + 42)));
//        showContentDialog.exec();
//        return;
//    }
//}


void AccountPage::on_prePageBtn_clicked()
{
    ui->accountTransactionsTableWidget->scrollToTop();
    if( currentPageIndex == 1) return;
    currentPageIndex--;
    showTransactions();
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

}

void AccountPage::on_nextPageBtn_clicked()
{
//    if( currentPageIndex >=  ((searchList.size() - 1)/10 + 1))  return;
    int totalPageNum = ui->pageLabel->text().remove("/").remove(" ").toInt();
    if(  currentPageIndex >= totalPageNum )  return;

    currentPageIndex++;
    showTransactions();
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->accountTransactionsTableWidget->scrollToTop();
}

void AccountPage::on_firstPageBtn_clicked()
{
    if( currentPageIndex == 1) return;
    currentPageIndex = 1;
    showTransactions();
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->accountTransactionsTableWidget->scrollToTop();
}

void AccountPage::on_lastPageBtn_clicked()
{
    int totalPageNum = ui->pageLabel->text().remove("/").remove(" ").toInt();
    if(  currentPageIndex >= totalPageNum )  return;

    currentPageIndex = totalPageNum;
    showTransactions();
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->accountTransactionsTableWidget->scrollToTop();
}


void AccountPage::on_pageLineEdit_editingFinished()
{
    currentPageIndex = ui->pageLineEdit->text().toInt();
    showTransactions();
}

void AccountPage::on_pageLineEdit_textEdited(const QString &arg1)
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


void AccountPage::on_withdrawBtn_clicked()
{
    QString contractFee = ui->contractFeeLabel->text();
    contractFee.remove(" UB");

    if(contractFee.toDouble() * ASSET_PRECISION > UBChain::getInstance()->transactionFee)
    {
        CheckPwdDialog checkPwdDialog;
        if(!checkPwdDialog.pop())   return;

        unsigned long long feeToWithdraw = contractFee.toDouble() * ASSET_PRECISION - UBChain::getInstance()->transactionFee;
        UBChain::getInstance()->postRPC( "id_wallet_get_contract_fee_" + accountName, toJsonFormat( "wallet_get_contract_fee", QStringList() << accountName << getBigNumberString(feeToWithdraw,ASSET_PRECISION) ));
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("You can not receive any contract fee!"));
        commonDialog.pop();
    }

}

void AccountPage::on_accountTransactionsTableWidget_cellPressed(int row, int column)
{
    if( column == 2 )
    {
        ShowContentDialog showContentDialog( ui->accountTransactionsTableWidget->item(row, column)->text(),this);

        int x = ui->accountTransactionsTableWidget->columnViewportPosition(column) + ui->accountTransactionsTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->accountTransactionsTableWidget->rowViewportPosition(row) - 10 + ui->accountTransactionsTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->accountTransactionsTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if( column == 3 )
    {
        ShowContentDialog showContentDialog( ui->accountTransactionsTableWidget->item(row, column)->text(),this);

        int x = ui->accountTransactionsTableWidget->columnViewportPosition(column) + ui->accountTransactionsTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->accountTransactionsTableWidget->rowViewportPosition(row) - 10 + ui->accountTransactionsTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->accountTransactionsTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if( column == 6 )
    {
        ShowContentDialog showContentDialog( ui->accountTransactionsTableWidget->item(row, column)->text(),this);

        int x = ui->accountTransactionsTableWidget->columnViewportPosition(column) + ui->accountTransactionsTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->accountTransactionsTableWidget->rowViewportPosition(row) - 10 + ui->accountTransactionsTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->accountTransactionsTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();



        return;
    }
}

void AccountPage::on_qrcodeBtn_clicked()
{
    QRCodeDialog qrcodeDialog(UBChain::getInstance()->addressMapValue(accountName).ownerAddress);
    qrcodeDialog.move(ui->qrcodeBtn->mapToGlobal( QPoint(20,0)));
    qrcodeDialog.exec();
}

