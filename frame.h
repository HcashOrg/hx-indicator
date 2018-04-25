#ifndef FRAME_H
#define FRAME_H
#include <QFrame>
#include <QWidget>
#include <QMap>
#include <QTranslator>
#include <QSystemTrayIcon>
#ifdef WIN32
#include "windows.h"
#endif

namespace Ui {
   class Frame;
}

class FirstLogin;
class NormalLogin;
class MainPage;
class AccountPage;
class TransferPage;
class BottomBar;
class LockPage;
class TitleBar;
class QMenu;
class WaitingForSync;
class FunctionBar;
class ContactPage;
class UpgradePage;
class ApplyDelegatePage;
class SelectWalletPathWidget;
class ShowBottomBarWidget;
class ShadowWidget;
class SmartContractPage;
class MultiSigPage;
class MultiSigTransactionPage;
class NewOrImportWalletWidget;

class ContactWidget;
class FunctionWidget;
class MinerPage;
class MyExchangeContractPage;

class Frame:public QFrame
{
    Q_OBJECT
public:
    Frame();
    ~Frame();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

public slots:
    void refreshAccountInfo();
    void autoRefresh();
    void shadowWidgetShow();
    void shadowWidgetHide();
    void setLanguage(QString);
    void syncFinished();

    void setCurrentAccount(QString accountName);

signals:
    void delegateListUpdated();
    void titleBackVisible(bool);
private slots:
    void alreadyLogin();
    void showAccountPage();
    void showAccountPage(QString);

    void showMinerPage();

    void showLockPage();
    void autoLock();
    void unlock();
    void updateTimer();
    void settingSaved();
    void privateKeyImported();

    void jsonDataUpdated(QString id);

    void showMainPage();

    void showTransferPage();
    void showTransferPage(QString);
    void showTransferPageWithAddress(QString, QString remark = "");

    void showContactPage();
    void showMultiSigPage();
    void showPoundagePage();//显示手续费承税单--朱正天
    void showMyExchangeContractPage();
    void showMultiSigTransactionPage(QString _multiSigAddress);
    void showWaittingForSyncWidget();
    void showNewOrImportWalletWidget();

    void iconIsActived(QSystemTrayIcon::ActivationReason reason);
    void showNormalAndActive();

    void newAccount(QString name);

private:
    bool mouse_press;
    QPoint move_point;
    SelectWalletPathWidget*   selectWalletPathWidget;
    FirstLogin* firstLogin;
    NormalLogin* normalLogin;

    MainPage*   mainPage;
    AccountPage* accountPage;
    TransferPage* transferPage;
    MultiSigPage* multiSigPage;
    MultiSigTransactionPage* multiSigTransactionPage;
    MinerPage* minerPage;
    MyExchangeContractPage* myExchangeContractPage;


    BottomBar* bottomBar;
    QWidget* centralWidget;
    LockPage*  lockPage;
    QTimer* timer;
    TitleBar* titleBar;
    QString lastPage;
    QString currentAccount;
    WaitingForSync* waitingForSync;
    NewOrImportWalletWidget* newOrImportWalletWidget;

    int currentPageNum;  //  0:mainPage   1:accountPage  2:delegatePgae  3:transferPage    4:contactPage    5:myExchangeContractPage
                         //   6: feedPage   7:minerPage   8: SmartContract     9: multiSigPage   10: multiSigTransactionPage
    ShadowWidget* shadowWidget;
    QSystemTrayIcon* trayIcon;
    void createTrayIconActions();
    void createTrayIcon();
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QMenu *trayIconMenu;
//    ShowBottomBarWidget* showBottomBarWidget;

#ifdef WIN32
    RECT rtConfined;   // 由于定义了 framelesswindowhint 为了不让鼠标拖动时能移到任务栏下
    RECT rtDefault;
#endif

    void  getAccountInfo();
    void startTimerForAutoRefresh();      // 自动刷新
    QTimer* timerForAutoRefresh;
    //FunctionBar* functionBar;
    FunctionWidget *functionBar;
    void closeCurrentPage();
    bool eventFilter(QObject *watched, QEvent *e);
    void closeEvent(QCloseEvent* e);
    void init();

    //ContactPage* contactPage;
    ContactWidget *contactPage;
    UpgradePage* upgradePage;

    QTranslator translator;         //  选择语言
    QTranslator menuTranslator;     //  右键菜单语言
    QTranslator translatorForTextBrowser;   // QTextBrowser的右键菜单翻译

    bool needToRefresh;

private:
    void paintEvent(QPaintEvent* e);
    QPixmap _pixmap;
private slots:
    void EnlargeRightPart();
    void RestoreRightPart();
};


#endif // FRAME_H
