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

#include "extra/HttpManager.h"

//namespace Ui {
//   class Frame;
//}

class FirstLogin;
class NormalLogin;
class MainPage;
class TransferPage;
class BottomBar;
class LockPage;
class TitleBar;
class QMenu;
class WaitingForSync;
class UpgradePage;
class ApplyDelegatePage;
class SelectWalletPathWidget;
class LightModeConfig;
class ShadowWidget;
class SmartContractPage;
class MultiSigPage;
class NewOrImportWalletWidget;
class ExitingWidget;

class DepositAutomatic;
class ContactWidget;
class FunctionWidget;
class MinerPage;
class BonusPage;
class OnchainOrderPage;
class ExchangeModePage;
class MyExchangeContractPage;
class PoundageWidget;
class GuardAccountPage;
class GuardIncomePage;
class LockContractPage;
class AssetPage;
class GuardKeyManagePage;
class ProposalPage;
class WithdrawConfirmPage;
class FeedPricePage;
class ColdHotTransferPage;
class ContractTokenPage;
class CitizenAccountPage;
class CitizenProposalPage;
class CitizenPolicyPage;
class NameTransferPage;
class CoverWidget;

class CrossCapitalMark;
class CustomShadowEffect;
class AutoUpdateNotify;
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
    void autoRefresh();
    void shadowWidgetShow();
    void shadowWidgetHide();
    void setLanguage(QString);
    void syncFinished();

    void setCurrentAccount(QString accountName);

    void ShowMainPageSlot();

    void onCloseWallet();

    void extendToWidth(int _width);
    void hideKLineWidget();
signals:
    void delegateListUpdated();
    void titleBackVisible(bool);
private slots:
    void alreadyLogin();

    void showMinerPage();
    void showBonusPage();

    void showLockPage();
    void autoLock();
    void unlock();
    void updateTimer();
    void settingSaved();
    void privateKeyImported();

    void jsonDataUpdated(QString id);

    void onBack();
    void showMainPage();

    void showTransferPage();
    void showTransferPage(QString,QString);
    void showTransferPageWithAddress(QString, QString remark = "");//对方地址、名称

    void showContactPage();
    void showMultiSigPage();
    void showPoundagePage();//显示手续费承税单--朱正天
    void showNameTransferPage();
    void showOnchainOrderPage();
    void showExchangeModePage();
    void showMyExchangeContractPage();
    void showGuardAccountPage();
    void showGuardIncomePage();
    void showLockContractPage();
    void showAssetPage();
    void showKeyManagePage();
    void showProposalPage();
    void showWithdrawConfirmPage();
    void showFeedPricePage();
    void showColdHotTransferPage();
    void showContractTokenPage();
    void showCitizenAccountPage();
    void showCitizenProposalPage();
    void showCitizenPolicyPage();

    void showWaittingForSyncWidget();
    void showNewOrImportWalletWidget();


    void iconIsActived(QSystemTrayIcon::ActivationReason reason);
    void showNormalAndActive();

    void newAccount(QString name);

private:
    HttpManager httpManager;////用于查询通道账户余额
private slots:
    void enter();
    void httpReplied(QByteArray _data, int _status);
    void httpError(int _status);

private slots:
    //显示通知气泡
    void ShowBubbleMessage(const QString &title,const QString &context,QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int msecs = 10000);
private:
    bool mouse_press;
    QPoint move_point;
    SelectWalletPathWidget*   selectWalletPathWidget;
    FirstLogin* firstLogin;
    NormalLogin* normalLogin;
    LightModeConfig* lightModeConfig = nullptr;

    MainPage*   mainPage;
    TransferPage* transferPage;
    MultiSigPage* multiSigPage;
    MinerPage* minerPage;
    BonusPage* bonusPage;
    OnchainOrderPage* onchainOrderPage = nullptr;
    ExchangeModePage* exchangeModePage = nullptr;
    MyExchangeContractPage* myExchangeContractPage = nullptr;
    PoundageWidget *poundage;
    GuardAccountPage*   guardAccountPage;
    GuardIncomePage*    guardIncomePage;
    LockContractPage*   lockContractPage;
    AssetPage*          assetPage;
    GuardKeyManagePage* guardKeyManagePage;
    ProposalPage*       proposalPage;
    WithdrawConfirmPage* withdrawConfirmPage;
    FeedPricePage*      feedPricePage;
    ColdHotTransferPage* coldHotTransferPage;
    ContractTokenPage*  contractTokenPage;
    CitizenAccountPage* citizenAccountPage;
    CitizenProposalPage* citizenProposalPage;
    CitizenPolicyPage*  citizenPolicyPage = nullptr;
    NameTransferPage*   nameTransferPage = nullptr;

    BottomBar* bottomBar;
    QWidget* centralWidget;
    LockPage*  lockPage;
    QTimer* timer;
    TitleBar* titleBar;
    QString currentAccount;
    WaitingForSync* waitingForSync;
    NewOrImportWalletWidget* newOrImportWalletWidget;

    int currentPageNum;  //  0:mainPage    3:transferPage    4:contactPage    5:myExchangeContractPage
                         //   6: onchainOrderPage   7:minerPage   8: assetPage     9: multiSigPage
                         //  11: poundage   12: guardKeyManagePage  13: proposalPage    14: withdrawConfirmPage  15: feedPricePage
                        //   16: coldHotTransferPage    17: guardAccountPage    18: guardIncomePage     19: bonusPage
                        //   20: contractTokenPage  21: citizenAccountPage  22: citizenProposalPage  23: lockContractPage
                        //   24: exchangeModePage   25: nameTransferPage    26: citizenPolicyPage
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

    int getInfoCount = 0;
    void  getAccountInfo();
    void startTimerForAutoRefresh();      // 自动刷新
    QTimer* timerForAutoRefresh;
    FunctionWidget *functionBar;
    QWidget* topLeftWidget = NULL;
    void closeCurrentPage();
    bool eventFilter(QObject *watched, QEvent *e);
    void closeEvent(QCloseEvent* e);
    void init();

    ContactWidget *contactPage;

    QTranslator translator;         //  选择语言
    QTranslator menuTranslator;     //  右键菜单语言
    QTranslator translatorForTextBrowser;   // QTextBrowser的右键菜单翻译

    ExitingWidget* exitingWidget = NULL;

private:
    void paintEvent(QPaintEvent* e);
private slots:
    void EnlargeRightPart();
    void RestoreRightPart();

private:
    DepositAutomatic *autoDeposit;
public:
    CrossCapitalMark *crossMark;
    AutoUpdateNotify *autoupdate;//自动更新
public:
    void installBlurEffect(QWidget *widget);

private:
    CoverWidget* coverWidget = nullptr;
private slots:
    void showCoverWidget();
};


#endif // FRAME_H
