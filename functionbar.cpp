#include "functionbar.h"
#include "ui_functionbar.h"

#include "lnk.h"
#include "commondialog.h"
#include "dialog/functionbarchoicedialog.h"
#include "setdialog.h"
#include "consoledialog.h"

#include <QPainter>
#include <QDebug>
#include <QLineEdit>
#include <QListView>


#define FUNCTIONPAGE_BTN_SELECTED       "background-color: rgb(20,170,255);color: rgb(255, 255, 255);border:none;"
#define FUNCTIONPAGE_BTN_UNSELECTED     "background-color: rgb(15,160,250);color: rgb(255, 255, 255);border:none;"
#define FUNCTIONPAGE_CHILD_BTN_SELECTED      "background-color: rgb(21,24,37);color: rgb(239,240,240);border:none;"
#define FUNCTIONPAGE_CHILD_BTN_UNSELECTED    "background-color: rgb(24,28,45);color: rgb(160,168,198);border:none;"

FunctionBar::FunctionBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionBar)
{
    
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(16,17,24));
    setPalette(palette);


    ui->accountBtn->setIconSize(QSize(103,94));
    ui->transferBtn->setIconSize(QSize(103,94));
    ui->contactBtn->setIconSize(QSize(103,94));
    ui->advanceBtn->setIconSize(QSize(103,94));


    ui->accountBtn->setStyleSheet("background:transparent;border:none;");
    ui->transferBtn->setStyleSheet("background:transparent;border:none;");
    ui->contactBtn->setStyleSheet("background:transparent;border:none;");
    ui->advanceBtn->setStyleSheet("background:transparent;border:none;");

    ui->logoLabel->setPixmap(QPixmap(":/ui/wallet_ui/logo_26x17.png"));
    ui->versionLabel->setText(QString("v") + WALLET_VERSION);

    ui->assetComboBox->setView(new QListView());

    ui->scanLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->scanLabel->setPixmap(QPixmap(":/ui/wallet_ui/scan.png"));
    ui->scanLabel->setToolTip(tr("If the balance or transaction record is incorrect, rescan the blockchain"));
    ui->scanLabel->installEventFilter(this);

    ui->choiceBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/choice.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                 "QToolButton:hover{background-image:url(:/ui/wallet_ui/choice_hover.png);}");


    timerForScanLabel = new QTimer(this);
    connect(timerForScanLabel,SIGNAL(timeout()),this,SLOT(showScanPic()));
    scanLabelCount = 0;


    choosePage(1);

    refresh();

}

FunctionBar::~FunctionBar()
{
    delete ui;
}

void FunctionBar::on_accountBtn_clicked()
{
    if(currentPageIndex == 1 || currentPageIndex == 4)   return;

    on_accountListBtn_clicked();
}


void FunctionBar::on_transferBtn_clicked()
{
    mutexForAddressMap.lock();
    int size = UBChain::getInstance()->addressMap.size();
    mutexForAddressMap.unlock();
    if( size > 0)   // 有至少一个账户
    {
        choosePage(2);
        emit showTransferPage();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("No account for transfering,\nadd an account first"));
//        commonDialog.move( this->mapToGlobal(QPoint(318,150)));
        commonDialog.pop();
        on_accountBtn_clicked();
    }
}

void FunctionBar::on_contactBtn_clicked()
{
    choosePage(3);
    emit showContactPage();
}

void FunctionBar::on_smartContractBtn_clicked()
{
    choosePage(5);
    emit showSmartContractPage();
}


void FunctionBar::on_feedBtn_clicked()
{
    mutexForAddressMap.lock();
    int size = UBChain::getInstance()->addressMap.size();
    mutexForAddressMap.unlock();
    if( size > 0)   // 有至少一个账户
    {
        choosePage(6);
        emit showFeedPage();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("No accounts now. Please add an account first."));
//        commonDialog.move( this->mapToGlobal(QPoint(318,150)));
        commonDialog.pop();
        on_accountBtn_clicked();
    }
}


void FunctionBar::on_multiSigBtn_clicked()
{
    choosePage(7);
    emit showMultiSigPage();
}


