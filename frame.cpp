#include <QPainter>
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
#include "accountpage.h"
#include "transferpage.h"
#include "bottombar.h"
#include "lockpage.h"
#include "titlebar.h"
#include "frame.h"
#include "lnk.h"
#include "debug_log.h"
#include "waitingforsync.h"
#include <QDesktopWidget>

#include "functionbar.h"
#include "contactpage.h"
#include "selectwalletpathwidget.h"
#include "control/shadowwidget.h"
#include "control/showbottombarwidget.h"
#include "contract/smartcontractpage.h"
#include "commondialog.h"
#include "feedpage.h"
#include "multisig/multisigpage.h"
#include "multisig/multisigtransactionpage.h"
#include "neworimportwalletwidget.h"

Frame::Frame(): timer(NULL),
    firstLogin(NULL),
    normalLogin(NULL),
    mainPage(NULL),
    accountPage(NULL),
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
    smartContractPage(NULL),
    feedPage(NULL),
    multiSigPage(NULL),
    multiSigTransactionPage(NULL),
    needToRefresh(false),
    timerForCollectContractTransaction(NULL)
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
    lastPage = "";
    currentAccount = "";


    connect(UBChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    QString language = UBChain::getInstance()->language;
    if( language.isEmpty())
    {
//        setLanguage("Simplified Chinese");
        setLanguage("English");
    }
    else
    {
        setLanguage(language);
    }

    setGeometry(0,0, 900, 556);
    moveWidgetToScreenCenter(this);

    shadowWidget = new ShadowWidget(this);
    shadowWidget->hide();
    shadowWidget->init(this->size());

    //  如果是第一次使用(未设置 blockchain 路径)
    mutexForConfigFile.lock();

    if( !UBChain::getInstance()->configFile->contains("/settings/chainPath") )
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
        QString path = UBChain::getInstance()->configFile->value("/settings/chainPath").toString();
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

            UBChain::getInstance()->configFile->clear();
            UBChain::getInstance()->configFile->setValue("/settings/lockMinutes",5);
            UBChain::getInstance()->lockMinutes     = 5;
            UBChain::getInstance()->configFile->setValue("/settings/notAutoLock",false);
            UBChain::getInstance()->notProduce      =  true;
            UBChain::getInstance()->configFile->setValue("/settings/language","English");
            UBChain::getInstance()->language = "English";
            UBChain::getInstance()->minimizeToTray  = false;
            UBChain::getInstance()->configFile->setValue("/settings/minimizeToTray",false);
            UBChain::getInstance()->closeToMinimize = false;
            UBChain::getInstance()->configFile->setValue("/settings/closeToMinimize",false);

        }
        else
        {
            UBChain::getInstance()->startExe();

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

//            UBChain::getInstance()->appDataPath = UBChain::getInstance()->configFile->value("/settings/chainPath").toString();

//            QStringList strList;
//            strList << "--data-dir" << UBChain::getInstance()->configFile->value("/settings/chainPath").toString()
//                    << "--rpcuser" << "a" << "--rpcpassword" << "b" << "--rpcport" << QString::number( RPC_PORT) << "--server";
//            if( UBChain::getInstance()->configFile->value("/settings/resyncNextTime",false).toBool())
//            {
//                strList << "--resync-blockchain";

//                QFile file(UBChain::getInstance()->configFile->value("/settings/chainPath").toString() + "\\config.json");
//                if(file.exists())   file.remove();
//            }
//            UBChain::getInstance()->configFile->setValue("settings/resyncNextTime",false);

//            UBChain::getInstance()->proc->start( WALLET_EXE_FILE,strList );

//            if( UBChain::getInstance()->proc->waitForStarted())
//            {
//                qDebug() << QString("launch %1 successfully").arg(WALLET_EXE_FILE);
//            }
//            else
//            {
//                qDebug() << QString("launch %1 failed").arg(WALLET_EXE_FILE);
//            }


        }


    }
    mutexForConfigFile.unlock();

    trayIcon = new QSystemTrayIcon(this);
    //放在托盘提示信息、托盘图标
    trayIcon ->setToolTip(QString("UBWallet ") + WALLET_VERSION);
    trayIcon ->setIcon(QIcon(":/pic/wallet_ui/LNK.ico"));
    //点击托盘执行的事件
    connect(trayIcon , SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
    createTrayIconActions();
    createTrayIcon();


}

