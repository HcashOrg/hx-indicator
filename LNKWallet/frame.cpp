
#include <QLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
#include <QMenu>
#include <QApplication>
#include <QStyleOption>

#include "firstlogin.h"
#include "normallogin.h"
#include "mainpage.h"
#include "transfer/transferpage.h"
#include "bottombar.h"
#include "lockpage.h"
#include "titlebar.h"
#include "frame.h"
#include "wallet.h"
#include "miner/minerpage.h"
#include "depositpage/DepositAutomatic.h"

#include "waitingforsync.h"
#include <QDesktopWidget>

#include "functionBar/FunctionWidget.h"
#include "contact/ContactWidget.h"
#include "poundage/PoundageWidget.h"
#include "selectwalletpathwidget.h"
#include "control/shadowwidget.h"
#include "commondialog.h"
#include "multisig/multisigpage.h"
#include "multisig/multisigtransactionpage.h"
#include "neworimportwalletwidget.h"
#include "exchange/OnchainOrderPage.h"
#include "exchange/myexchangecontractpage.h"
#include "extra/transactiondb.h"
#include "crossmark/crosscapitalmark.h"
#include "control/CustomShadowEffect.h"
#include "guard/GuardAccountPage.h"
#include "guard/GuardIncomePage.h"
#include "guard/AssetPage.h"
#include "guard/GuardKeyManagePage.h"
#include "guard/ProposalPage.h"
#include "guard/WithdrawConfirmPage.h"
#include "guard/FeedPricePage.h"
#include "guard/ColdHotTransferPage.h"
#include "bonus/BonusPage.h"
#include "dapp/ContractTokenPage.h"

Frame::Frame(): timer(NULL),
    firstLogin(NULL),
    normalLogin(NULL),
    mainPage(NULL),
    transferPage(NULL),
    bottomBar(NULL),
    centralWidget(NULL),
    lockPage(NULL),
    titleBar(NULL),
    contactPage(NULL),
    functionBar(NULL),
    shadowWidget(NULL),
    timerForAutoRefresh(NULL),
    waitingForSync(NULL),
    selectWalletPathWidget(NULL),
    newOrImportWalletWidget(NULL),
    multiSigPage(NULL),
    multiSigTransactionPage(NULL),
    minerPage(NULL),
    bonusPage(NULL),
    guardAccountPage(NULL),
    guardIncomePage(NULL),
    assetPage(NULL),
    guardKeyManagePage(NULL),
    proposalPage(NULL),
    withdrawConfirmPage(NULL),
    feedPricePage(NULL),
    coldHotTransferPage(NULL),
    contractTokenPage(NULL),
    poundage(nullptr)
{

#ifdef TARGET_OS_MAC
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint |Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);    // mac下设为window popup属性的对话框会有问题
#else
    setWindowFlags(Qt::Window|Qt::FramelessWindowHint |Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);
#endif

#ifdef WIN32
    ::SystemParametersInfo(SPI_GETWORKAREA , 0 , &this->rtConfined , 0);
    ::GetWindowRect(::GetDesktopWindow() , &this->rtDefault);
#endif

    setAttribute(Qt::WA_TranslucentBackground, true );
    setFrameShape(QFrame::NoFrame);
//    setMouseTracking(true);

//    setStyleSheet("Frame{background-color:rgba(0, 0, 0,255);}"
//                  );

//    setAttribute(Qt::WA_TranslucentBackground,true);
//    _pixmap.load(":/ui/wallet_ui/loginBg.png");

