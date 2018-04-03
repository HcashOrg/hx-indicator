#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMovie>

#include "mainpage.h"
#include "ui_mainpage.h"
#include "lnk.h"
#include "debug_log.h"
#include "namedialog.h"
#include "deleteaccountdialog.h"
#include "exportdialog.h"
#include "importdialog.h"
#include "commondialog.h"
#include "showcontentdialog.h"
#include "control/rightclickmenudialog.h"
#include "control/chooseaddaccountdialog.h"
#include "dialog/renamedialog.h"
#include "dialog/backupwalletdialog.h"

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    hasDelegateOrNot(false),
    refreshOrNot(false),
    currentAccountIndex(-1),
    ui(new Ui::MainPage)
{
	

    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));


    ui->accountTableWidget->installEventFilter(this);
    ui->accountTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->accountTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->accountTableWidget->setFrameShape(QFrame::NoFrame);
    ui->accountTableWidget->setMouseTracking(true);
    ui->accountTableWidget->setShowGrid(false);//隐藏表格线

    previousColorRow = 0;
    ui->accountTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->accountTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->accountTableWidget->horizontalHeader()->setVisible(true);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    connect(ui->accountTableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(changeSortMode(int)));

    ui->accountTableWidget->setColumnWidth(0,160);
    ui->accountTableWidget->setColumnWidth(1,320);
    ui->accountTableWidget->setColumnWidth(2,160);

    QString language = UBChain::getInstance()->language;
    if( language.isEmpty())
    {
        retranslator("English");
    }
    else
    {
        retranslator(language);
    }


    // 由于首页是第一个页面，第一次打开先等待x秒钟 再 updateAccountList
    QTimer::singleShot(500, this, SLOT(init()));

    ui->accountTableWidget->hide();

    ui->loadingWidget->setGeometry(0,93,827,448);
    ui->loadingLabel->move(314,101);

    ui->initLabel->hide();

}

MainPage::~MainPage()
{
	

    delete ui;

	
}

QString toThousandFigure( int);

void MainPage::importAccount()
{
    

    ImportDialog importDialog;
    connect(&importDialog,SIGNAL(accountImported()),this,SLOT(refresh()));
    importDialog.pop();

    emit refreshAccountInfo();

    
}

void MainPage::addAccount()
{
    NameDialog nameDialog;
    QString name = nameDialog.pop();

    if( !name.isEmpty())
    {
        UBChain::getInstance()->postRPC( "id-wallet_create_account-" + name, toJsonFormat( "wallet_create_account", QStringList() << name ));

    }


}