Frame::~Frame()
{

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
    setGeometry(0,0, 900, 556);
    moveWidgetToScreenCenter(this);

    titleBar = new TitleBar(this);
    titleBar->setGeometry(220,0,680,34);
    connect(titleBar,SIGNAL(minimum()),this,SLOT(showMinimized()));
    connect(titleBar,SIGNAL(closeWallet()),qApp,SLOT(quit()));
    connect(titleBar,SIGNAL(tray()),this,SLOT(hide()));

    titleBar->show();

    centralWidget = new QWidget(this);
    centralWidget->setGeometry(220,34,680,482);
    centralWidget->show();

    bottomBar = new BottomBar(this);
    bottomBar->move(220,516);
    bottomBar->show();

//    showBottomBarWidget = new ShowBottomBarWidget(centralWidget);
//    showBottomBarWidget->setGeometry(0,525,827,20);
//    connect(showBottomBarWidget, SIGNAL(showBottomBar()), bottomBar, SLOT(dynamicShow()) );
//    showBottomBarWidget->show();

    functionBar = new FunctionBar(this);
    functionBar->move(0,0);
    functionBar->show();
    connect(functionBar, SIGNAL(showMainPage()), this, SLOT( showMainPage()));
    connect(functionBar, SIGNAL(showAccountPage()), this, SLOT( showAccountPage()));
    connect(functionBar, SIGNAL(showSmartContractPage()), this, SLOT( showSmartContractPage()));
    connect(functionBar, SIGNAL(showTransferPage()), this, SLOT( showTransferPage()));
    connect(functionBar, SIGNAL(showContactPage()), this, SLOT( showContactPage()));
    connect(functionBar, SIGNAL(showFeedPage()), this, SLOT( showFeedPage()));
    connect(functionBar, SIGNAL(showMultiSigPage()), this, SLOT(showMultiSigPage()));
    connect(functionBar,SIGNAL(lock()),this,SLOT(showLockPage()));

    getAccountInfo();

    mainPage = new MainPage(centralWidget);
    mainPage->setAttribute(Qt::WA_DeleteOnClose);
//    QTimer::singleShot(1000,mainPage,SLOT(show()));
    mainPage->show();
    currentPageNum = 0;
    connect(mainPage,SIGNAL(openAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(mainPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(mainPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(mainPage,SIGNAL(showTransferPage(QString)),this,SLOT(showTransferPage(QString)));
    connect(mainPage,SIGNAL(newAccount(QString)),this,SLOT(newAccount(QString)));
    connect(functionBar,SIGNAL(assetChanged(int)),mainPage,SLOT(updateAccountList()));

    timer = new QTimer(this);               //  自动锁定
    connect(timer,SIGNAL(timeout()),this,SLOT(autoLock()));
    if(UBChain::getInstance()->notProduce)
    {
        timer->start(UBChain::getInstance()->lockMinutes * 60000);
    }

    startTimerForAutoRefresh();              // 自动刷新
    startTimerForCollectContractTransaction();

    init();

    QTimer::singleShot(5000,UBChain::getInstance(),SLOT(readAllAccountContractTransactions()));

    UBChain::getInstance()->contractServerInfoManager.getContractInfoFromLocalFile();
    UBChain::getInstance()->contractServerInfoManager.getContractInfoFromServer();
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


    UBChain::getInstance()->postRPC( "id_wallet_list_my_addresses", toJsonFormat( "wallet_list_my_addresses", QStringList() << ""));

    UBChain::getInstance()->postRPC( "id_balance", toJsonFormat( "balance", QStringList() << ""));

    UBChain::getInstance()->updateAllContractBalance();

//    UBChain::getInstance()->collectContracts();

}

void Frame::showAccountPage()
{
    QStringList accounts = UBChain::getInstance()->addressMap.keys();
    if( accounts.size() < 1)    return;
    showAccountPage(accounts.at(0));
}

void Frame::showAccountPage(QString accountName)
{
    currentAccount = accountName;
    UBChain::getInstance()->mainFrame->setCurrentAccount(accountName);
    getAccountInfo();

    closeCurrentPage();
    accountPage = new AccountPage(accountName,centralWidget);
    accountPage->setAttribute(Qt::WA_DeleteOnClose);
    accountPage->show();
    currentPageNum = 1;
    connect(accountPage,SIGNAL(back()),this,SLOT(showMainPage()));
    connect(accountPage,SIGNAL(accountChanged(QString)),this,SLOT(showAccountPage(QString)));
    connect(accountPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(accountPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(functionBar,SIGNAL(assetChanged(int)),this,SLOT(refresh()));

    functionBar->choosePage(4);

}

void Frame::showTransferPage(QString accountName)
{


    closeCurrentPage();
    getAccountInfo();
    UBChain::getInstance()->mainFrame->setCurrentAccount(accountName);
    transferPage = new TransferPage(accountName,centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(transferPage,SIGNAL(showAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(functionBar,SIGNAL(assetChanged(int)),transferPage,SLOT(onAssetChanged(int)));
    transferPage->show();

    currentPageNum = 3;
    functionBar->choosePage(2);


}


void Frame::showLockPage()
{

    timer->stop();
    shadowWidgetShow();


    UBChain::getInstance()->postRPC( "id_lock", toJsonFormat( "lock", QStringList() << "" ));
qDebug() << "lock ";

}

void Frame::autoLock()
{

    timer->stop();

    UBChain::getInstance()->postRPC( "id_lock", toJsonFormat( "lock", QStringList() << "" ));
qDebug() << "autolock ";

}

void Frame::unlock()
{
    setGeometry(0,0, 900, 556);
    moveWidgetToScreenCenter(this);

    if( UBChain::getInstance()->notProduce)
    {
        timer->start(UBChain::getInstance()->lockMinutes * 60000);
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
    UBChain::getInstance()->showCurrentDialog();


}

void Frame::updateTimer()
{
    if( timer != NULL && lockPage == NULL)
    {
        if( UBChain::getInstance()->notProduce)
        {
            timer->stop();
            timer->start(UBChain::getInstance()->lockMinutes * 60000);
        }
    }
}

void Frame::settingSaved()
{

    if( !UBChain::getInstance()->notProduce)
    {
        timer->stop();
    }
    else
    {
        timer->start( UBChain::getInstance()->lockMinutes * 60000);
    }

    QString language = UBChain::getInstance()->language;
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
//    if( UBChain::getInstance()->notProduce)
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

void Frame::refreshAccountInfo()
{
    needToRefresh = true;
    getAccountInfo();
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
//    RpcThread* rpcThread = new RpcThread;
//    connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//    rpcThread->setLogin("a","b");
//    rpcThread->setWriteData( toJsonFormat( "id_open", "open", QStringList() << "wallet" ));
//    rpcThread->start();

//    UBChain::getInstance()->initWorkerThreadManager();

    if( UBChain::getInstance()->contactsFile == NULL)
    {
        UBChain::getInstance()->getContactsFile();
    }

    UBChain::getInstance()->postRPC("id-is_new", toJsonFormat( "is_new", QStringList() << "" ));
}

void Frame::setCurrentAccount(QString accountName)
{
    UBChain::getInstance()->currentAccount = accountName;

    if(functionBar != NULL)
    {
        functionBar->setCurrentAccount(accountName);
    }
}

void Frame::setCurrentToken(QString tokenAddress)
{
    UBChain::getInstance()->currentTokenAddress = tokenAddress;

    functionBar->updateAssetComboBox();
}

void Frame::closeCurrentPage()
{
    switch (currentPageNum) {
    case 0:
        mainPage->close();
        mainPage = NULL;
        break;
    case 1:
        accountPage->close();
        accountPage = NULL;
        break;
    case 2:
        break;
    case 3:
        transferPage->close();
        transferPage = NULL;
        break;
    case 4:
        contactPage->close();
        contactPage = NULL;
        break;
    case 5:

        break;
    case 6:
        feedPage->close();
        feedPage = NULL;
        break;
    case 8:
        smartContractPage->close();
        smartContractPage = NULL;
        break;
    case 9:
        multiSigPage->close();
        multiSigPage = NULL;
        break;
    case 10:
        multiSigTransactionPage->close();
        multiSigTransactionPage = NULL;
        break;
    default:
        break;
    }


}

void Frame::refresh()
{
//    getAccountInfo();

    switch (currentPageNum) {
    case 0:
        mainPage->refresh();
        break;
    case 1:
        accountPage->refresh();
        break;
    case 2:

        break;
    case 3:
        transferPage->refresh();
        break;
    case 4:
        break;
    case 5:
//        showAccountPage(currentAccount);

        break;
    case 6:
        break;
    case 7:
//        showUpgradePage();
        break;
    case 8:
        break;
    default:
        break;
    }
}

void Frame::autoRefresh()
{
    getAccountInfo();

    UBChain::getInstance()->getTokensType();
    UBChain::getInstance()->getFeeders();

    UBChain::getInstance()->startCollectContractTransactions();
    UBChain::getInstance()->saveAllAccountContractTransactions();

    bottomBar->refresh();
    functionBar->refresh();

    switch (currentPageNum) {
    case 0:
        mainPage->refresh();
        break;
    case 1:
        if( lockPage == NULL)     // 锁定的时候 refresh会崩溃
        {
            accountPage->refresh();
        }
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
//        showAccountPage(currentAccount);
//        if( lockPage == NULL)     // 锁定的时候 refresh会崩溃
//        {
//            accountPage->refresh();
//        }
        break;
    case 6:
        feedPage->refresh();
        break;
    case 7:
//        showUpgradePage();
        break;
    case 8:
        smartContractPage->refresh();
        break;
    case 9:
        multiSigPage->refresh();
        break;
    case 10:
        multiSigTransactionPage->refresh();
        break;
    default:
        break;
    }
}


void Frame::showMainPage()
{
    closeCurrentPage();
    getAccountInfo();

    mainPage = new MainPage(centralWidget);
    mainPage->setAttribute(Qt::WA_DeleteOnClose);
    mainPage->show();
    currentPageNum = 0;
    connect(mainPage,SIGNAL(openAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(mainPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(mainPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
//    connect(mainPage,SIGNAL(refreshAccountInfo()),this,SLOT(refreshAccountInfo()));
    connect(mainPage,SIGNAL(showTransferPage(QString)),this,SLOT(showTransferPage(QString)));
    connect(mainPage,SIGNAL(newAccount(QString)),this,SLOT(newAccount(QString)));
    connect(functionBar,SIGNAL(assetChanged(int)),mainPage,SLOT(updateAccountList()));

}


void Frame::showSmartContractPage()
{
    closeCurrentPage();
    getAccountInfo();

    smartContractPage = new SmartContractPage(centralWidget);
    smartContractPage->setAttribute(Qt::WA_DeleteOnClose);
    smartContractPage->show();
    currentPageNum = 8;
}

void Frame::showTransferPage()
{
    closeCurrentPage();
    getAccountInfo();
    transferPage = new TransferPage(UBChain::getInstance()->currentAccount,centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(transferPage,SIGNAL(showAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(functionBar,SIGNAL(assetChanged(int)),transferPage,SLOT(onAssetChanged(int)));
    transferPage->show();
    currentPageNum = 3;
    functionBar->choosePage(2);
}


void Frame::showContactPage()
{
    closeCurrentPage();    
    getAccountInfo();
    contactPage = new ContactPage(centralWidget);
    connect(contactPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(contactPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(contactPage,SIGNAL(gotoTransferPage(QString,QString)),this,SLOT(showTransferPageWithAddress(QString,QString)));
    contactPage->setAttribute(Qt::WA_DeleteOnClose);
    contactPage->show();
    currentPageNum = 4;
}

bool Frame::eventFilter(QObject* watched, QEvent* e)
{
    if( (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::KeyPress)  )
    {
        updateTimer();
    }

    // currentDialog 上的鼠标事件也会移动 frame 和 本身
    if( UBChain::getInstance()->currentDialogVector.contains(  (QWidget*)watched) )
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

        UBChain::getInstance()->resetPosOfCurrentDialog();  // currentDialog 一起移动

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

void Frame::showTransferPageWithAddress(QString address, QString remark)
{
    closeCurrentPage();
    getAccountInfo();
    transferPage = new TransferPage(UBChain::getInstance()->currentAccount,centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
    transferPage->setAddress(address);
    transferPage->setContact(remark);
    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(transferPage,SIGNAL(showAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(functionBar,SIGNAL(assetChanged(int)),transferPage,SLOT(onAssetChanged(int)));
    transferPage->show();
    currentPageNum = 3;
    functionBar->choosePage(2);
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
//            mainPage->retranslator(language);
            showAccountPage( currentAccount);
            break;
        case 2:
            break;
        case 3:
//            transferPage->retranslator(language);
            showTransferPage(currentAccount);
            break;
        case 4:
//            contactPage->retranslator(language);
            showContactPage();
            break;
        case 5:
            break;
        case 6:
            showFeedPage();
            break;
        case 7:
//            upgradePage->retranslator();
//            showUpgradePage(currentAccount);
            break;
        case 8:
            showSmartContractPage();
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

    setGeometry(0,0, waitingForSync->width(), waitingForSync->height());
    moveWidgetToScreenCenter(this);
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

void Frame::showFeedPage()
{
    closeCurrentPage();
    feedPage = new FeedPage(centralWidget);
    feedPage->setAttribute(Qt::WA_DeleteOnClose);
    feedPage->show();
    currentPageNum = 6;
    functionBar->choosePage(6);
}

void Frame::showMultiSigPage()
{
    closeCurrentPage();
    multiSigPage = new MultiSigPage(centralWidget);
    connect(multiSigPage,SIGNAL(goToTransferPage(QString,QString)),this,SLOT(showTransferPageWithAddress(QString,QString)));
    connect(multiSigPage,SIGNAL(showMultiSigTransactionPage(QString)),this,SLOT(showMultiSigTransactionPage(QString)));
    multiSigPage->setAttribute(Qt::WA_DeleteOnClose);
    multiSigPage->show();
    currentPageNum = 9;
    functionBar->choosePage(7);
}

void Frame::showMultiSigTransactionPage(QString _multiSigAddress)
{
    closeCurrentPage();
    multiSigTransactionPage = new MultiSigTransactionPage(_multiSigAddress, centralWidget);
    connect(multiSigTransactionPage,SIGNAL(back()),this,SLOT(showMultiSigPage()));
    multiSigTransactionPage->setAttribute(Qt::WA_DeleteOnClose);
    multiSigTransactionPage->show();
    currentPageNum = 10;
    functionBar->choosePage(7);
}

void Frame::jsonDataUpdated(QString id)
{
    if( id == "id-is_new")
    {
        waitingForSync->close();
        waitingForSync = NULL;

        activateWindow();
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result != "\"result\":true")
        {
            normalLogin = new NormalLogin(this);

            firstLogin = NULL;
            normalLogin->setGeometry(0,0,240,320);
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
            firstLogin->setGeometry(0,0,240,320);
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
//            QDir dir(UBChain::getInstance()->appDataPath + "/wallets/wallet");
//            if(!dir.exists())   return;
//            QFileInfoList fileInfos = dir.entryInfoList( QStringList() << "*.log");
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

    if( id == "id_wallet_list_my_addresses")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        QStringList accountList;

        QStringList strList = result.split("},{");

        foreach (QString str, strList)
        {
            int pos = str.indexOf("\"name\":\"") + 8;
            if( pos == 7)  break;   // 如果还没有账号

            accountList += str.mid(pos, str.indexOf("\",", pos) - pos);
        }

        mutexForConfigFile.lock();
        UBChain::getInstance()->configFile->beginGroup("/accountInfo");
        QStringList keys = UBChain::getInstance()->configFile->childKeys();
        UBChain::getInstance()->configFile->endGroup();


        foreach (QString key, keys)
        {
            QString addName = UBChain::getInstance()->configFile->value("/accountInfo/" + key).toString();
            if( !accountList.contains( addName) && !addName.isEmpty())
            {
                // 如果config记录的账户钱包中没有 则清除config文件中记录以及内存中各map
                UBChain::getInstance()->deleteAccountInConfigFile( addName);
                UBChain::getInstance()->addressMapRemove( addName);
                UBChain::getInstance()->balanceMapRemove( addName);
                UBChain::getInstance()->registerMapRemove( addName);
            }
        }

        UBChain::getInstance()->configFile->beginGroup("/accountInfo");
        keys = UBChain::getInstance()->configFile->childKeys();

        foreach (QString key, keys)
        {
            QString addName = UBChain::getInstance()->configFile->value(key).toString();
            UBChain::getInstance()->balanceMapInsert(addName, UBChain::getInstance()->getBalance( addName));
            UBChain::getInstance()->registerMapInsert(addName, UBChain::getInstance()->getRegisterTime( addName));
            UBChain::getInstance()->addressMapInsert(addName, UBChain::getInstance()->getAddress (addName));
            accountList.removeAll( addName);     // 如果钱包中有账号 config中未记录， 保留在accountList里
        }
        UBChain::getInstance()->configFile->endGroup();

        if( !accountList.isEmpty())    // 把config中没有记录的账号写入config中
        {
            for( int i = 0; i < accountList.size(); i++)
            {
                QString addName = accountList.at(i);
                mutexForBalanceMap.lock();
                QString accountName = QString(QString::fromLocal8Bit("账户") + toThousandFigure(UBChain::getInstance()->balanceMap.size() + 1));
                mutexForBalanceMap.unlock();
                UBChain::getInstance()->configFile->setValue("/accountInfo/" + accountName, addName);
                UBChain::getInstance()->balanceMapInsert(addName, UBChain::getInstance()->getBalance( addName));
                UBChain::getInstance()->registerMapInsert(addName, UBChain::getInstance()->getRegisterTime( addName));
                UBChain::getInstance()->addressMapInsert(addName, UBChain::getInstance()->getAddress ( addName));
            }
        }
        mutexForConfigFile.unlock();

        if( needToRefresh)
        {
            refresh();
            needToRefresh = false;
        }

        return;
    }


    if( id == "id_lock")
    {
        if( lockPage )
        {
            qDebug() << "already exist a lockpage";
            return;
        }

        QString result = UBChain::getInstance()->jsonDataValue(id);
        if( result == "\"result\":null")
        {
            shadowWidgetHide();

            lockPage = new LockPage(this);
            lockPage->setAttribute(Qt::WA_DeleteOnClose);
            lockPage->setGeometry(0,0,240,320);
            connect( lockPage,SIGNAL(unlock()),this,SLOT(unlock()));
            connect( lockPage,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( lockPage,SIGNAL(closeWallet()),qApp,SLOT(quit()));
            connect( lockPage,SIGNAL(tray()),this,SLOT(hide()));
            connect( lockPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
            connect( lockPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
            lockPage->show();

            UBChain::getInstance()->hideCurrentDialog();

            setGeometry(0,0, lockPage->width(), lockPage->height());
            moveWidgetToScreenCenter(this);
        }
        return;
    }


    if( id.mid(0,37) == "id_wallet_get_account_public_address-" )
    {
//        QString  result = UBChain::getInstance()->jsonDataValue(id);
//        QString name = id.mid(37);

//        if( result.mid(0,9) == "\"result\":")
//        {
//            QString address = result.mid(10);
//            address.remove('\"');
//            UBChain::getInstance()->addressMapInsert(name, address);

//            refreshAccountInfo();
//        }

        return;
    }

    if( id == "id_blockchain_list_assets")
    {
        UBChain::getInstance()->parseAssetInfo();

        updateAssets();

        UBChain::getInstance()->updateAllToken();

        return;
    }

    if( id == "id_balance")
    {
        UBChain::getInstance()->parseBalance();
        return;
    }

    if( id.startsWith( "id_contract_get_info+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            QString contract = id.mid(QString("id_contract_get_info+").size());

            result.prepend("{");
            result.append("}");

            QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
            QByteArray ba = utfCodec->fromUnicode(result);

            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);
            if(json_error.error == QJsonParseError::NoError)
            {
                if( parse_doucment.isObject())
                {
                    QJsonObject jsonObject = parse_doucment.object();
                    if( jsonObject.contains("result"))
                    {
                        QJsonValue resultValue = jsonObject.take("result");
                        if( resultValue.isObject())
                        {
                            QJsonObject resultObject = resultValue.toObject();
                            QString contractName = resultObject.take("contract_name").toString();
                            QString level = resultObject.take("level").toString();
                            QString id = resultObject.take("id").toString();
                            QString description = resultObject.take("description").toString();
                            if(level == "forever")
                            {
                                UBChain::getInstance()->ERC20TokenInfoMap[id].contractAddress = id;
                                UBChain::getInstance()->ERC20TokenInfoMap[id].contractName = contractName;
                                UBChain::getInstance()->ERC20TokenInfoMap[id].level = level;
                                UBChain::getInstance()->ERC20TokenInfoMap[id].description = description;

                                ContractServerInfoMap* map = &UBChain::getInstance()->contractServerInfoManager.contractServerInfoSlice.map;
                                if(map->keys().contains(id))
                                {
                                    UBChain::getInstance()->ERC20TokenInfoMap[id].website = map->value(id).website;
                                    UBChain::getInstance()->ERC20TokenInfoMap[id].hotValue = map->value(id).hotValue;
                                    UBChain::getInstance()->ERC20TokenInfoMap[id].logoPath = UBChain::getInstance()->contractServerInfoManager.serverPathToLocalPath(map->value(id).logoPath);
                                    UBChain::getInstance()->ERC20TokenInfoMap[id].logoVersion = map->value(id).logoVersion;
                                    UBChain::getInstance()->ERC20TokenInfoMap[id].summary = map->value(id).summary;
                                }

                                UBChain::getInstance()->postRPC( "id_contract_call_offline_state+" + id,
                                                       toJsonFormat( "contract_call_offline", QStringList() << id
                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "state" << ""
                                                       ));

                            }

                        }
                    }
                }
            }
        }
        return;
    }

    if( id.startsWith( "id_contract_call_offline_state+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith( "\"result\":"))
        {
            QString contractAddress = id.mid(31);

            if( !UBChain::getInstance()->ERC20TokenInfoMap.contains(contractAddress))        return;

            QString state = result.mid(9);
            state.remove('\"');
            UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].state = state;

            if( state == "NOT_INITED")
            {
                return;
            }
            else
            {
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].contractAddress = contractAddress;
//                UBChain::getInstance()->configFile->setValue("/AddedContractToken/" + contractAddress,1);
                UBChain::getInstance()->postRPC( "id_contract_call_offline_+mortgageRate+" + contractAddress,
                                                 toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                               << UBChain::getInstance()->addressMap.keys().at(0) << "mortgageRate" << ""
                                                               ));
            }
        }
        return;
    }

    if( id.startsWith( "id_contract_call_offline_+mortgageRate+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        int pos = 26;
        QString contractAddress = id.mid(id.indexOf("+" , pos) + 1);
        if( !UBChain::getInstance()->ERC20TokenInfoMap.contains(contractAddress))        return;

        if( result.startsWith("\"result\":"))
        {
            QString mortgageRate = result.mid(9);
            mortgageRate.remove('\"');
            UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].isSmart = true;
            UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].mortgageRate = mortgageRate;

            UBChain::getInstance()->postRPC( "id_contract_call_offline_+anchorTokenSymbol+" + contractAddress,
                                             toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "anchorTokenSymbol" << ""
                                                       ));

            UBChain::getInstance()->postRPC( "id_contract_call_offline_+anchorTokenPrecision+" + contractAddress,
                                             toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "anchorTokenPrecision" << ""
                                                       ));

            UBChain::getInstance()->postRPC( "id_contract_call_offline_+anchorRatio+" + contractAddress,
                                             toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "anchorRatio" << ""
                                                       ));

            UBChain::getInstance()->postRPC( "id_contract_call_offline_+contractBalance+" + contractAddress,
                                             toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "contractBalance" << ""
                                                       ));

            UBChain::getInstance()->postRPC( "id_contract_call_offline_+lastTransferBlockNumber+" + contractAddress,
                                             toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "lastTransferBlockNumber" << ""
                                                       ));

            UBChain::getInstance()->postRPC( "id_contract_call_offline_+query_can_mint+" + contractAddress,
                                             toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "query_can_mint" << ""
                                                       ));

            UBChain::getInstance()->postRPC( "id_contract_call_offline_+base_mortgage_rate+" + contractAddress,
                                             toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "base_mortgage_rate" << ""
                                                       ));

            UBChain::getInstance()->postRPC( "id_contract_call_offline_+unusedBalance+" + contractAddress,
                                             toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "unusedBalance" << ""
                                                       ));
        }
        else
        {
            UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].isSmart = false;
        }


        UBChain::getInstance()->postRPC( "id_contract_call_offline_+tokenName+" + contractAddress,
                                         toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                   << UBChain::getInstance()->addressMap.keys().at(0) << "tokenName" << ""
                                                   ));

        UBChain::getInstance()->postRPC( "id_contract_call_offline_+precision+" + contractAddress,
                                         toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                   << UBChain::getInstance()->addressMap.keys().at(0) << "precision" << ""
                                                   ));

        UBChain::getInstance()->postRPC( "id_contract_call_offline_+admin+" + contractAddress,
                                         toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                   << UBChain::getInstance()->addressMap.keys().at(0) << "admin" << ""
                                                   ));

        UBChain::getInstance()->postRPC( "id_contract_call_offline_+tokenSymbol+" + contractAddress,
                                         toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                   << UBChain::getInstance()->addressMap.keys().at(0) << "tokenSymbol" << ""
                                                   ));

        UBChain::getInstance()->postRPC( "id_contract_call_offline_+totalSupply+" + contractAddress,
                                         toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                   << UBChain::getInstance()->addressMap.keys().at(0) << "totalSupply" << ""
                                                   ));

        UBChain::getInstance()->postRPC( "id_contract_call_offline_+state+" + contractAddress,
                                         toJsonFormat( "contract_call_offline", QStringList() << contractAddress
                                                   << UBChain::getInstance()->addressMap.keys().at(0) << "state" << ""
                                                   ));

        return;
    }

    if( id.startsWith( "id_contract_call_offline_+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            int pos = 26;
            QString func = id.mid( pos, id.indexOf("+" , pos) - pos);
            QString contractAddress = id.mid(id.indexOf("+" , pos) + 1);
            if( !UBChain::getInstance()->ERC20TokenInfoMap.contains(contractAddress))        return;

            if( func == "tokenName")
            {
                QString tokenName = result.mid(9);
                tokenName.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].name = tokenName;
            }
            else if( func == "precision")
            {
                QString precision = result.mid(9);
                precision.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].precision = precision.toDouble();
            }
            else if( func == "admin")
            {
                QString admin = result.mid(9);
                admin.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].admin = admin;
            }
            else if( func == "tokenSymbol")
            {
                QString tokenSymbol = result.mid(9);
                tokenSymbol.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].symbol = tokenSymbol;
            }
            else if( func == "totalSupply")
            {
                QString totalSupply = result.mid(9);
                totalSupply.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].totalSupply = totalSupply.toULongLong();
            }
            else if( func == "anchorTokenSymbol")
            {
                QString anchorTokenSymbol = result.mid(9);
                anchorTokenSymbol.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].anchorTokenSymbol = anchorTokenSymbol;
            }
            else if( func == "anchorTokenPrecision")
            {
                QString anchorTokenPrecision = result.mid(9);
                anchorTokenPrecision.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].anchorTokenPrecision = anchorTokenPrecision.toULongLong();
            }
            else if( func == "anchorRatio")
            {
                QString anchorRatio = result.mid(9);
                anchorRatio.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].anchorRatio = anchorRatio.toULongLong();
            }
            else if( func == "contractBalance")
            {
                QString contractBalance = result.mid(9);
                contractBalance.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].contractBalance = contractBalance.toULongLong();
            }
            else if( func == "lastTransferBlockNumber")
            {
                QString lastTransferBlockNumber = result.mid(9);
                lastTransferBlockNumber.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].lastTransferBlockNumber = lastTransferBlockNumber.toInt();
            }
            else if( func == "state")
            {
                QString state = result.mid(9);
                state.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].state = state;
            }
            else if( func == "query_can_mint")
            {
                QString amountCanMint = result.mid(9);
                amountCanMint.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].amountCanMint = amountCanMint.toULongLong();
            }
            else if( func == "base_mortgage_rate")
            {
                QString baseMortgageRate = result.mid(9);
                baseMortgageRate.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].baseMortgageRate = baseMortgageRate.toInt();
            }
            else if( func == "unusedBalance")
            {
                QString unusedBalance = result.mid(9);
                unusedBalance.remove('\"');
                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].unusedBalance = unusedBalance.toULongLong();
            }

        }

        return;
    }

    if( id.startsWith( "id_contract_call_offline_balanceOf+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        int pos = QString("id_contract_call_offline_balanceOf+").size();
        QString contractAddress = id.mid( pos, id.indexOf("+" , pos) - pos);
        QString accountAddress = id.mid(id.indexOf("+" , pos) + 1);

        if( result.startsWith("\"result\":"))
        {
            result = result.mid(9);
            result.remove('\"');
            double balance = result.toDouble();

            ContractBalanceMap map;
            if( UBChain::getInstance()->accountContractBalanceMap.contains(accountAddress))
            {
                map = UBChain::getInstance()->accountContractBalanceMap[accountAddress];
            }
            map.insert(contractAddress,balance);
            UBChain::getInstance()->accountContractBalanceMap.insert(accountAddress,map);
        }


        return;
    }

    if( id.startsWith( "id_blockchain_list_contract_transaction_history+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            int pos = QString("id_blockchain_list_contract_transaction_history+").size();
            QString contractAddress = id.mid( pos, id.indexOf("+" , pos) - pos);
            QString collectCount = id.mid(id.indexOf("+" , pos) + 1);
            if( !UBChain::getInstance()->ERC20TokenInfoMap.contains(contractAddress)) return;

            int collected = UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].collectedBlockHeight + collectCount.toInt();
            if(collected > UBChain::getInstance()->currentBlockHeight)   collected = UBChain::getInstance()->currentBlockHeight;
            qDebug() << contractAddress << collected;

            UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].collectedBlockHeight = collected;
            UBChain::getInstance()->setContractCollectedBlockHeight(contractAddress,UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].collectedBlockHeight);


            int fromBlockHeight = UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress).collectedBlockHeight + 1;
            if(fromBlockHeight > UBChain::getInstance()->currentBlockHeight)
            {
                UBChain::getInstance()->isCollecting = false;

                UBChain::getInstance()->startCollectContractTransactions();
            }
            else
            {
                UBChain::getInstance()->postRPC( "id_blockchain_list_contract_transaction_history+" + contractAddress + "+" + QString::number(ONCE_COLLECT_COUNT),
                                       toJsonFormat( "blockchain_list_contract_transaction_history",
                                       QStringList() << QString::number(fromBlockHeight) << QString::number(ONCE_COLLECT_COUNT)
                                       << contractAddress  << "14"  ));
                UBChain::getInstance()->isCollecting = true;
            }