//    QPalette pal = palette();
//    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
//    setPalette(pal);


    mouse_press = false;
    currentPageNum = 0;
    currentAccount = "";


    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    connect(HXChain::getInstance(),&HXChain::showBubbleSignal,this,&Frame::ShowBubbleMessage);
    QString language = HXChain::getInstance()->language;
    if( language.isEmpty())
    {
//        setLanguage("Simplified Chinese");
        setLanguage("English");
    }
    else
    {
        setLanguage(language);
    }

    setGeometry(0,0, 960, 543);
    moveWidgetToScreenCenter(this);

    shadowWidget = new ShadowWidget(this);
    shadowWidget->hide();
    shadowWidget->init(this->size());

    //  如果是第一次使用(未设置 blockchain 路径)
    mutexForConfigFile.lock();

    if( !HXChain::getInstance()->configFile->contains("/settings/chainPath") )
    {
        selectWalletPathWidget = new SelectWalletPathWidget(this);
        selectWalletPathWidget->setAttribute(Qt::WA_DeleteOnClose);
        selectWalletPathWidget->move(0,0);
        connect( selectWalletPathWidget,SIGNAL(enter()),this,SLOT(showWaittingForSyncWidget()));
        connect( selectWalletPathWidget,SIGNAL(newOrImportWallet()),this,SLOT(showNewOrImportWalletWidget()));
        connect( selectWalletPathWidget,SIGNAL(closeWallet()),qApp,SLOT(quit()));


        selectWalletPathWidget->show();
        setGeometry(0,0, selectWalletPathWidget->width(), selectWalletPathWidget->height());
        moveWidgetToScreenCenter(this);
    }
    else
    {
        QString path = HXChain::getInstance()->configFile->value("/settings/chainPath").toString();
        QDir dir(path);
        QFileInfo fileInfo(path + "/wallet.json");

        if( !dir.exists() || !fileInfo.exists() )  // 如果数据文件被删除了
        {
            selectWalletPathWidget = new SelectWalletPathWidget(this);
            selectWalletPathWidget->setAttribute(Qt::WA_DeleteOnClose);
            selectWalletPathWidget->move(0,0);
            connect( selectWalletPathWidget,SIGNAL(enter()),this,SLOT(showWaittingForSyncWidget()));
            connect( selectWalletPathWidget,SIGNAL(newOrImportWallet()),this,SLOT(showNewOrImportWalletWidget()));
            connect( selectWalletPathWidget,SIGNAL(closeWallet()),qApp,SLOT(quit()));


            selectWalletPathWidget->show();
            setGeometry(0,0, selectWalletPathWidget->width(), selectWalletPathWidget->height());
            moveWidgetToScreenCenter(this);

            HXChain::getInstance()->configFile->clear();
            HXChain::getInstance()->configFile->setValue("/settings/lockMinutes",5);
            HXChain::getInstance()->lockMinutes     = 5;
            HXChain::getInstance()->configFile->setValue("/settings/notAutoLock",false);
            HXChain::getInstance()->notProduce      =  true;
            HXChain::getInstance()->configFile->setValue("/settings/language","English");
            HXChain::getInstance()->language = "English";
            HXChain::getInstance()->configFile->setValue("/settings/feeType","HX");
            HXChain::getInstance()->feeType = "HX";

            HXChain::getInstance()->configFile->setValue("/settings/backupNeeded",false);
            HXChain::getInstance()->IsBackupNeeded = false;

            HXChain::getInstance()->configFile->setValue("/settings/autoDeposit",false);
            HXChain::getInstance()->autoDeposit = false;
            HXChain::getInstance()->configFile->setValue("/settings/contractFee",1);
            HXChain::getInstance()->contractFee = 1;

            HXChain::getInstance()->minimizeToTray  = false;
            HXChain::getInstance()->configFile->setValue("/settings/minimizeToTray",false);
            HXChain::getInstance()->closeToMinimize = false;
            HXChain::getInstance()->configFile->setValue("/settings/closeToMinimize",false);

        }
        else
        {
            if(fileInfo.size() == 0)
            {
                QFile autoSaveFile(path + "/wallet.json.autobak");
                if(autoSaveFile.exists())
                {
                    QFile oldFile(path + "/wallet.json");
                    qDebug() << "remove corrupted wallet.json : " << oldFile.remove();
                    qDebug() << "recover auto backup wallet.json : " << autoSaveFile.copy(path + "/wallet.json");
                }
            }


            HXChain::getInstance()->startExe();

            waitingForSync = new WaitingForSync(this);
            waitingForSync->setAttribute(Qt::WA_DeleteOnClose);
            waitingForSync->move(0,0);
            connect( waitingForSync,SIGNAL(sync()), this, SLOT(syncFinished()));
            connect( waitingForSync,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( waitingForSync,SIGNAL(tray()),this,SLOT(hide()));
            connect( waitingForSync,SIGNAL(closeWallet()),qApp,SLOT(quit()));
            waitingForSync->show();

            setGeometry(0,0, waitingForSync->width(), waitingForSync->height());
            moveWidgetToScreenCenter(this);
        }


    }
    mutexForConfigFile.unlock();

    trayIcon = new QSystemTrayIcon(this);
    //放在托盘提示信息、托盘图标
    trayIcon ->setToolTip(QString("HXIndicator ") + WALLET_VERSION);
    trayIcon ->setIcon(QIcon(":/ui/wallet_ui/HX.ico"));
    //点击托盘执行的事件
    connect(trayIcon , SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
    createTrayIconActions();
    createTrayIcon();

    //朱正天--自动转账
    autoDeposit = new DepositAutomatic(this);
    //自动记录链外资金划转
    crossMark = new CrossCapitalMark(this);
}

Frame::~Frame()
{

    delete crossMark;
    crossMark = nullptr;
    qDebug() << "~Frame begin;";

    if(centralWidget)
    {
        delete centralWidget;
        centralWidget = NULL;
    }

	if (titleBar)
	{
		delete titleBar;
		titleBar = NULL;
	}

    if (timer)
	{
		delete timer;
		timer = NULL;
	}

	if (bottomBar)
	{
		delete bottomBar;
		bottomBar = NULL;
	}

    if (lockPage)
    {
        delete lockPage;
        lockPage = NULL;
    }

    if( waitingForSync)
    {
        delete waitingForSync;
        waitingForSync = NULL;
    }

    if( functionBar)
    {
        delete functionBar;
        functionBar = NULL;
    }


qDebug() << "~Frame end;";

}

void Frame::alreadyLogin()
{
    setGeometry(0,0, 960, 543);
    moveWidgetToScreenCenter(this);

    titleBar = new TitleBar(this);
    titleBar->resize(770,33);
    titleBar->move(190,0);
    titleBar->show();

    connect(titleBar,SIGNAL(minimum()),this,SLOT(showMinimized()));
    connect(titleBar,SIGNAL(closeWallet()),this,SLOT(onCloseWallet()));
    connect(titleBar,SIGNAL(tray()),this,SLOT(hide()));
    connect(titleBar,&TitleBar::back,this,&Frame::onBack);
    connect(this,&Frame::titleBackVisible,titleBar,&TitleBar::backBtnVis);

    centralWidget = new QWidget(this);
    centralWidget->resize(770,510);
    centralWidget->move(190, titleBar->height());
    centralWidget->show();

    bottomBar = new BottomBar(this);
    bottomBar->resize(160,40);
    bottomBar->move(800,503);
    bottomBar->raise();
    bottomBar->show();

//    showBottomBarWidget = new ShowBottomBarWidget(centralWidget);
//    showBottomBarWidget->setGeometry(0,525,827,20);
//    connect(showBottomBarWidget, SIGNAL(showBottomBar()), bottomBar, SLOT(dynamicShow()) );
//    showBottomBarWidget->show();

    functionBar = new FunctionWidget(this);
    functionBar->resize(190,480);
    functionBar->move(0,63);
    functionBar->show();
    connect(functionBar,SIGNAL(lock()),this,SLOT(showLockPage()));

    topLeftWidget = new QWidget(this);
    topLeftWidget->setGeometry(0,0,190,63);
    topLeftWidget->setStyleSheet("background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(82,61,137), stop:1 rgba(84,64,144));");
    QLabel* logoLabel = new QLabel(topLeftWidget);
    logoLabel->setGeometry(50,15,92,35);
    logoLabel->setPixmap(QPixmap(":/ui/wallet_ui/HX_logo.png"));
    topLeftWidget->show();
    topLeftWidget->raise();

    connect(functionBar,&FunctionWidget::showMinerSignal,this,&Frame::showMinerPage);
    connect(functionBar,&FunctionWidget::showBonusSignal,this,&Frame::showBonusPage);
    connect(functionBar,&FunctionWidget::showAccountSignal,this,&Frame::showMainPage);
    connect(functionBar,&FunctionWidget::showContactSignal,this,&Frame::showContactPage);
    connect(functionBar,&FunctionWidget::showAdvanceSignal,this,&Frame::showMainPage);
    connect(functionBar,&FunctionWidget::showExchangeSignal,this,&Frame::showMainPage);
    connect(functionBar,&FunctionWidget::showMultiSigSignal,this,&Frame::showMultiSigPage);
    connect(functionBar,&FunctionWidget::showPoundageSignal,this,&Frame::showPoundagePage);
    connect(functionBar,&FunctionWidget::ShrinkSignal,this,&Frame::EnlargeRightPart);
    connect(functionBar,&FunctionWidget::RestoreSignal,this,&Frame::RestoreRightPart);
    connect(functionBar,&FunctionWidget::showOnchainOrderSignal,this,&Frame::showOnchainOrderPage);
    connect(functionBar,&FunctionWidget::showMyOrderSignal,this,&Frame::showMyExchangeContractPage);
    connect(functionBar,&FunctionWidget::showGuardAccountSignal,this,&Frame::showGuardAccountPage);
    connect(functionBar,&FunctionWidget::showGuardIncomeSignal,this,&Frame::showGuardIncomePage);
    connect(functionBar,&FunctionWidget::showAssetSignal,this,&Frame::showAssetPage);
    connect(functionBar,&FunctionWidget::showKeyManageSignal,this,&Frame::showKeyManagePage);
    connect(functionBar,&FunctionWidget::showProposalSignal,this,&Frame::showProposalPage);
    connect(functionBar,&FunctionWidget::showWithdrawConfirmSignal,this,&Frame::showWithdrawConfirmPage);
    connect(functionBar,&FunctionWidget::showFeedPriceSignal,this,&Frame::showFeedPricePage);
    connect(functionBar,&FunctionWidget::showColdHotTransferSignal,this,&Frame::showColdHotTransferPage);
    connect(functionBar,&FunctionWidget::showContractTokenSignal,this,&Frame::showContractTokenPage);
    getAccountInfo();

    mainPage = new MainPage(centralWidget);
    mainPage->resize(770,510);
    mainPage->setAttribute(Qt::WA_DeleteOnClose);
//    QTimer::singleShot(1000,mainPage,SLOT(show()));
    mainPage->show();
    currentPageNum = 0;
    connect(mainPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(mainPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(mainPage,SIGNAL(showTransferPage(QString,QString)),this,SLOT(showTransferPage(QString,QString)));
    connect(mainPage,SIGNAL(newAccount(QString)),this,SLOT(newAccount(QString)));
    connect(mainPage,&MainPage::backBtnVisible,titleBar,&TitleBar::backBtnVis);

    timer = new QTimer(this);               //  自动锁定
    connect(timer,SIGNAL(timeout()),this,SLOT(autoLock()));
    if(HXChain::getInstance()->notProduce)
    {
        timer->start(HXChain::getInstance()->lockMinutes * 60000);
    }

    startTimerForAutoRefresh();              // 自动刷新

    init();
    functionBar->DefaultShow();
}


QString toThousandFigure( int number)     // 转换为0001,0015  这种数字格式
{
    if( number <= 9999 && number >= 1000)
    {
        return QString::number(number);
    }

    if( number <= 999 && number >= 100)
    {
        return QString( "0" + QString::number(number));
    }

    if( number <= 99 && number >= 10)
    {
        return QString( "00" + QString::number(number));
    }

    if( number <= 9 && number >= 0)
    {
        return QString( "000" + QString::number(number));
    }
    return "99999";
}

void Frame::getAccountInfo()
{
    HXChain::getInstance()->postRPC( "id-list_my_accounts", toJsonFormat( "list_my_accounts", QJsonArray()));

    HXChain::getInstance()->postRPC( "id-list_assets", toJsonFormat( "list_assets", QJsonArray() << "A" << "100"));

    HXChain::getInstance()->fetchTransactions();

    HXChain::getInstance()->fetchFormalGuards();
}


void Frame::showMinerPage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    minerPage = new MinerPage(centralWidget);
    minerPage->setAttribute(Qt::WA_DeleteOnClose);
    minerPage->show();

    currentPageNum = 7;
}

void Frame::showBonusPage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    bonusPage = new BonusPage(centralWidget);
    bonusPage->setAttribute(Qt::WA_DeleteOnClose);
    bonusPage->show();

    currentPageNum = 19;
}

void Frame::showTransferPage(QString accountName,QString assetType)
{
    closeCurrentPage();
    getAccountInfo();
    HXChain::getInstance()->mainFrame->setCurrentAccount(accountName);
    qDebug()<<"transaccccccc";
    transferPage = new TransferPage(accountName,centralWidget,assetType);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
//    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    transferPage->show();

    currentPageNum = 3;
    //朱正天functionBar->choosePage(2);


}


void Frame::showLockPage()
{

    timer->stop();
    shadowWidgetShow();


    HXChain::getInstance()->postRPC( "id-lock", toJsonFormat( "lock", QJsonArray()));
qDebug() << "lock ";

}

void Frame::autoLock()
{

    timer->stop();

    HXChain::getInstance()->postRPC( "id-lock", toJsonFormat( "lock", QJsonArray() ));
qDebug() << "autolock ";

}

void Frame::unlock()
{
    setGeometry(0,0, 960, 543);
    moveWidgetToScreenCenter(this);

    if( HXChain::getInstance()->notProduce)
    {
        timer->start(HXChain::getInstance()->lockMinutes * 60000);
    }
    centralWidget->show();
    bottomBar->show();
    titleBar->show();
    qDebug() << "lockPage " << lockPage;
    if( lockPage)
    {
        lockPage->close();
        lockPage = NULL;
    }

qDebug() << "unlock showCurrentDialog";
    HXChain::getInstance()->showCurrentDialog();


}

void Frame::updateTimer()
{
    if( timer != NULL && lockPage == NULL)
    {
        if( HXChain::getInstance()->notProduce)
        {
            timer->stop();
            timer->start(HXChain::getInstance()->lockMinutes * 60000);
        }
    }
}

void Frame::settingSaved()
{

    if( !HXChain::getInstance()->notProduce)
    {
        timer->stop();
    }
    else
    {
        timer->start( HXChain::getInstance()->lockMinutes * 60000);
    }

    QString language = HXChain::getInstance()->language;
    if( language.isEmpty())
    {
        setLanguage("English");
    }
    else
    {
        setLanguage(language);
    }


}

void Frame::privateKeyImported()
{

    getAccountInfo();
    mainPage->updateAccountList();

}

void Frame::mousePressEvent(QMouseEvent *event)
{
//    if( HXChain::getInstance()->notProduce)
//    {
//        updateTimer();
//    }

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }
#ifdef WIN32
    ::ClipCursor(&rtConfined);
#endif
}