void FunctionBar::choosePage(int pageIndex)
{
    currentPageIndex = pageIndex;
    switch (pageIndex) {
    case 1:
        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn_unselected.png"));

        ui->accountListBtn->show();
        ui->accountListBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_SELECTED);
        ui->billInfoBtn->show();
        ui->billInfoBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);

        ui->smartContractBtn->hide();
        ui->feedBtn->hide();
        ui->multiSigBtn->hide();

        ui->currentChoiceLabel->setText(tr("Account"));

        break;
    case 2:
        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn_unselected.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn_unselected.png"));

        ui->accountListBtn->hide();
        ui->billInfoBtn->hide();
        ui->smartContractBtn->hide();
        ui->feedBtn->hide();
        ui->multiSigBtn->hide();

        ui->currentChoiceLabel->setText(tr("Transfer"));

        break;
    case 3:
        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn_unselected.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn_unselected.png"));

        ui->accountListBtn->hide();
        ui->billInfoBtn->hide();
        ui->smartContractBtn->hide();
        ui->feedBtn->hide();
        ui->multiSigBtn->hide();

        ui->currentChoiceLabel->setText(tr("Contacts"));

        break;
    case 4:
        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn_unselected.png"));

        ui->accountListBtn->show();
        ui->accountListBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);
        ui->billInfoBtn->show();
        ui->billInfoBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_SELECTED);

        ui->smartContractBtn->hide();
        ui->feedBtn->hide();
        ui->multiSigBtn->hide();

        ui->currentChoiceLabel->setText(tr("Account"));

        break;
    case 5:

        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn_unselected.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn.png"));

        ui->accountListBtn->hide();
        ui->billInfoBtn->hide();

        ui->smartContractBtn->show();
        ui->smartContractBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_SELECTED);
        ui->feedBtn->show();
        ui->feedBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);
        ui->multiSigBtn->show();
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);

        ui->currentChoiceLabel->setText(tr("Advanced"));


        break;
    case 6:

        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn_unselected.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn.png"));

        ui->accountListBtn->hide();
        ui->billInfoBtn->hide();

        ui->smartContractBtn->show();
        ui->smartContractBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);
        ui->feedBtn->show();
        ui->feedBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_SELECTED);
        ui->multiSigBtn->show();
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);

        ui->currentChoiceLabel->setText(tr("Advanced"));


        break;
    case 7:

        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn_unselected.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn.png"));

        ui->accountListBtn->hide();
        ui->billInfoBtn->hide();

        ui->smartContractBtn->show();
        ui->smartContractBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);
        ui->feedBtn->show();
        ui->feedBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);
        ui->multiSigBtn->show();
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_SELECTED);

        ui->currentChoiceLabel->setText(tr("Advanced"));

        break;
    default:
        break;
    }
}

void FunctionBar::updateAssetComboBox()
{
    assetUpdating = true;

    ui->assetComboBox->clear();
    ui->assetComboBox->addItem(ASSET_NAME);
    foreach (QString key, UBChain::getInstance()->ERC20TokenInfoMap.keys())
    {
        ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(key);
        if(info.state == "COMMON" || info.state == "RETIRE")
        {
            ui->assetComboBox->addItem( info.contractName);
        }
    }

    if( UBChain::getInstance()->currentTokenAddress.isEmpty() || ui->assetComboBox->findText(UBChain::getInstance()->ERC20TokenInfoMap.value(UBChain::getInstance()->currentTokenAddress).contractName) == -1)
    {
        ui->assetComboBox->setCurrentText(ASSET_NAME);
        UBChain::getInstance()->currentTokenAddress = "";
    }
    else
    {
        ui->assetComboBox->setCurrentText(UBChain::getInstance()->ERC20TokenInfoMap.value(UBChain::getInstance()->currentTokenAddress).contractName);
    }

    assetUpdating = false;
}