//            UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].noResponseTimes = 0;
//            if(UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].interval < MAX_COLLECT_COUNT)
//            {
//                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].interval *= 10;
//                if(UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].interval > MAX_COLLECT_COUNT)
//                {
//                    UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].interval = MAX_COLLECT_COUNT;
//                }
//            }

            result.prepend("{");
            result.append("}");

            QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
            QByteArray ba = utfCodec->fromUnicode(result);

            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);
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
                            QJsonArray resultArray = resultValue.toArray();
                            for( int i = 0; i < resultArray.size(); i++)
                            {
                                QJsonObject object          = resultArray.at(i).toObject();
                                QString trxId               = object.take("result_trx_id").toString();

                                UBChain::getInstance()->postRPC( "id_blockchain_get_pretty_contract_transaction+" + trxId,
                                                                 toJsonFormat( "blockchain_get_pretty_contract_transaction",
                                                                             QStringList() << trxId
                                                                             ));

                            }
                        }
                    }
                }

            }
        }
        return;
    }

    if( id.startsWith( "id_blockchain_get_pretty_contract_transaction+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
            QByteArray ba = utfCodec->fromUnicode(result);

            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);
            if(json_error.error == QJsonParseError::NoError)
            {
                if( parse_doucment.isObject())
                {
                    QJsonObject jsonObject = parse_doucment.object();
                    if( jsonObject.contains("result"))
                    {
                        ContractTransaction contractTransaction;
                        QJsonObject object = jsonObject.take("result").toObject();
                        contractTransaction.trxId = object.take("result_trx_id").toString();
                        contractTransaction.blockNum = object.take("block_num").toInt();
                        contractTransaction.timeStamp   = object.take("timestamp").toString();

                        QJsonObject object2 = object.take("to_contract_ledger_entry").toObject();
                        contractTransaction.fromAddress = object2.take("from_account").toString();
                        contractTransaction.contractAddress = object2.take("to_account").toString();
                        QJsonValue feeValue = object2.take("fee").toObject().take("amount");
                        if( feeValue.isString())
                        {
                            contractTransaction.fee = feeValue.toString().toULongLong();
                        }
                        else
                        {
                            contractTransaction.fee = QString::number(feeValue.toDouble(),'g',10).toULongLong();
                        }

                        QJsonArray array = object.take("reserved").toArray();
                        contractTransaction.method = array.at(0).toString();
                        QStringList params = array.at(1).toString().split(",");

                        if( contractTransaction.method == "transfer")
                        {
                            if(params.size() != 2)  return;
                            contractTransaction.toAddress = params.at(0);
                            contractTransaction.amount = params.at(1).toULongLong();
                        }
                        else if( contractTransaction.method == "init_token")
                        {
                            if(params.size() == 4)          //  无抵押合约  有4个变量 第3个是totalsupply
                            {
                                contractTransaction.amount = params.at(2).toULongLong();
                            }
                            else if(params.size() == 5)     //  有抵押合约  有5个变量 第2个是totalsupply
                            {
                                contractTransaction.amount = params.at(1).toULongLong();
                            }
                            else
                            {
                                return;
                            }
                        }
                        else if( contractTransaction.method == "mint")
                        {
                            if(params.size() != 1)  return;
                            contractTransaction.amount = params.at(0).toULongLong();
                        }
                        else if( contractTransaction.method == "destroy")
                        {
                            if(params.size() != 1)  return;
                            contractTransaction.amount = params.at(0).toULongLong();
                        }
                        else if( contractTransaction.method == "retire")
                        {
                        }
                        else
                        {
                            return;
                        }

                        if( UBChain::getInstance()->isMyAddress(contractTransaction.fromAddress))
                        {
                            QString key = contractTransaction.fromAddress + "-" + contractTransaction.contractAddress;
                            if(UBChain::getInstance()->checkContractTransactionExist(key,contractTransaction.trxId))    return;
                            UBChain::getInstance()->accountContractTransactionMap[key].transactionVector.append(contractTransaction);
                        }

                        if( UBChain::getInstance()->isMyAddress(contractTransaction.toAddress) && contractTransaction.fromAddress != contractTransaction.toAddress)
                        {
                            QString key = contractTransaction.toAddress + "-" + contractTransaction.contractAddress;
                            if(UBChain::getInstance()->checkContractTransactionExist(key,contractTransaction.trxId))    return;
                            UBChain::getInstance()->accountContractTransactionMap[key].transactionVector.append(contractTransaction);
                        }

                    }
                }

            }

        }
        return;
    }

    if( id == "id_wallet_get_transaction_fee" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            int pos = result.indexOf("\"amount\":") + 9;
            QString amount = result.mid(pos, result.indexOf(",", pos) - pos);
            amount.remove("\"");

            UBChain::getInstance()->transactionFee = amount.toULongLong();

            if( currentPageNum == 3 && transferPage != NULL)
            {
                transferPage->updateTransactionFee();
            }
        }

        return;
    }

    if( id == QString("id_contract_call_offline_") + FEEDER_CONTRACT_ADDRESS + "_to_feed_tokens")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":"))
        {
            QString str = result.mid(9);
            str.remove("[");
            str.remove("]");
            str.remove("\"");
            str.remove("\\");
            UBChain::getInstance()->tokens = str.split(",");


            foreach (QString token, UBChain::getInstance()->tokens)
            {
                UBChain::getInstance()->postRPC( QString("id_contract_call_offline_") + FEEDER_CONTRACT_ADDRESS + "_price_of_tokens_" + token,
                                                 toJsonFormat( "contract_call_offline",
                                                               QStringList() << FEEDER_CONTRACT_ADDRESS << UBChain::getInstance()->addressMap.keys().at(0)
                                                               << "price_of_token" << token));
            }
        }

        return;
    }

    if( id.startsWith( QString("id_contract_call_offline_") + FEEDER_CONTRACT_ADDRESS + "_price_of_tokens_") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        QString token = id.mid(42 + QString(FEEDER_CONTRACT_ADDRESS).size());

        if(result.startsWith("\"result\":"))
        {
            QString str = result.mid(9);
            str.remove("\"");
            UBChain::getInstance()->tokenPriceMap.insert(token,str);

        }

        return;
    }

    if( id == QString("id_contract_call_offline_") + FEEDER_CONTRACT_ADDRESS + "_feeders")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":"))
        {
            QString str = result.mid(9);
            str.remove("[");
            str.remove("]");
            str.remove("\"");
            str.remove("\\");
            UBChain::getInstance()->feeders = str.split(",");
        }

        return;
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
     connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
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
    

    UBChain::getInstance()->postRPC( "id_blockchain_list_pending_transactions", toJsonFormat( "blockchain_list_pending_transactions", QStringList() << "" ));

    UBChain::getInstance()->postRPC(  "id_wallet_set_transaction_scanning", toJsonFormat( "wallet_set_transaction_scanning", QStringList() << "true" ) );

    UBChain::getInstance()->postRPC(  "id_delegate_set_network_min_connection_count", toJsonFormat( "delegate_set_network_min_connection_count", QStringList() << "0" ) );

    UBChain::getInstance()->postRPC(  "id_wallet_delegate_set_block_production", toJsonFormat( "wallet_delegate_set_block_production", QStringList() << "ALL" << "false" ) );

    UBChain::getInstance()->postRPC( "id_blockchain_list_assets", toJsonFormat( "blockchain_list_assets", QStringList() << ""));

