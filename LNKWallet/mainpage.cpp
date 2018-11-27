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
#include "dialog/ErrorResultDialog.h"
#include "control/AssetIconItem.h"
#include "control/BlankDefaultWidget.h"
#include "miner/registerdialog.h"

#include "capitalTransferPage/CaptialNotify.h"

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
	

    ui->setupUi(this);
    InitStyle();
    ui->backupBtn->setVisible(HXChain::getInstance()->IsBackupNeeded);
    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));


    ui->accountTableWidget->installEventFilter(this);
    ui->accountTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->accountTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->accountTableWidget->setFrameShape(QFrame::NoFrame);
    ui->accountTableWidget->setMouseTracking(true);
    ui->accountTableWidget->setShowGrid(false);//隐藏表格线

    ui->accountTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->accountTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->accountTableWidget->horizontalHeader()->setVisible(true);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->accountTableWidget->setColumnWidth(0,140);
    ui->accountTableWidget->setColumnWidth(1,180);
    ui->accountTableWidget->setColumnWidth(2,80);
    ui->accountTableWidget->setColumnWidth(3,80);
    ui->accountTableWidget->setColumnWidth(4,80);
    ui->accountTableWidget->setColumnWidth(5,80);
    ui->accountTableWidget->horizontalHeader()->setStretchLastSection(true);

//    QString language = HXChain::getInstance()->language;
//    if( language.isEmpty())
//    {
//        retranslator("Simplified Chinese");
//    }
//    else
//    {
//        retranslator(language);
//    }


    blankWidget = new BlankDefaultWidget(ui->accountTableWidget);
    blankWidget->setTextTip(tr("there's no account or assest!"));
    // 由于首页是第一个页面，第一次打开先等待x秒钟 再 updateAccountList
    QTimer::singleShot(10, this, SLOT(init()));

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->accountTableWidget);

    activeTunnelMoneyNotify();
}

MainPage::~MainPage()
{
    delete ui;
}

QString toThousandFigure( int);

void MainPage::updateAccountList()
{
    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    ui->addressLabel->setText(info.address);
    ui->pubKeyLabel->setText( info.pubKey);


    AssetAmountMap map = info.assetAmountMap;
    QStringList keys = HXChain::getInstance()->assetInfoMap.keys();

    int size = keys.size();
    ui->accountTableWidget->setRowCount(0);
    ui->accountTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->accountTableWidget->setRowHeight(i,40);

        QString assetId = keys.at(i);
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetId);

        //资产名
        QString symbol = assetInfo.symbol;
        ui->accountTableWidget->setItem(i,0,new QTableWidgetItem(symbol));

        //数量
        ui->accountTableWidget->setItem(i,1,new QTableWidgetItem(getBigNumberString(map.value(assetId).amount, assetInfo.precision)));

        ui->accountTableWidget->setItem(i,2,new QTableWidgetItem(tr("transfer")));
        if(ui->accountTableWidget->item(i,0)->text() != ASSET_NAME)
        {
            ui->accountTableWidget->setItem(i,3,new QTableWidgetItem(tr("deposit")));
            ui->accountTableWidget->setItem(i,4,new QTableWidgetItem(tr("withdraw")));
            ui->accountTableWidget->setItem(i,5,new QTableWidgetItem(tr("allot")));
        }

        for(int j = 2;j < 6;++j)
        {
            if(ui->accountTableWidget->item(i,j))
            {
                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->accountTableWidget->item(i,j)->text());
                ui->accountTableWidget->setCellWidget(i,j,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&MainPage::on_accountTableWidget_cellClicked,this,i,j));
                if(5 == j && captialNotify)
                {
                    toolButton->setRedpointVisiable(!captialNotify->isAccountTunnelMoneyEmpty(ui->accountComboBox->currentText(),symbol));
                }
            }
        }

        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->accountTableWidget->item(i,0)->text());
        ui->accountTableWidget->setCellWidget(i, 0, assetIconItem);

    }

    tableWidgetSetItemZebraColor(ui->accountTableWidget);

    if(ui->accountComboBox->count() == 0)
    {
        ui->accountTableWidget->clearContents();
        ui->accountTableWidget->setRowCount(0);
        //blankWidget->setVisible(true);
    }
    else
    {
        //blankWidget->setVisible(false);
    }
}

