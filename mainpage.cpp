#include <QDebug>

#include <QTimer>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMovie>
#include <QClipboard>


#include "mainpage.h"
#include "ui_mainpage.h"
#include "wallet.h"

#include "namedialog.h"
#include "exportdialog.h"
#include "importdialog.h"
#include "commondialog.h"
#include "showcontentdialog.h"
#include "dialog/backupwalletdialog.h"
#include "control/qrcodedialog.h"

#include "depositpage/DepositPage.h"
#include "withdrawpage/WithdrawPage.h"
#include "capitalTransferPage/CapitalTransferPage.h"
#include "ToolButtonWidget.h"
#include "alltransactionwidget.h"

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    hasDelegateOrNot(false),
    refreshOrNot(false),
    currentAccountIndex(-1),
    ui(new Ui::MainPage)
{
	

    ui->setupUi(this);
    InitStyle();
    ui->backupBtn->setVisible(UBChain::getInstance()->IsBackupNeeded);
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

    ui->accountTableWidget->setColumnWidth(0,185);
    ui->accountTableWidget->setColumnWidth(1,185);
    ui->accountTableWidget->setColumnWidth(2,74);
    ui->accountTableWidget->setColumnWidth(3,74);
    ui->accountTableWidget->setColumnWidth(4,74);
    ui->accountTableWidget->setColumnWidth(5,74);
    ui->accountTableWidget->horizontalHeader()->setStretchLastSection(true);

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


}

MainPage::~MainPage()
{
	

    delete ui;

	
}

QString toThousandFigure( int);

void MainPage::updateAccountList()
{
    AccountInfo info = UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    ui->addressLabel->setText(info.address);


    AssetAmountMap map = info.assetAmountMap;
    QStringList keys = UBChain::getInstance()->assetInfoMap.keys();

    int size = keys.size();
    ui->accountTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->accountTableWidget->setRowHeight(i,37);

        QString assetId = keys.at(i);
        AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(assetId);

        //资产名
        QString symbol = assetInfo.symbol;
        ui->accountTableWidget->setItem(i,0,new QTableWidgetItem(symbol));

        //数量
        ui->accountTableWidget->setItem(i,1,new QTableWidgetItem(getBigNumberString(map.value(assetId).amount, assetInfo.precision)));

        ui->accountTableWidget->setItem(i,2,new QTableWidgetItem(tr("transfer")));
        if(ui->accountTableWidget->item(i,0)->text() != "LNK")
        {
            ui->accountTableWidget->setItem(i,3,new QTableWidgetItem(tr("deposit")));
            ui->accountTableWidget->setItem(i,4,new QTableWidgetItem(tr("withdraw")));
            ui->accountTableWidget->setItem(i,5,new QTableWidgetItem(tr("allot")));
        }

        for(int j = 0; j < 6; j++)
        {
            if(ui->accountTableWidget->item(i,j))
            {
                ui->accountTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
            }

            //ui->accountTableWidget->item(i,j)->setTextColor(QColor(192,196,212));
            //
            //if(i % 2)
            //{
            //    ui->accountTableWidget->item(i,j)->setBackgroundColor(QColor(43,49,69));
            //}
            //else
            //{
            //    ui->accountTableWidget->item(i,j)->setBackgroundColor(QColor(40,46,66));
            //}
        }
        for(int j = 2;j < 6;++j)
        {
            if(ui->accountTableWidget->item(i,j))
            {
                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setInitNone(true);
                toolButton->setText(ui->accountTableWidget->item(i,j)->text());
                ui->accountTableWidget->setCellWidget(i,j,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&MainPage::on_accountTableWidget_cellClicked,this,i,j));
            }
        }
    }
    if(ui->accountComboBox->count() == 0)
    {
        ui->accountTableWidget->clearContents();
        ui->accountTableWidget->setRowCount(0);
    }
}