void Frame::mouseMoveEvent(QMouseEvent *event)
{

    //若鼠标左键被按下
    if(mouse_press)
    {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

        //移动主窗体位置
        this->move(move_pos - move_point);
    }

}

void Frame::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
#ifdef WIN32
    ::ClipCursor(&rtDefault);
#endif
}

void Frame::startTimerForAutoRefresh()
{
    if( timerForAutoRefresh != NULL)
    {
        timerForAutoRefresh->stop();
        delete timerForAutoRefresh;
    }

    timerForAutoRefresh = new QTimer(this);
    connect(timerForAutoRefresh,SIGNAL(timeout()),this,SLOT(autoRefresh()));
    timerForAutoRefresh->start(AUTO_REFRESH_TIME);
}

void Frame::syncFinished()
{
    qDebug() << "sync finished ";
    waitingForSync->timerForWSConnected->stop();

//    HXChain::getInstance()->initWorkerThreadManager();

    if( HXChain::getInstance()->contactsFile == NULL)
    {
        HXChain::getInstance()->getContactsFile();
    }

    HXChain::getInstance()->postRPC("id-is_new", toJsonFormat( "is_new", QJsonArray()));
}

void Frame::setCurrentAccount(QString accountName)
{
    HXChain::getInstance()->currentAccount = accountName;
}

void Frame::ShowMainPageSlot()
{
    functionBar->contactShowTransferPageSlots();
    //showMainPage();
}


void Frame::closeCurrentPage()
{
    switch (currentPageNum) {
    case 0:
        mainPage->close();
        mainPage = NULL;
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        transferPage->close();
        transferPage = NULL;
        break;
    case 4:
        if(contactPage)
        {
            contactPage->close();
            contactPage = NULL;
        }
        break;
    case 5:
        myExchangeContractPage->close();
        myExchangeContractPage = NULL;
        break;
    case 6:
        onchainOrderPage->close();
        onchainOrderPage = NULL;
        break;
    case 7:
        minerPage->close();
        minerPage = NULL;
        break;
    case 8:
        assetPage->close();
        assetPage = NULL;
        break;
    case 9:
        multiSigPage->close();
        multiSigPage = NULL;
        break;
    case 10:
        multiSigTransactionPage->close();
        multiSigTransactionPage = NULL;
        break;
    case 11:
        if(poundage)
        {
            poundage->close();
            poundage = NULL;
        }
        break;
    case 12:
        if(guardKeyManagePage)
        {
            guardKeyManagePage->close();
            guardKeyManagePage = NULL;
        }
        break;
    case 13:
        if(proposalPage)
        {
            proposalPage->close();
            proposalPage = NULL;
        }
        break;
    case 14:
        if(withdrawConfirmPage)
        {
            withdrawConfirmPage->close();
            withdrawConfirmPage = NULL;
        }
        break;
    case 15:
        if(feedPricePage)
        {
            feedPricePage->close();
            feedPricePage = NULL;
        }
        break;
    case 16:
        if(coldHotTransferPage)
        {
            coldHotTransferPage->close();
            coldHotTransferPage = NULL;
        }
        break;
    case 17:
        if(guardAccountPage)
        {
            guardAccountPage->close();
            guardAccountPage = NULL;
        }
        break;
    case 18:
        if(guardIncomePage)
        {
            guardIncomePage->close();
            guardIncomePage = NULL;
        }
        break;
    case 19:
        if(bonusPage)
        {
            bonusPage->close();
            bonusPage = NULL;
        }
        break;
    case 20:
        if(contractTokenPage)
        {
            contractTokenPage->close();
            contractTokenPage = NULL;
        }
        break;
    default:
        break;
    }


}