void MainPage::updateAccountList()
{

    mutexForConfigFile.lock();
    UBChain::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = UBChain::getInstance()->configFile->childKeys();

    QStringList accounts;
    foreach (QString key, keys)
    {
        accounts.append(UBChain::getInstance()->configFile->value(key).toString());
    }
    UBChain::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();

    sortAccountsByBalance(accounts);

    int size = accounts.size();
    if( size == 0)  // 如果还没有账户
    {
        ui->initLabel->show();
        ui->accountTableWidget->hide();
        ui->loadingWidget->hide();

        return;
    }
    else
    {
        ui->initLabel->hide();
        ui->accountTableWidget->show();
        ui->loadingWidget->show();
    }


    if( UBChain::getInstance()->currentTokenAddress.isEmpty())
    {
        AssetInfo info = UBChain::getInstance()->assetInfoMap.value(0);
        QTableWidgetItem* item = ui->accountTableWidget->horizontalHeaderItem(2);
        item->setText(tr("Balance/") + info.symbol);

        ui->accountTableWidget->setRowCount(size);
        for( int i = size - 1; i > -1; i--)
        {
            QString accountName = accounts.at(i);
            int rowNum = i;

            ui->accountTableWidget->setRowHeight(rowNum,37);
            ui->accountTableWidget->setItem(rowNum,0,new QTableWidgetItem(accountName));
            ui->accountTableWidget->setItem(rowNum,1,new QTableWidgetItem(UBChain::getInstance()->addressMapValue(accountName).ownerAddress));


            AssetBalanceMap map = UBChain::getInstance()->accountBalanceMap.value(accountName);
            ui->accountTableWidget->setItem(rowNum,2,new QTableWidgetItem(getBigNumberString(map.value(0),info.precision)));


            ui->accountTableWidget->item(rowNum,0)->setTextColor(QColor(192,196,212));
            ui->accountTableWidget->item(rowNum,1)->setTextColor(QColor(192,196,212));
            ui->accountTableWidget->item(rowNum,2)->setTextColor(QColor(24,250,239));

            for( int j = 0; j < 3; j++)
            {
                ui->accountTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);

                if(i % 2)
                {
                    ui->accountTableWidget->item(i,j)->setBackgroundColor(QColor(43,49,69));
                }
                else
                {
                    ui->accountTableWidget->item(i,j)->setBackgroundColor(QColor(40,46,66));
                }
            }

        }

    }
    else
    {
        // 如果是合约资产
        QString contractAddress = UBChain::getInstance()->currentTokenAddress;

        ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);
        QTableWidgetItem* item = ui->accountTableWidget->horizontalHeaderItem(2);
        item->setText(tr("Balance/") + info.contractName);

        ui->accountTableWidget->setRowCount(size);
        for( int i = size - 1; i > -1; i--)
        {
            QString accountName = accounts.at(i);
            int rowNum = i;

            ui->accountTableWidget->setRowHeight(rowNum,37);
            ui->accountTableWidget->setItem(rowNum,0,new QTableWidgetItem(accountName));
            ui->accountTableWidget->setItem(rowNum,1,new QTableWidgetItem(UBChain::getInstance()->addressMapValue(accountName).ownerAddress));


            ContractBalanceMap map = UBChain::getInstance()->accountContractBalanceMap.value(UBChain::getInstance()->addressMap.value(accountName).ownerAddress);
            ui->accountTableWidget->setItem(rowNum,2,new QTableWidgetItem(getBigNumberString(map.value(contractAddress),info.precision)));
    //        ui->accountTableWidget->setItem(rowNum,2,new QTableWidgetItem(UBChain::getInstance()->balanceMapValue(accountName).remove(ASSET_NAME)));
            ui->accountTableWidget->item(rowNum,0)->setTextAlignment(Qt::AlignCenter);
            ui->accountTableWidget->item(rowNum,1)->setTextAlignment(Qt::AlignCenter);
            ui->accountTableWidget->item(rowNum,2)->setTextAlignment(Qt::AlignCenter);

            ui->accountTableWidget->item(rowNum,0)->setTextColor(QColor(192,196,212));
            ui->accountTableWidget->item(rowNum,1)->setTextColor(QColor(192,196,212));
            ui->accountTableWidget->item(rowNum,2)->setTextColor(QColor(24,250,239));

            if(rowNum % 2)
            {
                for( int i = 0; i < 3; i++)
                {
                    ui->accountTableWidget->item(rowNum,i)->setBackgroundColor(QColor(43,49,69));
                }
            }
            else
            {
                for( int i = 0; i < 3; i++)
                {
                    ui->accountTableWidget->item(rowNum,i)->setBackgroundColor(QColor(40,46,66));
                }
            }
        }

    }

    ui->loadingWidget->hide();    
}

void MainPage::on_addAccountBtn_clicked()
{
    addAccount();

//    ChooseAddAccountDialog* chooseAddAccountDialog = new ChooseAddAccountDialog(this);
//    chooseAddAccountDialog->move( ui->addAccountBtn->mapToGlobal( QPoint(10,-79) ) );
//    connect( chooseAddAccountDialog, SIGNAL(newAccount()), this, SLOT( addAccount()));
//    connect( chooseAddAccountDialog, SIGNAL(importAccount()), this, SLOT( importAccount()));
//    chooseAddAccountDialog->exec();
}

void MainPage::on_importAccountBtn_clicked()
{
    importAccount();
}


