#ifndef FUNCTIONWIDGET_H
#define FUNCTIONWIDGET_H

#include <QWidget>
#include <QEvent>
//////////////////////////////////////////////////////////////////////////
///<summary>左侧工具栏 </summary>
///
///<remarks> 2018.04.02 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class FunctionWidget;
}

class FunctionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionWidget(QWidget *parent = 0);
    ~FunctionWidget();
public:
    void retranslator();
signals:
    //兼容以前的信号---将来需要删除
    //void showMainPage();
    //void showAccountPage();
    //void showSmartContractPage();
    //void showTransferPage();
    //void showContactPage();
    //void showFeedPage();
    //void showMultiSigPage();

    void lock();
public slots:
    void contactShowTransferPageSlots();//额外槽函数，接受联系人界面转账点击信号，functionbar恢复到账户页面
    void DefaultShow();
signals:
    //点击大按钮的信号
    void showAccountSignal();
    void showContactSignal();
    void showAdvanceSignal();
    void showExchangeSignal();
    void showGuardSignal();
    void showCitizenSignal();
    //点击二级按钮的信号
    void showPoundageSignal();
    void showMultiSigSignal();
    void showNameTransferSignal();
    void showMyExchangeContractSignal();
    void showMinerSignal();
    void showBonusSignal();
    void showExchangeModeSignal();
    void showOnchainOrderSignal();
    void showMyOrderSignal();
    void showContractTokenSignal();
    void showGuardAccountSignal();
    void showGuardIncomeSignal();
    void showLockContractSignal();
    void showAssetSignal();
    void showKeyManageSignal();
    void showProposalSignal();
    void showWithdrawConfirmSignal();
    void showFeedPriceSignal();
    void showColdHotTransferSignal();
    void showCitizenAccountSignal();
    void showCitizenProposalSignal();
    void showCitizenPolicySignal();
signals:
    void ShrinkSignal();
    void RestoreSignal();

    //默认打开二级菜单对应界面信号
    void AccountDefaultSignal();
    void AdvanceDefaultSignal();
    void ExchangeDefaultSignal();
    void GuardDefaultSignal();
    void CitizenDefaultSignal();
private slots:
    void ShowContactWidgetSlots();
    void ShowAccountWidgetSlots();
    void ShowAdvanceWidgetSlots();
    void ShowExchangeWidgetSlots();
    void ShowGuardWidgetSlots();
    void ShowCitizenWidgetSlots();
    void ShowMoreWidgetSlots();

    void ShowSettingWidgetSlots();
    void ShowConsoleWidgetSlots();
    void ShowAboutWidgetSlots();
    void ShowUpdateWidgetSlots();

private:
    void updateCheckState(int buttonNumber);
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::FunctionWidget *ui;
private:
    class FunctionWidgetPrivate;
    FunctionWidgetPrivate *_p;
protected:

    virtual bool eventFilter(QObject *watched,QEvent *e);
    void paintEvent(QPaintEvent*);
};

#endif // FUNCTIONWIDGET_H