void Frame::autoRefresh()
{
    getAccountInfo();

    bottomBar->refresh();
   //朱正天 functionBar->refresh();

    switch (currentPageNum) {
    case 0:
        mainPage->refresh();
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
//        showTransferPage( transferPage->getCurrentAccount());
        transferPage->refresh();
        break;
    case 4:
        break;
    case 5:
        myExchangeContractPage->refresh();
        break;
    case 6:
        break;
    case 7:
        minerPage->refresh();
        break;
    case 8:
        break;
    case 9:
        multiSigPage->refresh();
        break;
    case 10:
        multiSigTransactionPage->refresh();
    case 12:
        guardKeyManagePage->refresh();
        break;
    case 13:
        proposalPage->refresh();
        break;
    case 14:
        withdrawConfirmPage->refresh();
        break;
    case 15:
        feedPricePage->refresh();
        break;
    case 16:
        coldHotTransferPage->refresh();
        break;
    case 19:
        bonusPage->refresh();
        break;
    case 20:
        contractTokenPage->refresh();
        break;
    default:
        break;
    }
}


void Frame::showMainPage()
{
    emit titleBackVisible(false);
    closeCurrentPage();
    getAccountInfo();

    mainPage = new MainPage(centralWidget);
    mainPage->setAttribute(Qt::WA_DeleteOnClose);
    mainPage->show();
    currentPageNum = 0;
    connect(mainPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(mainPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(mainPage,SIGNAL(showTransferPage(QString,QString)),this,SLOT(showTransferPage(QString,QString)));
    connect(mainPage,SIGNAL(newAccount(QString)),this,SLOT(newAccount(QString)));
    connect(mainPage,&MainPage::backBtnVisible,titleBar,&TitleBar::backBtnVis);
}


void Frame::showTransferPage()
{
    closeCurrentPage();
    getAccountInfo();
    qDebug()<<"transempty";
    transferPage = new TransferPage(HXChain::getInstance()->currentAccount,centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
//    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    transferPage->show();
    currentPageNum = 3;
   //朱正天 functionBar->choosePage(2);
}


void Frame::showContactPage()
{
    //EnlargeRightPart();
    emit titleBackVisible(false);
    closeCurrentPage();

    contactPage = new ContactWidget(centralWidget);

    connect(contactPage,&ContactWidget::backBtnVisible,titleBar,&TitleBar::backBtnVis);
    connect(contactPage,SIGNAL(gotoTransferPage(QString,QString)),functionBar,SLOT(contactShowTransferPageSlots()));
    connect(contactPage,SIGNAL(gotoTransferPage(QString,QString)),this,SLOT(showTransferPageWithAddress(QString,QString)));//address name
    //contactPage->setAttribute(Qt::WA_DeleteOnClose);
    contactPage->show();
    currentPageNum = 4;
    EnlargeRightPart();
}

bool Frame::eventFilter(QObject* watched, QEvent* e)
{
    if( (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::KeyPress)  )
    {
        updateTimer();
    }

    // currentDialog 上的鼠标事件也会移动 frame 和 本身
    if( HXChain::getInstance()->currentDialogVector.contains(  (QWidget*)watched) )
    {
        if( e->type() == QEvent::MouseMove)
        {
            mouseMoveEvent( (QMouseEvent*)e);
        }
        else if( e->type() == QEvent::MouseButtonPress)
        {
            mousePressEvent( (QMouseEvent*)e);
        }
        else if( e->type() == QEvent::MouseButtonRelease)
        {
            mouseReleaseEvent( (QMouseEvent*)e);
        }

        HXChain::getInstance()->resetPosOfCurrentDialog();  // currentDialog 一起移动

        return false;
    }

    return false;
}

void Frame::shadowWidgetShow()
{
    qDebug() << "shadowWidgetShow";
    shadowWidget->raise();
    shadowWidget->show();
}

void Frame::shadowWidgetHide()
{
    qDebug() << "shadowWidgetHide";
    shadowWidget->hide();
}

void Frame::showTransferPageWithAddress(QString address, QString name)
{
    qDebug()<<"transssss"<<HXChain::getInstance()->currentAccount;
    emit titleBackVisible(true);
    closeCurrentPage();
    getAccountInfo();
    transferPage = new TransferPage(HXChain::getInstance()->currentAccount,centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
    transferPage->setAddress(address);
//    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    transferPage->show();
    currentPageNum = 3;
   //朱正天 functionBar->choosePage(2);
}

void Frame::setLanguage(QString language)
{


    menuTranslator.load(QString(":/qm/qt_zh_cn"));
    translatorForTextBrowser.load(":/language/widgets.qm");

    if( language == "Simplified Chinese")
    {
        translator.load(":/language/wallet_simplified_Chinese.qm");
        QApplication::installTranslator(&menuTranslator);
        QApplication::installTranslator(&translatorForTextBrowser);
    }
    else if( language == "English")
    {
        translator.load(":/language/wallet_English.qm");
        QApplication::removeTranslator(&menuTranslator);
        QApplication::removeTranslator(&translatorForTextBrowser);
    }

    QApplication::installTranslator(&translator);


    if( titleBar != NULL)       // 已经登录
    {
        functionBar->retranslator();
        titleBar->retranslator();
        bottomBar->retranslator();
        shadowWidget->retranslator();

        switch (currentPageNum) {
        case 0:
            showMainPage();
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
//            transferPage->retranslator(language);
            //showTransferPage(currentAccount,"");
            break;
        case 4:
//            contactPage->retranslator(language);
            showContactPage();
            break;
        case 5:
            showMyExchangeContractPage();
            break;
        case 6:
            showOnchainOrderPage();
            break;
        case 7:
            showMinerPage();
            break;
        case 8:
            showAssetPage();
            break;
        case 9:
            showMultiSigPage();
            break;
        case 10:
            if(multiSigTransactionPage != NULL)
            {
                showMultiSigTransactionPage(multiSigTransactionPage->multiSigAddress);
            }
            break;
        case 11:
            showPoundagePage();
            break;
        case 12:
            showKeyManagePage();
            break;
        case 13:
            showProposalPage();
            break;
        case 14:
            showWithdrawConfirmPage();
            break;
        case 15:
            showFeedPricePage();
            break;
        case 16:
            showColdHotTransferPage();
            break;
        case 17:
            showGuardAccountPage();
            break;
        case 18:
            showGuardIncomePage();
            break;
        case 19:
            showBonusPage();
            break;
        default:
            break;
        }
    }

    
}

void Frame::showWaittingForSyncWidget()
{
    if( selectWalletPathWidget)
    {
        selectWalletPathWidget->close();
        selectWalletPathWidget = NULL;
    }

    if( newOrImportWalletWidget)
    {
        newOrImportWalletWidget->close();
        newOrImportWalletWidget = NULL;
    }

    waitingForSync = new WaitingForSync(this);
    waitingForSync->setAttribute(Qt::WA_DeleteOnClose);
    waitingForSync->move(0,0);
    connect( waitingForSync,SIGNAL(sync()), this, SLOT(syncFinished()));
    connect( waitingForSync,SIGNAL(minimum()),this,SLOT(showMinimized()));
    connect( waitingForSync,SIGNAL(tray()),this,SLOT(hide()));
    connect( waitingForSync,SIGNAL(closeWallet()),qApp,SLOT(quit()));

    waitingForSync->show();

//    setGeometry(0,0, waitingForSync->width(), waitingForSync->height());
//    moveWidgetToScreenCenter(this);
}

void Frame::showNewOrImportWalletWidget()
{
    if( selectWalletPathWidget)
    {
        selectWalletPathWidget->close();
        selectWalletPathWidget = NULL;
    }

    newOrImportWalletWidget = new NewOrImportWalletWidget(this);
    newOrImportWalletWidget->setAttribute(Qt::WA_DeleteOnClose);
    newOrImportWalletWidget->move(0,0);
    connect( newOrImportWalletWidget,SIGNAL(enter()),this,SLOT(showWaittingForSyncWidget()));
    connect( newOrImportWalletWidget,SIGNAL(closeWallet()),qApp,SLOT(quit()));

    newOrImportWalletWidget->show();
    setGeometry(0,0, newOrImportWalletWidget->width(), newOrImportWalletWidget->height());
    moveWidgetToScreenCenter(this);
}

void Frame::showMultiSigPage()
{
    emit titleBackVisible(false);
    closeCurrentPage();
    multiSigPage = new MultiSigPage(centralWidget);
    connect(multiSigPage,SIGNAL(goToTransferPage(QString,QString)),this,SLOT(showTransferPageWithAddress(QString,QString)));
    connect(multiSigPage,SIGNAL(showMultiSigTransactionPage(QString)),this,SLOT(showMultiSigTransactionPage(QString)));
    multiSigPage->setAttribute(Qt::WA_DeleteOnClose);
    multiSigPage->show();
    currentPageNum = 9;
    //朱正天 functionBar->choosePage(7);
}

void Frame::showPoundagePage()
{
    emit titleBackVisible(false);
    closeCurrentPage();
    poundage = new PoundageWidget(centralWidget);
    connect(poundage,&PoundageWidget::backBtnVisible,titleBar,&TitleBar::backBtnVis);
    poundage->resize(centralWidget->size());
    poundage->setAttribute(Qt::WA_DeleteOnClose);
    poundage->show();
    currentPageNum = 11;
}

void Frame::showOnchainOrderPage()
{
    emit titleBackVisible(false);
    closeCurrentPage();
    onchainOrderPage = new OnchainOrderPage(centralWidget);
    connect(onchainOrderPage,&OnchainOrderPage::backBtnVisible,titleBar,&TitleBar::backBtnVis);
    onchainOrderPage->setAttribute(Qt::WA_DeleteOnClose);
    onchainOrderPage->show();
    currentPageNum = 6;
}

void Frame::showMyExchangeContractPage()
{
    emit titleBackVisible(false);
    if(HXChain::getInstance()->accountInfoMap.isEmpty())
    {
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(tr("Please Import Or Create Account First!"));
        dia.pop();
        HXChain::getInstance()->mainFrame->ShowMainPageSlot();
        return;
    }

    closeCurrentPage();
    myExchangeContractPage = new MyExchangeContractPage(centralWidget);
    connect(myExchangeContractPage,&MyExchangeContractPage::backBtnVisible,titleBar,&TitleBar::backBtnVis);
    myExchangeContractPage->setAttribute(Qt::WA_DeleteOnClose);
    myExchangeContractPage->show();
    currentPageNum = 5;

    //RestoreRightPart();不用自己调用，functionbar已连接该槽
}

void Frame::showGuardAccountPage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    guardAccountPage = new GuardAccountPage(centralWidget);
    guardAccountPage->setAttribute(Qt::WA_DeleteOnClose);
    guardAccountPage->show();
    currentPageNum = 17;
}

void Frame::showGuardIncomePage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    guardIncomePage = new GuardIncomePage(centralWidget);
    guardIncomePage->setAttribute(Qt::WA_DeleteOnClose);
    guardIncomePage->show();
    currentPageNum = 18;
}

void Frame::showAssetPage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    assetPage = new AssetPage(centralWidget);
    assetPage->setAttribute(Qt::WA_DeleteOnClose);
    assetPage->show();
    currentPageNum = 8;
}

void Frame::showKeyManagePage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    guardKeyManagePage = new GuardKeyManagePage(centralWidget);
    connect(guardKeyManagePage,&GuardKeyManagePage::backBtnVisible,titleBar,&TitleBar::backBtnVis);
    guardKeyManagePage->setAttribute(Qt::WA_DeleteOnClose);
    guardKeyManagePage->show();
    currentPageNum = 12;
}