void FunctionBar::updateTotalBalance()
{
    unsigned long long totalBalance = 0;
    int assetIndex = ui->assetComboBox->currentIndex();
    if( assetIndex == 0)
    {
        AssetInfo info = UBChain::getInstance()->assetInfoMap.value(assetIndex);
        foreach (QString key, UBChain::getInstance()->accountBalanceMap.keys())
        {
            AssetBalanceMap map = UBChain::getInstance()->accountBalanceMap.value(key);
            totalBalance += map.value(assetIndex);
        }

        QString balanceStr = getBigNumberString(totalBalance,info.precision);
        QString intStr;
        QString fractionStr;
        QString finalStr;
        finalStr += "<body><font style=\"font-size:17px\" color=#18faef>";
        if(balanceStr.contains("."))            // 空间允许的情况下
        {
            QStringList strList = balanceStr.split(".");
            intStr = strList.at(0);
            fractionStr = strList.at(1);

            finalStr += intStr;
            finalStr += "</font><font style=\"font-size:12px\" color=#18faef>";
            finalStr += ".";
            finalStr += fractionStr;
            finalStr += "</font><font style=\"font-size:12px\" color=#c0c4d4>";
            finalStr += " " + info.symbol;
            finalStr += " </font></body>";
        }
        else
        {
            intStr = balanceStr;

            finalStr += intStr;
            finalStr += "</font><font style=\"font-size:12px\" color=#c0c4d4>";
            finalStr += " " + info.symbol;
            finalStr += " </font></body>";
        }

        ui->totalBalanceLabel->setText(finalStr);
//        ui->totalBalanceLabel->setText( "<body><font style=\"font-size:17px\" color=#18faef>" + getBigNumberString(totalBalance,info.precision) + "</font><font style=\"font-size:9px\" color=#c0c4d4> " + info.contractName +"</font></body>" );
        ui->totalBalanceLabel->adjustSize();

        if(ui->totalBalanceLabel->width() > 140)
        {
            // 如果显示不下
            ui->totalBalanceLabel->setText( "<body><font style=\"font-size:12px\" color=#18faef>" + getBigNumberString(totalBalance,info.precision) + "</font><font style=\"font-size:12px\" color=#c0c4d4> " + info.symbol +"</font></body>" );
            ui->totalBalanceLabel->adjustSize();
            ui->totalBalanceLabel->move(60 + (160 - ui->totalBalanceLabel->width()) / 2, 95);

        }
        else
        {
            ui->totalBalanceLabel->move(60 + (160 - ui->totalBalanceLabel->width()) / 2, 90);
        }
    }
    else
    {
        // 如果是合约资产
        QStringList contracts = UBChain::getInstance()->ERC20TokenInfoMap.keys();
        QString contractAddress = contracts.at(assetIndex - 1);

        ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);

        QStringList keys = UBChain::getInstance()->addressMap.keys();
        foreach (QString key, keys)
        {
            QString accountAddress = UBChain::getInstance()->addressMap.value(key).ownerAddress;
            totalBalance += UBChain::getInstance()->accountContractBalanceMap.value(accountAddress).value(contractAddress);
        }

        QString balanceStr = getBigNumberString(totalBalance,info.precision);
        QString intStr;
        QString fractionStr;
        QString finalStr;
        finalStr += "<body><font style=\"font-size:17px\" color=#18faef>";
        if(balanceStr.contains("."))
        {
            QStringList strList = balanceStr.split(".");
            intStr = strList.at(0);
            fractionStr = strList.at(1);

            finalStr += intStr;
            finalStr += "</font><font style=\"font-size:12px\" color=#18faef>";
            finalStr += ".";
            finalStr += fractionStr;
            finalStr += "</font><font style=\"font-size:12px\" color=#c0c4d4>";
            finalStr += " " + info.contractName;
            finalStr += " </font></body>";
        }
        else
        {
            intStr = balanceStr;

            finalStr += intStr;
            finalStr += "</font><font style=\"font-size:12px\" color=#c0c4d4>";
            finalStr += " " + info.contractName;
            finalStr += " </font></body>";
        }

        ui->totalBalanceLabel->setText(finalStr);
        ui->totalBalanceLabel->adjustSize();

        if(ui->totalBalanceLabel->width() > 140)
        {
            // 如果显示不下
            ui->totalBalanceLabel->setText( "<body><font style=\"font-size:12px\" color=#18faef>" + getBigNumberString(totalBalance,info.precision) + "</font><font style=\"font-size:12px\" color=#c0c4d4> " + info.contractName +"</font></body>" );
            ui->totalBalanceLabel->adjustSize();
            ui->totalBalanceLabel->move(60 + (160 - ui->totalBalanceLabel->width()) / 2, 95);

        }
        else
        {
            ui->totalBalanceLabel->move(60 + (160 - ui->totalBalanceLabel->width()) / 2, 90);
        }

    }
}

void FunctionBar::retranslator()
{
    assetUpdating = true;
    ui->retranslateUi(this);

    ui->scanLabel->setToolTip(tr("If the balance or transaction record is incorrect, rescan the blockchain"));

    assetUpdating = false;
}