void MainPage::on_addAccountBtn_clicked()
{
    NameDialog nameDialog;
    QString name = nameDialog.pop();

    if( !name.isEmpty())
    {
        HXChain::getInstance()->postRPC( "id-wallet_create_account-" + name, toJsonFormat( "wallet_create_account", QJsonArray() << name ));

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
        connect(deposit,&DepositPage::backBtnVisible,this,&MainPage::backBtnVisible);
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
        connect(withdraw,&WithdrawPage::backBtnVisible,this,&MainPage::backBtnVisible);
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
        connect(capital,&CapitalTransferPage::backBtnVisible,this,&MainPage::backBtnVisible);
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
    ui->backupBtn->setVisible(HXChain::getInstance()->IsBackupNeeded);

    updateAccountList();

    if(ui->accountComboBox->count() == 0)
    {
        blankWidget->setVisible(true);
    }
    else
    {
        blankWidget->setVisible(false);
    }

}

void MainPage::init()
{
//    refreshOrNot = false;

//    updateAccountList();

//    refreshOrNot = true;

     ui->accountComboBox->clear();
     QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
     ui->accountComboBox->addItems(accounts);

     if(accounts.contains(HXChain::getInstance()->currentAccount))
     {
         ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
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
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result.startsWith(QString("\"result\":\"%1").arg(ACCOUNT_ADDRESS_PREFIX)))
        {
//            CommonDialog commonDialog(CommonDialog::OkOnly);
//            commonDialog.setText( tr("Please backup up the wallet!!!") );
//            commonDialog.pop();

            HXChain::getInstance()->autoSaveWalletFile();

            HXChain::getInstance()->configFile->setValue("/settings/backupNeeded",true);
            HXChain::getInstance()->IsBackupNeeded = true;

            BackupWalletDialog backupWalletDialog;
            backupWalletDialog.pop();


            // 写入 track-address
            result.prepend("{");
            result.append("}");
            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QString address = jsonObject.take("result").toString();
            HXChain::getInstance()->witnessConfig->addTrackAddress(address);
            HXChain::getInstance()->witnessConfig->save();


            QTimer::singleShot(5000,this,&MainPage::init);
        }
        else if(result.startsWith("\"error\":"))
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Fail to create account!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        return;
    }

    if("id-refresh-account" == id)
    {
        QTimer::singleShot(5000,this,&MainPage::init);
    }

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


void MainPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;
    HXChain::getInstance()->currentAccount = ui->accountComboBox->currentText();

    refresh();
}

void MainPage::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->addressLabel->text());
}

void MainPage::on_copyBtn2_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->pubKeyLabel->text());
}

void MainPage::on_qrcodeBtn_clicked()
{
    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    QRCodeDialog qrcodeDialog(info.address);
    qrcodeDialog.move(ui->qrcodeBtn->mapToGlobal( QPoint(20,0)));
    qrcodeDialog.exec();
}


void MainPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);


    ui->accountTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);


    ui->copyBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/copy.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                               "QToolButton:hover{background-image:url(:/ui/wallet_ui/copy_hover.png);}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));
    ui->copyBtn2->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/copy.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                               "QToolButton:hover{background-image:url(:/ui/wallet_ui/copy_hover.png);}");
    ui->copyBtn2->setToolTip(tr("copy to clipboard"));

    ui->qrcodeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/qrcode.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                 "QToolButton:hover{background-image:url(:/ui/wallet_ui/qrcode_hover.png);}");

    ui->allTransactionBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->importAccountBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->addAccountBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->registerBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
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



void MainPage::on_registerBtn_clicked()
{
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

    if(HXChain::getInstance()->getUnregisteredAccounts().isEmpty())
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("There are no unregistered accounts in the wallet!"));
        commonDialog.pop();
    }
    else
    {
        RegisterDialog registerDialog;
        registerDialog.pop();
    }
}

void MainPage::activeTunnelMoneyNotify()
{
    captialNotify = new CaptialNotify(this);
    connect(captialNotify,&CaptialNotify::checkTunnelMoneyFinish,[this](){
        QString accountName = this->ui->accountComboBox->currentText();
        //获取该账户所有的tunnel余额，判断是否充足
        for(int i = 0;i < this->ui->accountTableWidget->rowCount();++i)
        {
            QString assetName = this->ui->accountTableWidget->item(i,0)->text();
            bool isMoneyEmpty = this->captialNotify->isAccountTunnelMoneyEmpty(accountName,assetName);
            if(ToolButtonWidget *button = dynamic_cast<ToolButtonWidget*>(this->ui->accountTableWidget->cellWidget(i,5)))
            {
                qDebug()<<"nnnnnnnnnn"<<accountName<<assetName<<isMoneyEmpty;
                button->setRedpointVisiable(!isMoneyEmpty);
            }

        }
    });
    //开启自动获取划转账户金额提醒
    captialNotify->startCheckTunnelMoney();

}