void Frame::showProposalPage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    proposalPage = new ProposalPage(centralWidget);
    proposalPage->setAttribute(Qt::WA_DeleteOnClose);
    proposalPage->show();
    currentPageNum = 13;
}

void Frame::showWithdrawConfirmPage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    withdrawConfirmPage = new WithdrawConfirmPage(centralWidget);
    withdrawConfirmPage->setAttribute(Qt::WA_DeleteOnClose);
    withdrawConfirmPage->show();
    currentPageNum = 14;
}

void Frame::showFeedPricePage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    feedPricePage = new FeedPricePage(centralWidget);
    feedPricePage->setAttribute(Qt::WA_DeleteOnClose);
    feedPricePage->show();
    currentPageNum = 15;
}

void Frame::showColdHotTransferPage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    coldHotTransferPage = new ColdHotTransferPage(centralWidget);
    coldHotTransferPage->setAttribute(Qt::WA_DeleteOnClose);
    coldHotTransferPage->show();
    currentPageNum = 16;
}

void Frame::showContractTokenPage()
{
    emit titleBackVisible(false);

    closeCurrentPage();
    contractTokenPage = new ContractTokenPage(centralWidget);
    connect(contractTokenPage,&ContractTokenPage::backBtnVisible,titleBar,&TitleBar::backBtnVis);
    contractTokenPage->setAttribute(Qt::WA_DeleteOnClose);
    contractTokenPage->show();
    currentPageNum = 20;
}

void Frame::showMultiSigTransactionPage(QString _multiSigAddress)
{
    closeCurrentPage();
    multiSigTransactionPage = new MultiSigTransactionPage(_multiSigAddress, centralWidget);
    connect(multiSigTransactionPage,SIGNAL(back()),this,SLOT(showMultiSigPage()));
    multiSigTransactionPage->setAttribute(Qt::WA_DeleteOnClose);
    multiSigTransactionPage->show();
    currentPageNum = 10;
  //朱正天  functionBar->choosePage(7);
}