void FunctionBar::setCurrentAccount(QString accountName)
{
    if(accountName.isEmpty())
    {
        ui->accountLabel->hide();
        ui->balanceLabel->hide();
    }
    else
    {
        ui->accountLabel->show();
        ui->balanceLabel->show();

        ui->accountLabel->setText(accountName);

        if( UBChain::getInstance()->currentTokenAddress.isEmpty())
        {
            if( !UBChain::getInstance()->accountBalanceMap.contains(accountName))
            {
                ui->balanceLabel->setText(QString("0 ") + ASSET_NAME );
                return;
            }

            AssetBalanceMap assetBalanceMap = UBChain::getInstance()->accountBalanceMap.value(accountName);
            int assetId = 0;
            unsigned long long amount = assetBalanceMap.value(assetId);
            AssetInfo info = UBChain::getInstance()->assetInfoMap.value(assetId);
            ui->balanceLabel->setText( getBigNumberString(amount,info.precision) + " " + ASSET_NAME);
        }
        else
        {
            // 如果是合约资产
            QString contractAddress = UBChain::getInstance()->currentTokenAddress;

            ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress);

            QString accountAddress = UBChain::getInstance()->addressMap.value(accountName).ownerAddress;
            unsigned long long amount = UBChain::getInstance()->accountContractBalanceMap.value(accountAddress).value(contractAddress);
            ui->balanceLabel->setText( getBigNumberString(amount,info.precision) + " " + info.contractName );

        }
    }
}

void FunctionBar::showScanPic()
{
    scanLabelCount++;
    if(scanLabelCount > 40)
    {
        scanLabelCount = 0;
        timerForScanLabel->stop();
    }

    QMatrix matrix;
    matrix.rotate(scanLabelCount * 9);

    ui->scanLabel->setPixmap(QPixmap(":/ui/wallet_ui/scan.png").transformed(matrix,Qt::SmoothTransformation));

}

void FunctionBar::refresh()
{
    updateAssetComboBox();
    updateTotalBalance();
    setCurrentAccount(UBChain::getInstance()->currentAccount);
}

void FunctionBar::on_advanceBtn_clicked()
{
    if(currentPageIndex == 5 || currentPageIndex == 6 ||currentPageIndex == 7)      return;

    on_smartContractBtn_clicked();
}

void FunctionBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(16,17,24),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(16,17,24),Qt::SolidPattern));
    painter.drawRect(0,0,60,556);

    painter.setPen(QPen(QColor(24,28,45),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(24,28,45),Qt::SolidPattern));
    painter.drawRect(61,0,160,556);
}

bool FunctionBar::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->scanLabel)
    {
        if( e->type() == QEvent::MouseButtonPress)
        {
            if(scanLabelCount == 0)
            {
                timerForScanLabel->start(25);
                UBChain::getInstance()->postRPC( "id_scan", toJsonFormat( "scan", QStringList() << "0"));
            }

            return true;
        }
    }

    return QWidget::eventFilter(watched,e);
}


void FunctionBar::on_accountListBtn_clicked()
{
    choosePage(1);
    emit showMainPage();
}

void FunctionBar::on_billInfoBtn_clicked()
{
    choosePage(4);
    emit showAccountPage();
}

void FunctionBar::on_assetComboBox_currentIndexChanged(int index)
{
    if( assetUpdating)  return;

    if( index == 0)
    {
        UBChain::getInstance()->currentTokenAddress = "";
    }
    else
    {
        UBChain::getInstance()->currentTokenAddress = UBChain::getInstance()->ERC20TokenInfoMap.keys().at(index - 1);
    }

    updateTotalBalance();
    setCurrentAccount(UBChain::getInstance()->currentAccount);

    emit assetChanged(index);
}

void FunctionBar::on_choiceBtn_clicked()
{
    FunctionBarChoiceDialog functionBarChoiceDialog(this);
    functionBarChoiceDialog.move( ui->choiceBtn->mapToGlobal(QPoint(-32,-10 - functionBarChoiceDialog.height())));
    functionBarChoiceDialog.exec();

    switch (functionBarChoiceDialog.choice)
    {
    case 1:
        emit lock();
        break;
    case 2:
    {
        ConsoleDialog consoleDialog;
        consoleDialog.pop();
    }
        break;
    case 3:
    {
        SetDialog setDialog;
        connect(&setDialog,SIGNAL(settingSaved()),UBChain::getInstance()->mainFrame,SLOT(settingSaved()));
        setDialog.pop();
    }
        break;
    default:
        break;
    }
}