void MainPage::on_accountTableWidget_cellClicked(int row, int column)
{
    

//    AccountCellWidget* cellWidget = static_cast<AccountCellWidget*>( ui->accountTableWidget->cellWidget(row,0) );

//    emit openAccountPage( cellWidget->accountName);

    emit openAccountPage( ui->accountTableWidget->item(row,0)->text());

    //    showDetailWidget( cellWidget->accountName );

//        showDetailWidget( ui->accountTableWidget->item(row,0)->text() );

//    if( currentAccountIndex >= 0 && currentAccountIndex != row)
//    {
//        AccountCellWidget* cellWidget2 = static_cast<AccountCellWidget*>( ui->accountTableWidget->cellWidget(currentAccountIndex,0) );
//        cellWidget2->setBackgroundColor( 255,255,255);
//        for( int i = 1; i < 3; i++)
//        {
//            ui->accountTableWidget->item(currentAccountIndex,i)->setBackgroundColor(QColor(255,255,255));
//        }
//    }

//    cellWidget->setBackgroundColor( 245,248,248,150);
//    for( int i = 1; i < 3; i++)
//    {
//        ui->accountTableWidget->item(row,i)->setBackgroundColor(QColor(245,248,248,150));
//    }

//    currentAccountIndex = row;

	
}


void MainPage::on_accountTableWidget_cellEntered(int row, int column)
{

//    if( ui->accountTableWidget->rowCount() > 0)
//        {
//            for( int i = 0; i < 3; i++)
//            {
//                if( ui->accountTableWidget->item(previousColorRow,i) != NULL)
//                {
//                    ui->accountTableWidget->item(previousColorRow,i)->setBackgroundColor(QColor(255,255,255));
//                }
//            }
//        }

//        for( int i = 0; i < 3; i++)
//        {
//            if( ui->accountTableWidget->item(row,i) != NULL)
//            {
//                ui->accountTableWidget->item(row,i)->setBackgroundColor(QColor(245,248,248));
//            }
//        }

//    previousColorRow = row;

}

int tableWidgetPosToRow(QPoint pos, QTableWidget* table);

void MainPage::refresh()
{
    qDebug() << "mainpage refresh"   << refreshOrNot;
    if( !refreshOrNot) return;

    updateAccountList();
    updateTotalBalance();

}

void MainPage::init()
{
    refreshOrNot = false;

    updateAccountList();
    updateTotalBalance();

    refreshOrNot = true;
}

void MainPage::sortAccountsByBalance(QStringList &accounts)
{
    for(int i = 0; i < accounts.size(); i++)
    {
        for(int j = 0; j < accounts.size() - i - 1; j++)
        {
            QString account1 = accounts.at(j);
            unsigned long long balance1 = UBChain::getInstance()->accountBalanceMap.value(account1).value(0);

            QString account2 = accounts.at(j + 1);
            unsigned long long balance2 = UBChain::getInstance()->accountBalanceMap.value(account2).value(0);

            if(sortMode == 0)
            {
                if( balance1 < balance2)
                {
                    accounts[j] = account2;
                    accounts[j + 1] = account1;
                }
            }
            else
            {
                if( balance1 > balance2)
                {
                    accounts[j] = account2;
                    accounts[j + 1] = account1;
                }
            }
        }
    }

}

void MainPage::changeSortMode(int _section)
{
    if(_section != 2)   return;
    sortMode = !sortMode;
    updateAccountList();
}

void MainPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));

    painter.drawRect(rect());
    //painter.drawRect(0,0,680,482);

}

void MainPage::retranslator(QString language)
{
    ui->retranslateUi(this);

    if( language == "Simplified Chinese")
    {

    }
    else if( language == "English")
    {

    }
}

void MainPage::jsonDataUpdated(QString id)
{
    if( id.startsWith("id-wallet_create_account-") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << "mmmmmmmm " << id << result;

        QString name = id.mid(QString("id-wallet_create_account-").size());

        if(result.startsWith(QString("\"result\":\"%1").arg(ACCOUNT_ADDRESS_PREFIX)))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Please backup up the wallet!!!") );
            commonDialog.pop();

            BackupWalletDialog backupWalletDialog;
            backupWalletDialog.pop();

        }
        else if(result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Create account failed: " + errorMessage );
            commonDialog.pop();
        }
        else
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Failed"));
            commonDialog.pop();
        }

        return;
    }

}