void Frame::jsonDataUpdated(QString id)
{
    if( id == "id-is_new")
    {
        if(waitingForSync)
        {
            waitingForSync->close();
            waitingForSync = NULL;
        }

        activateWindow();
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result != "\"result\":true")
        {
            normalLogin = new NormalLogin(this);

            firstLogin = NULL;
            normalLogin->move(0,0);
            normalLogin->show();
            connect( normalLogin,SIGNAL(login()), this, SLOT(alreadyLogin()));
            connect( normalLogin,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( normalLogin,SIGNAL(closeWallet()),qApp,SLOT(quit()));
            connect( normalLogin,SIGNAL(tray()),this,SLOT(hide()));
            connect( normalLogin,SIGNAL(showShadowWidget()), this, SLOT(shadowWidgetShow()));
            connect( normalLogin,SIGNAL(hideShadowWidget()), this,SLOT(shadowWidgetHide()));

            setGeometry(0,0, normalLogin->width(), normalLogin->height());
            moveWidgetToScreenCenter(this);
        }
        else
        {
            firstLogin = new FirstLogin(this);
            normalLogin = NULL;
            firstLogin->move(0,0);
            firstLogin->show();
            connect( firstLogin,SIGNAL(login()), this, SLOT(alreadyLogin()));
            connect( firstLogin,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( firstLogin,SIGNAL(closeWallet()),qApp,SLOT(quit()));
            connect( firstLogin,SIGNAL(tray()),this,SLOT(hide()));
            connect( firstLogin,SIGNAL(showShadowWidget()), this, SLOT(shadowWidgetShow()));
            connect( firstLogin,SIGNAL(hideShadowWidget()), this,SLOT(shadowWidgetHide()));

            setGeometry(0,0, firstLogin->width(), firstLogin->height());
            moveWidgetToScreenCenter(this);
        }
//        else
//        {
//            // 其他情况视为钱包数据损坏
//            QDir dir(HXChain::getInstance()->appDataPath + "/wallets/wallet");
//            if(!dir.exists())   return;
//            QFileInfoList fileInfos = dir.entryInfoList( QJsonArray() << "*.log");
//            for(int i= 0; i < fileInfos.size(); i++)
//            {
//                QString path = fileInfos.at(i).absoluteFilePath();
//                QFile file(path);
//                file.rename(file.fileName() + ".bak");
//            }

//            int pos = result.indexOf("\"message\":\"") + 11;
//            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);


//            CommonDialog commonDialog(CommonDialog::OkOnly);
////            commonDialog.setText(QString::fromLocal8Bit("钱包数据损坏，请重新启动: ") + errorMessage );
//            commonDialog.setText(tr("Wallet data error: ") + errorMessage );
//            commonDialog.pop();

//            qApp->quit();
//        }

        return;
    }

    if( id == "id-list_my_accounts")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        HXChain::getInstance()->parseAccountInfo();

        if(HXChain::getInstance()->importedWalletNeedToAddTrackAddresses)
        {
            foreach (QString accountName, HXChain::getInstance()->accountInfoMap.keys())
            {
                qDebug() << accountName << HXChain::getInstance()->accountInfoMap.value(accountName).address;
                HXChain::getInstance()->addTrackAddress(HXChain::getInstance()->accountInfoMap.value(accountName).address);
            }

            HXChain::getInstance()->importedWalletNeedToAddTrackAddresses = false;
            HXChain::getInstance()->configFile->setValue("/settings/importedWalletNeedToAddTrackAddresses",false);

            HXChain::getInstance()->resyncNextTime = true;
            HXChain::getInstance()->configFile->setValue("/settings/resyncNextTime", true);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("This wallet is newly imported. It will rescan the blockchain data when launched next time."
                                    " After that the transactions of the accounts in this wallet will be shown.") );
            commonDialog.pop();
        }

        foreach (QString accountName, HXChain::getInstance()->accountInfoMap.keys())
        {
            HXChain::getInstance()->fetchAccountBalances(accountName);
        }

        HXChain::getInstance()->fetchMyContracts();

        return;
    }

    if( id.startsWith("id-get_account_balances-") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":["))
        {
            QString accountName = id.mid(id.indexOf("-",10) + 1);

            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();
            for(int i = 0; i < array.size(); i++)
            {
                QJsonObject object = array.at(i).toObject();
                AssetAmount assetAmount;
                assetAmount.assetId = object.take("asset_id").toString();

                QJsonValue value = object.take("amount");
                if(value.isString())
                {
                    assetAmount.amount = value.toString().toULongLong();
                }
                else
                {
                    assetAmount.amount = QString::number(value.toDouble(),'g',12).toULongLong();
                }

                HXChain::getInstance()->accountInfoMap[accountName].assetAmountMap.insert(assetAmount.assetId,assetAmount);
            }
        }



        return;
    }



    if( id == "id-lock")
    {
        if( lockPage )
        {
            qDebug() << "already exist a lockpage";
            return;
        }

        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")
        {
            shadowWidgetHide();

            lockPage = new LockPage(this);
            lockPage->setAttribute(Qt::WA_DeleteOnClose);
            lockPage->move(0,0);
            connect( lockPage,SIGNAL(unlock()),this,SLOT(unlock()));
            connect( lockPage,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( lockPage,SIGNAL(closeWallet()),qApp,SLOT(quit()));
            connect( lockPage,SIGNAL(tray()),this,SLOT(hide()));
            connect( lockPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
            connect( lockPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
            lockPage->show();

            HXChain::getInstance()->hideCurrentDialog();

            setGeometry(0,0, lockPage->width(), lockPage->height());
            moveWidgetToScreenCenter(this);
        }
        return;
    }

    if( id == "id-lock-onCloseWallet")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        hide();
        HXChain::getInstance()->isExiting = true;
        HXChain::getInstance()->postRPC( "id-witness_node_stop", toJsonFormat( "witness_node_stop", QJsonArray()));


        return;
    }
    if("id-witness_node_stop" == id)
    {
        HXChain::getInstance()->clientProc->waitForFinished();
        HXChain::getInstance()->nodeProc->waitForFinished();
        QTimer::singleShot(1,qApp,SLOT(quit()));
        return;
    }


    if( id == "id-list_assets")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1(), &json_error);
            if(json_error.error == QJsonParseError::NoError)
            {
                if( parse_doucment.isObject())
                {
                    QJsonObject jsonObject = parse_doucment.object();
                    if( jsonObject.contains("result"))
                    {
                        QJsonValue resultValue = jsonObject.take("result");
                        if( resultValue.isArray())
                        {
//                            HXChain::getInstance()->assetInfoMap.clear();
                            QMap<QString,AssetInfo> map;

                            QJsonArray resultArray = resultValue.toArray();
                            for( int i = 0; i < resultArray.size(); i++)
                            {
                                QJsonObject object = resultArray.at(i).toObject();
                                QString assetId = object.take("id").toString();
                                AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetId);

                                assetInfo.id = assetId;
                                assetInfo.issuer = object.take("issuer").toString();
                                assetInfo.precision = object.take("precision").toInt();
                                assetInfo.symbol = object.take("symbol").toString();
                                //                        if(assetInfo.symbol != ASSET_NAME)
                                //                        {
                                //                            assetInfo.symbol.prepend("link-");
                                //                        }

                                QJsonObject object2 = object.take("options").toObject();

                                QJsonValue value2 = object2.take("max_supply");
                                if( value2.isString())
                                {
                                    assetInfo.maxSupply = value2.toString().toULongLong();
                                }
                                else
                                {
                                    assetInfo.maxSupply = QString::number(value2.toDouble(),'g',10).toULongLong();
                                }

                                if(assetInfo.symbol != ASSET_NAME)
                                {
                                    HXChain::getInstance()->postRPC( "id-get_current_multi_address-" + assetInfo.symbol, toJsonFormat( "get_current_multi_address", QJsonArray() << assetInfo.symbol));
                                }


                                // 喂价相关
                                QJsonArray publisherArray = object.take("publishers").toArray();
                                assetInfo.publishers.clear();
                                foreach (QJsonValue v, publisherArray)
                                {
                                    assetInfo.publishers += v.toString();
                                }

                                assetInfo.currentFeedTime = object.take("current_feed_publication_time").toString();
                                QJsonObject currentFeedObject = object.take("current_feed").toObject();
                                QJsonObject settlementPriceObject = currentFeedObject.take("settlement_price").toObject();

                                QJsonObject baseObject = settlementPriceObject.take("base").toObject();
                                assetInfo.baseAmount.assetId = baseObject.take("asset_id").toString();
                                assetInfo.baseAmount.amount = jsonValueToULL(baseObject.take("amount"));

                                QJsonObject quoteObject = settlementPriceObject.take("quote").toObject();
                                assetInfo.quoteAmount.assetId = quoteObject.take("asset_id").toString();
                                assetInfo.quoteAmount.amount = jsonValueToULL(quoteObject.take("amount"));

//                                HXChain::getInstance()->assetInfoMap.insert(assetInfo.id,assetInfo);
                                map.insert(assetInfo.id, assetInfo);
                            }

                            HXChain::getInstance()->assetInfoMap = map;
                        }
                    }
                }
            }
        }

        return;
    }

    if( id.startsWith("id-get_current_multi_address-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;
        if(result.startsWith("\"result\":"))
        {
            QString assetSymbol = id.mid(QString("id-get_current_multi_address-").size());
            QString assetId = HXChain::getInstance()->getAssetId(assetSymbol);

            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonObject object = jsonObject.take("result").toObject();

            if(HXChain::getInstance()->assetInfoMap.contains(assetId))
            {
                HXChain::getInstance()->assetInfoMap[assetId].multisigAddressId = object.take("id").toString();
                HXChain::getInstance()->assetInfoMap[assetId].hotAddress = object.take("bind_account_hot").toString();
                HXChain::getInstance()->assetInfoMap[assetId].coldAddress = object.take("bind_account_cold").toString();
                HXChain::getInstance()->assetInfoMap[assetId].effectiveBlock = object.take("effective_block_num").toInt();
            }
        }

        return;
    }

    if( id == "id-list_miners")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {

            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();

            HXChain::getInstance()->minersVector.clear();
            foreach (QJsonValue v, array)
            {
                QJsonArray array2 = v.toArray();

                Miner miner;
                miner.name = array2.at(0).toString();
                miner.minerId = array2.at(1).toString();
                HXChain::getInstance()->minersVector.append(miner);
            }
        }

        return;
    }

    if( id == "id-list_guard_members")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();

            //        HXChain::getInstance()->formalGuardMap.clear();
            foreach (QJsonValue v, array)
            {
                QJsonArray array2 = v.toArray();
                QString account = array2.at(0).toString();
                GuardInfo info;
                if(HXChain::getInstance()->formalGuardMap.contains(account))
                {
                    info = HXChain::getInstance()->formalGuardMap.value(account);
                }
                info.guardId = array2.at(1).toString();
                info.accountId = HXChain::getInstance()->accountInfoMap.value(account).id;
                HXChain::getInstance()->formalGuardMap.insert(account, info);

                HXChain::getInstance()->fetchProposals();

                HXChain::getInstance()->postRPC( "id-get_guard_member-" + account, toJsonFormat( "get_guard_member", QJsonArray() << account));
                //            HXChain::getInstance()->fetchGuardAllMultisigAddresses(accountId);

                HXChain::getInstance()->postRPC( "guard-get_account-" + account, toJsonFormat( "get_account", QJsonArray() << account));

            }
        }

        return;
    }

    if( id.startsWith("id-get_guard_member-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            QString account = id.mid(QString("id-get_guard_member-").size());

            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonObject object = jsonObject.take("result").toObject();

            if(HXChain::getInstance()->formalGuardMap.contains(account))
            {
                HXChain::getInstance()->formalGuardMap[account].accountId   = object.take("guard_member_account").toString();
                HXChain::getInstance()->formalGuardMap[account].voteId      = object.take("vote_id").toString();
                HXChain::getInstance()->formalGuardMap[account].isFormal    = object.take("formal").toBool();

                HXChain::getInstance()->fetchGuardAllMultisigAddresses(HXChain::getInstance()->formalGuardMap[account].accountId);
            }
        }

        return;
    }

    if( id.startsWith("guard-get_account-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        //        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            QString account = id.mid(QString("guard-get_account-").size());

            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonObject object = jsonObject.take("result").toObject();

            if(HXChain::getInstance()->formalGuardMap.contains(account))
            {
                HXChain::getInstance()->formalGuardMap[account].address = object.take("addr").toString();
            }
        }

        return;
    }

    if( id == "id-list_all_guards")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();

            HXChain::getInstance()->allGuardMap.clear();
            foreach (QJsonValue v, array)
            {
                QJsonArray array2 = v.toArray();
                HXChain::getInstance()->allGuardMap.insert(array2.at(0).toString(),array2.at(1).toString());
            }
        }

        return;
    }

    if( id == "id-list_miners")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.value("result").toArray();

            //        HXChain::getInstance()->formalGuardMap.clear();
            foreach (QJsonValue v, array)
            {
                QJsonArray array2 = v.toArray();
                QString account = array2.at(0).toString();
                MinerInfo info;
                info.minerId = array2.at(1).toString();
                HXChain::getInstance()->minerMap.insert(account, info);

                HXChain::getInstance()->postRPC( "id-get_miner-" + account, toJsonFormat( "get_miner", QJsonArray() << account));
            }
        }

        return;
    }

    if( id.startsWith("id-get_miner-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            QString account = id.mid(QString("id-get_miner-").size());

            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonObject object = jsonObject.value("result").toObject();

            if(HXChain::getInstance()->minerMap.contains(account))
            {
                HXChain::getInstance()->minerMap[account].accountId     = object.value("miner_account").toString();
                HXChain::getInstance()->minerMap[account].signingKey    = object.value("signing_key").toString();
                HXChain::getInstance()->minerMap[account].totalMissed   = object.value("total_missed").toInt();
                HXChain::getInstance()->minerMap[account].totalProduced = object.value("total_produced").toInt();
                HXChain::getInstance()->minerMap[account].lastBlock     = object.value("last_confirmed_block_num").toInt();
                HXChain::getInstance()->minerMap[account].participationRate = object.value("participation_rate").toDouble();

            }
        }

        return;
    }

    if(id == "id-get_proposal_for_voter")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();

            HXChain::getInstance()->proposalInfoMap.clear();
            foreach (QJsonValue v, array)
            {
                QJsonObject object = v.toObject();
                ProposalInfo info;
                info.proposalId = object.take("id").toString();
                info.proposer   = object.take("proposer").toString();
                info.expirationTime = object.take("expiration_time").toString();
                QJsonValue proposedTransactionValue = object.take("proposed_transaction");
                info.transactionStr = QJsonDocument(proposedTransactionValue.toObject()).toJson();
                info.type       = object.take("type").toString();

                QJsonArray array2 = object.take("approved_key_approvals").toArray();
                foreach (QJsonValue v2, array2)
                {
                    info.approvedKeys += v2.toString();
                }

                QJsonArray array3 = object.take("disapproved_key_approvals").toArray();
                foreach (QJsonValue v3, array3)
                {
                    info.disapprovedKeys += v3.toString();
                }

                QJsonArray array4 = object.take("required_account_approvals").toArray();
                foreach (QJsonValue v4, array4)
                {
                    info.requiredAccounts += v4.toString();
                }

                info.proposalOperationType = proposedTransactionValue.toObject().take("operations").toArray()
                        .at(0).toArray().at(0).toInt();

                HXChain::getInstance()->proposalInfoMap.insert(info.proposalId, info);
            }
        }

        return;
    }

    if( id.startsWith("id-get_multi_address_obj-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            QString str = id.mid(QString("id-get_multi_address_obj-").size());
            QString assetSymbol = str.split("-").at(0);
            QString accountId = str.split("-").at(1);

            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();

            QVector<GuardMultisigAddress> vector;
            foreach (QJsonValue v, array)
            {
                GuardMultisigAddress gma;
                QJsonObject object = v.toObject();
                gma.multisigAddressObjectId = object.take("id").toString();
                gma.hotAddress  = object.take("new_address_hot").toString();
                gma.hotPubKey   = object.take("new_pubkey_hot").toString();
                gma.coldAddress = object.take("new_address_cold").toString();
                gma.coldPubKey  = object.take("new_pubkey_cold").toString();
                gma.pairId      = object.take("multisig_account_pair_object_id").toString();
                vector.append(gma);
            }
            HXChain::getInstance()->guardMultisigAddressesMap.insert(str,vector);
        }

        return;
    }

    if( id.startsWith("id-get_contracts_hash_entry_by_owner-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":"))
        {
            QString accountName = id.mid(QString("id-get_contracts_hash_entry_by_owner-").size());
//qDebug() << id << result;
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();

//            HXChain::getInstance()->accountInfoMap[accountName].contractsVector.clear();

            foreach (QJsonValue v, array)
            {
                QJsonObject object = v.toObject();

                ContractInfo contractInfo;
                contractInfo.contractAddress    = object.take("contract_address").toString();
                contractInfo.hashValue          = object.take("hash").toString();

                if(contractInfo.hashValue == EXCHANGE_CONTRACT_HASH)
                {
                    HXChain::getInstance()->postRPC( "id-invoke_contract_offline-state-" + accountName + "-" + contractInfo.contractAddress, toJsonFormat( "invoke_contract_offline",
                                                                                           QJsonArray() << accountName << contractInfo.contractAddress
                                                                                           << "state"  << ""));
                }

                if(!HXChain::getInstance()->accountInfoMap[accountName].contractsVector.contains(contractInfo))
                {
                    HXChain::getInstance()->accountInfoMap[accountName].contractsVector.append(contractInfo);
                }
            }
        }


        return;
    }

    if( id.startsWith("id-invoke_contract_offline-state-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":"))
        {
            QString str = id.mid(QString("id-invoke_contract_offline-state-").size());
            QStringList strList = str.split("-");
            QString accountName = strList.at(0);
            QString contractAddress = strList.at(1);

            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QString state = jsonObject.take("result").toString();

            for(int i = 0; i < HXChain::getInstance()->accountInfoMap[accountName].contractsVector.size(); i++)
            {
                if(HXChain::getInstance()->accountInfoMap[accountName].contractsVector[i].contractAddress == contractAddress)
                {
                    HXChain::getInstance()->accountInfoMap[accountName].contractsVector[i].state = state;
                }
            }
        }

        return;
    }

    if( id == "id-list_transactions")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();

            foreach (QJsonValue v, array)
            {
                QString transactionId = v.toString();
                TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct(transactionId);
//qDebug() << "list_transactions " << ts.transactionId <<ts.type;
                if(ts.type == -1 || ts.blockNum == 0)
                {
                    HXChain::getInstance()->postRPC( "id-get_transaction-" + transactionId, toJsonFormat( "get_transaction", QJsonArray() << transactionId));
                }
            }
        }


        return;
    }

    if( id.startsWith("id-get_transaction-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result ;

        if(result.startsWith("\"result\":"))
        {
            HXChain::getInstance()->parseTransaction(result);
        }
        else
        {
            QString trxId = id.mid(QString("id-get_transaction-").size());
            TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct(trxId);
            QDateTime dateTime = QDateTime::fromString(ts.expirationTime,"yyyy-MM-ddThh:mm:ss");
            dateTime.setTimeSpec(Qt::UTC);
            QDateTime currentTime = QDateTime::currentDateTimeUtc();

            if(dateTime < currentTime)      // 如果交易失效了 则从DB中删掉
            {
                HXChain::getInstance()->transactionDB.removeTransactionStruct(trxId);
            }
        }

        return;
    }

}