//    UBChain::getInstance()->postRPC( "id_delegate_set_transaction_min_fee", toJsonFormat( "delegate_set_transaction_min_fee", QStringList() << "10"));

    UBChain::getInstance()->postRPC( "id_wallet_get_transaction_fee", toJsonFormat( "wallet_get_transaction_fee", QStringList() << "" ));


    
}

void Frame::startTimerForCollectContractTransaction()
{
    timerForCollectContractTransaction = new QTimer(this);
    connect(timerForCollectContractTransaction,SIGNAL(timeout()),this,SLOT(onTimerForCollectContractTransaction()));
    timerForCollectContractTransaction->start(COLLECT_CONTRACT_TRANSACTION_INTERVAL);
}

void Frame::paintEvent(QPaintEvent *e)
{

//        QStyleOption opt;

//        opt.init(this);
//        QPainter p(this);
//        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//        QWidget::paintEvent(e);

}

void Frame::onTimerForCollectContractTransaction()
{
    UBChain::getInstance()->collectContracts();
}

void Frame::scan()
{
    UBChain::getInstance()->postRPC( "id_scan", toJsonFormat( "scan", QStringList() << "0"));
}

void Frame::newAccount(QString name)
{
//    UBChain::getInstance()->postRPC( toJsonFormat( "id_wallet_get_account_public_address-" + name, "wallet_get_account_public_address", QStringList() << name));

    getAccountInfo();
}

void Frame::updateAssets()
{

//    if( assetPage != NULL)
//    {
//        assetPage->updateAssetInfo();
//        assetPage->updateMyAsset();
//        return;
//    }
}