void MainPage::updateTotalBalance()
{
//    unsigned long long totalBalance = 0;
//    int assetIndex = ui->assetComboBox->currentIndex();
//    if( assetIndex == 0)
//    {
//        AssetInfo info = UBChain::getInstance()->assetInfoMap.value(assetIndex);
//        foreach (QString key, UBChain::getInstance()->accountBalanceMap.keys())
//        {
//            AssetBalanceMap map = UBChain::getInstance()->accountBalanceMap.value(key);
//            totalBalance += map.value(assetIndex);
//        }
//        ui->totalBalanceLabel->setText( "<body><font style=\"font-size:18px\" color=#ff0000>" + getBigNumberString(totalBalance,info.precision) + "</font><font style=\"font-size:12px\" color=#000000> " + info.symbol +"</font></body>" );
//        ui->totalBalanceLabel->adjustSize();
//        ui->scanBtn->move(ui->totalBalanceLabel->x() + ui->totalBalanceLabel->width() + 10,ui->totalBalanceLabel->y() + 7);
//        ui->scanLabel->move(ui->totalBalanceLabel->x() + ui->totalBalanceLabel->width() + 10,ui->totalBalanceLabel->y() + 7);

//    }
//    else
//    {
//        // 如果是合约资产
//        QStringList contracts = UBChain::getInstance()->ERC20TokenInfoMap.keys();
//        QString contractAddress = contracts.at(assetIndex - 1);

//        ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);

//        QStringList keys = UBChain::getInstance()->addressMap.keys();
//        foreach (QString key, keys)
//        {
//            QString accountAddress = UBChain::getInstance()->addressMap.value(key).ownerAddress;
//            totalBalance += UBChain::getInstance()->accountContractBalanceMap.value(accountAddress).value(contractAddress);
//        }
//        ui->totalBalanceLabel->setText( "<body><font style=\"font-size:18px\" color=#ff0000>" + getBigNumberString(totalBalance,info.precision) + "</font><font style=\"font-size:12px\" color=#000000> " + info.contractName +"</font></body>" );
//        ui->totalBalanceLabel->adjustSize();
//        ui->scanBtn->move(ui->totalBalanceLabel->x() + ui->totalBalanceLabel->width() + 10,ui->totalBalanceLabel->y() + 7);
//        ui->scanLabel->move(ui->totalBalanceLabel->x() + ui->totalBalanceLabel->width() + 10,ui->totalBalanceLabel->y() + 7);
//    }
}

void MainPage::updatePending()
{
    
    mutexForPending.lock();

    if( !UBChain::getInstance()->pendingFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "pending.dat not exist";
        return;
    }

    QByteArray ba = QByteArray::fromBase64( UBChain::getInstance()->pendingFile->readAll());
    QString str(ba);
    QStringList strList = str.split(";");
    strList.removeLast();

    mutexForAddressMap.lock();
    QStringList keys = UBChain::getInstance()->addressMap.keys();
    mutexForAddressMap.unlock();

    mutexForConfigFile.lock();
    UBChain::getInstance()->configFile->beginGroup("recordId");
    QStringList recordKeys = UBChain::getInstance()->configFile->childKeys();
    UBChain::getInstance()->configFile->endGroup();


    foreach (QString ss, strList)
    {
        QStringList sList = ss.split(",");

        if( !keys.contains( sList.at(1)) && keys.size() > 0)   // 如果账号被删除了， 删掉pending 中的记录   keys.size() 防止刚启动 addressmap为空
        {
            strList.removeAll( ss);
            continue;
        }
        // 如果config中recordId会被置为1， 则删除该记录
        if( UBChain::getInstance()->configFile->value("recordId/" + sList.at(0)).toInt() != 0 )
        {
            strList.removeAll( ss);
            continue;
        }

        // 如果config中recordId被删除了， 则删除该记录
        if( !UBChain::getInstance()->configFile->contains("recordId/" + sList.at(0)))
        {
            strList.removeAll( ss);
            continue;
        }

    }
    mutexForConfigFile.unlock();


    ba.clear();
    foreach (QString ss, strList)
    {
        ba += QString( ss + ";").toUtf8();
    }
    ba = ba.toBase64();
    UBChain::getInstance()->pendingFile->resize(0);
    QTextStream ts(UBChain::getInstance()->pendingFile);
    ts << ba;
    UBChain::getInstance()->pendingFile->close();

    mutexForPending.unlock();
    
}

