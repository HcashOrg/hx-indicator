
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
#include "wallet.h"
#include "miner/minerpage.h"

#include "waitingforsync.h"
#include <QDesktopWidget>

#include "functionbar.h"
#include "functionBar/FunctionWidget.h"
//#include "contactpage.h"
#include "contact/ContactWidget.h"
#include "poundage/PoundageWidget.h"
#include "selectwalletpathwidget.h"
#include "control/shadowwidget.h"
#include "control/showbottombarwidget.h"
#include "commondialog.h"
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
    multiSigPage(NULL),
    multiSigTransactionPage(NULL),
    minerPage(NULL),
    needToRefresh(false)
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

    setGeometry(0,0, 960, 580);
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
    setGeometry(0,0, 960, 580);
    moveWidgetToScreenCenter(this);

    titleBar = new TitleBar(this);
    titleBar->resize(960,50);
    titleBar->move(0,0);
    titleBar->show();

    connect(titleBar,SIGNAL(minimum()),this,SLOT(showMinimized()));
    connect(titleBar,SIGNAL(closeWallet()),qApp,SLOT(quit()));
    connect(titleBar,SIGNAL(tray()),this,SLOT(hide()));

    centralWidget = new QWidget(this);
    centralWidget->resize(770,530);
    centralWidget->move(190,50);
    centralWidget->show();

    bottomBar = new BottomBar(this);
    bottomBar->resize(160,40);
    bottomBar->move(800,540);
    bottomBar->raise();
    bottomBar->show();

//    showBottomBarWidget = new ShowBottomBarWidget(centralWidget);
//    showBottomBarWidget->setGeometry(0,525,827,20);
//    connect(showBottomBarWidget, SIGNAL(showBottomBar()), bottomBar, SLOT(dynamicShow()) );
//    showBottomBarWidget->show();

    functionBar = new FunctionWidget(this);
    functionBar->resize(190,530);
    functionBar->move(0,50);
    functionBar->show();
    connect(functionBar, SIGNAL(showMainPage()), this, SLOT( showMainPage()));
    //connect(functionBar, SIGNAL(showAccountPage()), this, SLOT( showAccountPage()));
    connect(functionBar, SIGNAL(showTransferPage()), this, SLOT( showTransferPage()));
    //connect(functionBar, SIGNAL(showContactPage()), this, SLOT( showContactPage()));
    //connect(functionBar, SIGNAL(showMultiSigPage()), this, SLOT(showMultiSigPage()));
    connect(functionBar,SIGNAL(lock()),this,SLOT(showLockPage()));
    connect(functionBar,SIGNAL(showMinerSignal()),this,SLOT(showMinerPage()));

    connect(functionBar,&FunctionWidget::showAccountSignal,this,&Frame::showMainPage);
    connect(functionBar,&FunctionWidget::showContactSignal,this,&Frame::showContactPage);
    connect(functionBar,&FunctionWidget::showAdvanceSignal,this,&Frame::showMainPage);
    connect(functionBar,&FunctionWidget::showMultiSigSignal,this,&Frame::showMultiSigPage);
    connect(functionBar,&FunctionWidget::showPoundageSignal,this,&Frame::showPoundagePage);
    connect(functionBar,&FunctionWidget::ShrinkSignal,this,&Frame::EnlargeRightPart);
    connect(functionBar,&FunctionWidget::RestoreSignal,this,&Frame::RestoreRightPart);
    getAccountInfo();

    mainPage = new MainPage(centralWidget);
    mainPage->resize(770,530);
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

    init();

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
    UBChain::getInstance()->postRPC( "id-list_my_accounts", toJsonFormat( "list_my_accounts", QJsonArray()));

//    UBChain::getInstance()->postRPC( "id_balance", toJsonFormat( "balance", QJsonArray()));
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

    //朱正天functionBar->choosePage(4);

}

void Frame::showMinerPage()
{
    closeCurrentPage();

    minerPage = new MinerPage(centralWidget);
    minerPage->setAttribute(Qt::WA_DeleteOnClose);
    minerPage->show();

    currentPageNum = 7;
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
    transferPage->show();

    currentPageNum = 3;
    //朱正天functionBar->choosePage(2);


}


void Frame::showLockPage()
{

    timer->stop();
    shadowWidgetShow();


    UBChain::getInstance()->postRPC( "id-lock", toJsonFormat( "lock", QJsonArray()));
qDebug() << "lock ";

}

void Frame::autoLock()
{

    timer->stop();

    UBChain::getInstance()->postRPC( "id-lock", toJsonFormat( "lock", QJsonArray() ));
qDebug() << "autolock ";

}