void Frame::onBack()
{
    switch (currentPageNum) {
    case 0:
        showMainPage();
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        showMainPage();
        break;
    case 4:
        showContactPage();
        break;
    case 5:
//        myExchangeContractPage->onBack();
//        emit titleBackVisible(false);
        showMyExchangeContractPage();
        break;
    case 6:
//        onchainOrderPage->onBack();
//        emit titleBackVisible(false);
        showOnchainOrderPage();
        break;
    case 7:
        break;
    case 8:
        break;
    case 9:
        break;
    case 10:
        break;
    case 11:
        showPoundagePage();
        break;
    case 12:
        showKeyManagePage();
        break;
    case 20:
        showContractTokenPage();
        break;
    default:
        break;
    }

}

void Frame::closeEvent(QCloseEvent *e)
{

    hide();
    e->ignore();

}

void Frame::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        //点击托盘显示窗口
        case QSystemTrayIcon::Trigger:
        {
            showNormalAndActive();
            break;
        }

        default:
            break;
    }
}

void Frame::createTrayIconActions()
{
     minimizeAction = new QAction(tr("Minimize"), this);
     connect(minimizeAction, SIGNAL(triggered()), this, SLOT(showMinimized()));

     restoreAction = new QAction(tr("Restore"), this);
     connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormalAndActive()));

     quitAction = new QAction(tr("Quit"), this);
     connect(quitAction, SIGNAL(triggered()), this, SLOT(onCloseWallet()));
}