//  tablewidget 从 pos 获取 item（每行第0个）
int tableWidgetPosToRow(QPoint pos, QTableWidget* table)
{
    int headerHeight = 30;
    int rowHeight = 37;

    // 获得当前滚动条的位置
    int scrollBarValue = table->verticalScrollBar()->sliderPosition();

    if( pos.y() < headerHeight || pos.y() > table->height())
    {
        return -1;
    }
    else
    {
        int rowCount = floor( (pos.y() - headerHeight) / rowHeight) + scrollBarValue;
        return rowCount;
    }

}

bool MainPage::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->accountTableWidget)
    {
        if( e->type() == QEvent::ContextMenu)
        {
            QContextMenuEvent* contextMenuEvent = static_cast<QContextMenuEvent*>(e);
            QPoint pos = contextMenuEvent->pos();
            pos -= QPoint(0,47);
            int row = ui->accountTableWidget->row(ui->accountTableWidget->itemAt(pos));
            if( row == -1)    return false;

            if( ui->accountTableWidget->item(row,0) == NULL)   return false;
            QString name = ui->accountTableWidget->item(row,0)->text();
            RightClickMenuDialog* rightClickMenuDialog = new RightClickMenuDialog( name, this);
            rightClickMenuDialog->move( ui->accountTableWidget->mapToGlobal(contextMenuEvent->pos()) );
            connect( rightClickMenuDialog, SIGNAL(transferFromAccount(QString)), this, SIGNAL(showTransferPage(QString)));
            connect( rightClickMenuDialog, SIGNAL(renameAccount(QString)), this, SLOT(renameAccount(QString)));
            connect( rightClickMenuDialog, SIGNAL(exportAccount(QString)), this, SLOT(showExportDialog(QString)));
            connect( rightClickMenuDialog, SIGNAL(deleteAccount(QString)), this, SLOT(deleteAccount(QString)));
            rightClickMenuDialog->exec();

            return true;


        }
    }

    return QWidget::eventFilter(watched,e);
}


void MainPage::showExportDialog(QString name)
{

    ExportDialog exportDialog(name);
    exportDialog.pop();
}



void MainPage::renameAccount(QString name)
{
    RenameDialog renameDialog;
    QString newName = renameDialog.pop();

    if( !newName.isEmpty() && newName != name)
    {
        UBChain::getInstance()->write("wallet_account_rename " + name + " " + newName + '\n');
        QString result = UBChain::getInstance()->read();
        qDebug() << result;
        if( result.mid(0,2) == "OK")
        {
            mutexForConfigFile.lock();
            UBChain::getInstance()->configFile->beginGroup("/accountInfo");
            QStringList keys = UBChain::getInstance()->configFile->childKeys();
            foreach (QString key, keys)
            {
                if( UBChain::getInstance()->configFile->value(key) == name)
                {
                    UBChain::getInstance()->configFile->setValue(key, newName);
                    break;
                }
            }
            UBChain::getInstance()->configFile->endGroup();
            mutexForConfigFile.unlock();
            UBChain::getInstance()->balanceMapInsert( newName, UBChain::getInstance()->balanceMapValue(name));
            UBChain::getInstance()->balanceMapRemove(name);
            UBChain::getInstance()->registerMapInsert( newName, UBChain::getInstance()->registerMapValue(name));
            UBChain::getInstance()->registerMapRemove(name);
            UBChain::getInstance()->addressMapInsert( newName, UBChain::getInstance()->addressMapValue(name));
            UBChain::getInstance()->addressMapRemove(name);

//            detailWidget->accountName = newName;
            emit newAccount(newName);

        }
        else
        {
            return;
        }


    }
}

void MainPage::deleteAccount(QString name)
{
    DeleteAccountDialog deleteACcountDialog( name);
    deleteACcountDialog.pop();
}


//void MainPage::hideDetailWidget()
//{
//    detailOrNot = false;

//    ui->accountTableWidget->setColumnWidth(0,173);
//    ui->accountTableWidget->setColumnWidth(1,424);
//    ui->accountTableWidget->setColumnWidth(2,154);

//    ui->addAccountBtn->move(675,422);

//    detailWidget->dynamicHide();
//}