void Frame::unlock()
{
    setGeometry(0,0, 960, 580);
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
//    rpcThread->setWriteData( toJsonFormat( "id_open", "open", QJsonArray() << "wallet" ));
//    rpcThread->start();

//    UBChain::getInstance()->initWorkerThreadManager();

    if( UBChain::getInstance()->contactsFile == NULL)
    {
        UBChain::getInstance()->getContactsFile();
    }

    UBChain::getInstance()->postRPC("id-is_new", toJsonFormat( "is_new", QJsonArray()));
}

void Frame::setCurrentAccount(QString accountName)
{
    UBChain::getInstance()->currentAccount = accountName;
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
    case 7:
        minerPage->close();
        minerPage = NULL;
        break;
    case 8:
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

    bottomBar->refresh();
   //朱正天 functionBar->refresh();

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
    transferPage->show();
    currentPageNum = 3;
   //朱正天 functionBar->choosePage(2);
}


void Frame::showContactPage()
{
    closeCurrentPage();
    contactPage = new ContactWidget(centralWidget);

    //getAccountInfo();
    //contactPage = new ContactPage(centralWidget);
    //connect(contactPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    //connect(contactPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(contactPage,SIGNAL(gotoTransferPage(QString,QString)),this,SLOT(showTransferPageWithAddress(QString,QString)));
    //contactPage->setAttribute(Qt::WA_DeleteOnClose);
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
            break;
        case 7:
//            upgradePage->retranslator();
//            showUpgradePage(currentAccount);
            break;
        case 8:
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


void Frame::showMultiSigPage()
{
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
    PoundageWidget *poundage = new PoundageWidget(centralWidget);
    poundage->resize(centralWidget->size());
    poundage->show();
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
        QString result = UBChain::getInstance()->jsonDataValue(id);

        UBChain::getInstance()->parseAccountInfo();

        foreach (QString accountName, UBChain::getInstance()->accountInfoMap.keys())
        {
            UBChain::getInstance()->getAccountBalances(accountName);
        }

//        if( needToRefresh)
//        {
//            refresh();
//            needToRefresh = false;
//        }

        return;
    }

    if( id.startsWith("id-get_account_balances-") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

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

                UBChain::getInstance()->accountInfoMap[accountName].assetAmountMap.insert(assetAmount.assetId,assetAmount);
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

    if( id == "id-list_assets")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        UBChain::getInstance()->parseAssetInfo();

        return;
    }

    if( id == "id-list_miners")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonArray array = jsonObject.take("result").toArray();

        UBChain::getInstance()->minersVector.clear();
        foreach (QJsonValue v, array)
        {
            QJsonArray array2 = v.toArray();

            Miner miner;
            miner.name = array2.at(0).toString();
            miner.minerId = array2.at(1).toString();
            UBChain::getInstance()->minersVector.append(miner);
        }

        return;
    }

//    if( id == "id_wallet_get_transaction_fee" )
//    {
//        QString result = UBChain::getInstance()->jsonDataValue(id);

//        if( result.startsWith("\"result\":"))
//        {
//            int pos = result.indexOf("\"amount\":") + 9;
//            QString amount = result.mid(pos, result.indexOf(",", pos) - pos);
//            amount.remove("\"");

//            UBChain::getInstance()->transactionFee = amount.toULongLong();

//            if( currentPageNum == 3 && transferPage != NULL)
//            {
//                transferPage->updateTransactionFee();
//            }
//        }

//        return;
//    }
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
    UBChain::getInstance()->postRPC( "id-list_assets", toJsonFormat( "list_assets", QJsonArray() << "A" << "100"));

    UBChain::getInstance()->postRPC( "id-list_miners", toJsonFormat( "list_miners", QJsonArray() << "A" << "100"));
//    UBChain::getInstance()->postRPC( "id_wallet_get_transaction_fee", toJsonFormat( "wallet_get_transaction_fee", QJsonArray()));


    
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
    functionBar->resize(70,functionBar->height());

    centralWidget->resize(890,centralWidget->height());

    centralWidget->move(70,50);
//    if(QWidget *widget = centralWidget->childAt(10,10))
//    {
//        widget->resize(805,centralWidget->height());
//    }

    contactPage->resize(890,centralWidget->height());


    //bottomBar->resize(805,40);
    //bottomBar->move(95,516);
}

void Frame::RestoreRightPart()
{
    functionBar->resize(190,functionBar->height());

    centralWidget->move(190,50);
    if(QWidget *widget = centralWidget->childAt(10,10))
    {
        widget->resize(770,centralWidget->height());
    }
    centralWidget->resize(770,centralWidget->height());

    //bottomBar->resize(680,40);
    //bottomBar->move(220,516);
}

void Frame::newAccount(QString name)
{
//    UBChain::getInstance()->postRPC( toJsonFormat( "id_wallet_get_account_public_address-" + name, "wallet_get_account_public_address", QJsonArray() << name));

    getAccountInfo();
}