void MainPage::on_addAccountBtn_clicked()
{
    NameDialog nameDialog;
    QString name = nameDialog.pop();

    if( !name.isEmpty())
    {
        UBChain::getInstance()->postRPC( "id-wallet_create_account-" + name, toJsonFormat( "wallet_create_account", QJsonArray() << name ));

    }

}

void MainPage::on_importAccountBtn_clicked()
{
    ImportDialog importDialog;
//    connect(&importDialog,SIGNAL(accountImported()),this,SLOT(refresh()));
    importDialog.pop();

    init();
}


void MainPage::on_accountTableWidget_cellClicked(int row, int column)
{

    if(!ui->accountTableWidget->item(row,column))
    {
        return;
    }
    if(column == 2)
    {//转账界面
        emit backBtnVisible(true);
        emit showTransferPage( ui->accountComboBox->currentText(),ui->accountTableWidget->item(row,0)->text());
        return;
    }

    if( 3 == column )
    {//充值界面
        emit backBtnVisible(true);
        DepositPage *deposit = new DepositPage(DepositPage::DepositDataInput(ui->accountComboBox->currentText(),
                                               ui->addressLabel->text(),ui->accountTableWidget->item(row,0)->text()),this);
        deposit->setAttribute(Qt::WA_DeleteOnClose);
        deposit->show();
        deposit->raise();
        return;
    }

    if( 4 == column )
    {//提现界面
        emit backBtnVisible(true);
        WithdrawPage *withdraw = new WithdrawPage(WithdrawPage::WithdrawDataInput(ui->accountComboBox->currentText(),
                                                  ui->addressLabel->text(),ui->accountTableWidget->item(row,0)->text(),
                                                  ui->accountTableWidget->item(row,1)->text()),this);
        withdraw->setAttribute(Qt::WA_DeleteOnClose);
        withdraw->show();
        withdraw->raise();
        return;
    }

    if( 5 == column)
    {//资金划转界面
        emit backBtnVisible(true);
        CapitalTransferPage *capital = new CapitalTransferPage(CapitalTransferPage::CapitalTransferInput(
                                           ui->accountComboBox->currentText(),ui->addressLabel->text(),ui->accountTableWidget->item(row,0)->text()),this);
        capital->setAttribute(Qt::WA_DeleteOnClose);
        capital->show();
        capital->raise();
        return;

    }
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
//    qDebug() << "mainpage refresh"   << refreshOrNot;
//    if( !refreshOrNot) return;
    ui->backupBtn->setVisible(UBChain::getInstance()->IsBackupNeeded);

    updateAccountList();

}

void MainPage::init()
{
//    refreshOrNot = false;

    updateAccountList();
//    updateTotalBalance();

//    refreshOrNot = true;

     ui->accountComboBox->clear();
     QStringList accounts = UBChain::getInstance()->accountInfoMap.keys();
     ui->accountComboBox->addItems(accounts);

     if(accounts.contains(UBChain::getInstance()->currentAccount))
     {
         ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
     }

     inited = true;

     refresh();
}