void Frame::createTrayIcon()
{
     trayIconMenu = new QMenu(this);
     trayIconMenu->addAction(minimizeAction);
     trayIconMenu->addAction(restoreAction);
     trayIconMenu->addSeparator();
     trayIconMenu->addAction(quitAction);
     trayIcon->setContextMenu(trayIconMenu);
}

void Frame::showNormalAndActive()
{
    showNormal();
    activateWindow();
}


// 提前载入，防止切换到别的界面显示不出来
void Frame::init()
{
    HXChain::getInstance()->postRPC( "id-list_assets", toJsonFormat( "list_assets", QJsonArray() << "A" << "100"));

    HXChain::getInstance()->postRPC( "id-list_miners", toJsonFormat( "list_miners", QJsonArray() << "A" << "100"));

    //HXChain::getInstance()->postRPC( "id-network_add_nodes", toJsonFormat( "network_add_nodes", QJsonArray() << (QJsonArray() << "192.168.1.195:5444") ));

    HXChain::getInstance()->fetchTransactions();

    HXChain::getInstance()->fetchFormalGuards();
    HXChain::getInstance()->fetchAllGuards();
}


void Frame::paintEvent(QPaintEvent *e)
{

//        QStyleOption opt;

//        opt.init(this);
//        QPainter p(this);
//        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//        QWidget::paintEvent(e);

}

void Frame::EnlargeRightPart()
{
    functionBar->resize(62,functionBar->height());

    centralWidget->resize(898,centralWidget->height());

    centralWidget->move(62, titleBar->height());
//    if(QWidget *widget = centralWidget->childAt(10,10))
//    {
//        widget->resize(805,centralWidget->height());
//    }

    contactPage->resize(898,centralWidget->height());


    //bottomBar->resize(805,40);
    //bottomBar->move(95,516);
}

void Frame::RestoreRightPart()
{
    functionBar->resize(190,functionBar->height());

    centralWidget->move(190, titleBar->height());
    if(QWidget *widget = centralWidget->childAt(10,10))
    {
        widget->resize(770,centralWidget->height());
    }
    centralWidget->resize(770,centralWidget->height());

    //bottomBar->resize(680,40);
    //bottomBar->move(220,516);
}

void Frame::installBlurEffect(QWidget *widget)
{
    if(!widget) return;
    CustomShadowEffect *bodyShadow = new CustomShadowEffect(this);
    bodyShadow->setBlurRadius(20.0);
    bodyShadow->setDistance(5.0);
    bodyShadow->setColor(QColor(150, 150, 150, 35));
    widget->setGraphicsEffect(bodyShadow);
}

void Frame::newAccount(QString name)
{
    getAccountInfo();
}

void Frame::onCloseWallet()
{
    HXChain::getInstance()->postRPC( "id-lock-onCloseWallet", toJsonFormat( "lock", QJsonArray()));
}

void Frame::ShowBubbleMessage(const QString &title, const QString &context,QSystemTrayIcon::MessageIcon icon, int msecs)
{
    trayIcon->showMessage(title,context,icon,msecs);
}