void MainPage::paintEvent(QPaintEvent *)
{
    //QPainter painter(this);
    //painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    //painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
    //
    //painter.drawRect(rect());
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

        if(result.startsWith(QString("\"result\":\"%1").arg(ACCOUNT_ADDRESS_PREFIX)))
        {
//            CommonDialog commonDialog(CommonDialog::OkOnly);
//            commonDialog.setText( tr("Please backup up the wallet!!!") );
//            commonDialog.pop();

            UBChain::getInstance()->autoSaveWalletFile();

            UBChain::getInstance()->configFile->setValue("/settings/backupNeeded",true);
            UBChain::getInstance()->IsBackupNeeded = true;

            BackupWalletDialog backupWalletDialog;
            backupWalletDialog.pop();


            // 写入 track-address
            result.prepend("{");
            result.append("}");
            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QString address = jsonObject.take("result").toString();
            UBChain::getInstance()->addTrackAddress(address);

            QTimer::singleShot(5000,this,&MainPage::init);
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

    if("id-refresh-account" == id)
    {
        QTimer::singleShot(5000,this,&MainPage::init);
    }

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
//    if( watched == ui->accountTableWidget)
//    {
//        if( e->type() == QEvent::ContextMenu)
//        {
//            QContextMenuEvent* contextMenuEvent = static_cast<QContextMenuEvent*>(e);
//            QPoint pos = contextMenuEvent->pos();
//            pos -= QPoint(0,47);
//            int row = ui->accountTableWidget->row(ui->accountTableWidget->itemAt(pos));
//            if( row == -1)    return false;

//            if( ui->accountTableWidget->item(row,0) == NULL)   return false;
//            QString name = ui->accountTableWidget->item(row,0)->text();
//            RightClickMenuDialog* rightClickMenuDialog = new RightClickMenuDialog( name, this);
//            rightClickMenuDialog->move( ui->accountTableWidget->mapToGlobal(contextMenuEvent->pos()) );
//            connect( rightClickMenuDialog, SIGNAL(transferFromAccount(QString)), this, SIGNAL(showTransferPage(QString)));
//            connect( rightClickMenuDialog, SIGNAL(renameAccount(QString)), this, SLOT(renameAccount(QString)));
//            connect( rightClickMenuDialog, SIGNAL(exportAccount(QString)), this, SLOT(showExportDialog(QString)));
//            connect( rightClickMenuDialog, SIGNAL(deleteAccount(QString)), this, SLOT(deleteAccount(QString)));
//            rightClickMenuDialog->exec();

//            return true;


//        }
//    }

    return QWidget::eventFilter(watched,e);
}


void MainPage::showExportDialog(QString name)
{
    ExportDialog exportDialog(name);
    exportDialog.pop();
}


void MainPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;
    UBChain::getInstance()->currentAccount = ui->accountComboBox->currentText();

    updateAccountList();
}

void MainPage::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->addressLabel->text());

//    CommonDialog commonDialog(CommonDialog::OkOnly);
//    commonDialog.setText(tr("Copy to clipboard"));
//    commonDialog.pop();
}

void MainPage::on_qrcodeBtn_clicked()
{
    AccountInfo info = UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    QRCodeDialog qrcodeDialog(info.address);
    qrcodeDialog.move(ui->qrcodeBtn->mapToGlobal( QPoint(20,0)));
    qrcodeDialog.exec();
}


void MainPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    QPalette pe;
    pe.setColor(QPalette::WindowText,QColor(0xC6CAD4));
    ui->label_op->setPalette(pe);
    QFont font("\"Microsoft YaHei UI Light\" ",20,50);
    font.setPixelSize(12);
    ui->label_op->setFont(font);


    ui->accountTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);


    ui->copyBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/copy.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                               "QToolButton:hover{background-image:url(:/ui/wallet_ui/copy_hover.png);}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));

    ui->qrcodeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/qrcode.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                 "QToolButton:hover{background-image:url(:/ui/wallet_ui/qrcode_hover.png);}");

    ui->allTransactionBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->importAccountBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->addAccountBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
//    ui->accountComboBox->setStyleSheet("QComboBox{border: none;background:transparent;font: 12pt \"Microsoft YaHei UI Light\";\
//               background-position: center left;color: black;selection-background-color: darkgray;}");


    ui->backupBtn->adjustSize();
    ui->backupBtn->setGeometry(ui->backupBtn->x(),ui->backupBtn->y(),ui->backupBtn->width(),20);
}

void MainPage::on_allTransactionBtn_clicked()
{
    emit backBtnVisible(true);

    AllTransactionWidget* allTransactionWidget = new AllTransactionWidget(this);
    allTransactionWidget->setAttribute(Qt::WA_DeleteOnClose);
    allTransactionWidget->show();
    allTransactionWidget->raise();
}

void MainPage::on_backupBtn_clicked()
{
    BackupWalletDialog backupWalletDialog;
    backupWalletDialog.pop();
}
