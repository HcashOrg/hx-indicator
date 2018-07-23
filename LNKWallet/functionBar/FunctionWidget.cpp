#include "FunctionWidget.h"
#include "ui_FunctionWidget.h"

#include <QMenu>

#include "functionBar/FunctionAccountWidget.h"
#include "functionBar/FunctionAdvanceWidget.h"
#include "functionBar/FunctionExchangeWidget.h"
#include "functionBar/FunctionGuardWidget.h"

#include "setdialog.h"
#include "consoledialog.h"
#include "wallet.h"

class FunctionWidget::FunctionWidgetPrivate
{
public:
    FunctionWidgetPrivate()
        :accountBar(new FunctionAccountWidget())
        ,advanceBar(new FunctionAdvanceWidget())
        ,exchangeBar(new FunctionExchangeWidget())
        ,guardBar(new FunctionGuardWidget())
        ,contextMenu(new QMenu())
    {

    }
public:
    FunctionAccountWidget *accountBar;
    FunctionAdvanceWidget *advanceBar;
    FunctionExchangeWidget *exchangeBar;
    FunctionGuardWidget   *guardBar;
    QMenu *contextMenu;
};

FunctionWidget::FunctionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionWidget),
    _p(new FunctionWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

FunctionWidget::~FunctionWidget()
{
    delete _p;
    delete ui;
}

void FunctionWidget::retranslator()
{
    ui->retranslateUi(this);
    _p->accountBar->retranslator();
    _p->advanceBar->retranslator();
    _p->exchangeBar->retranslator();
    _p->guardBar->retranslator();
}

void FunctionWidget::contactShowTransferPageSlots()
{
    ShowAccountWidgetSlots();
}

void FunctionWidget::DefaultShow()
{
    ShowAccountWidgetSlots();
}

void FunctionWidget::ShowContactWidgetSlots()
{
    ui->stackedWidget->setVisible(false);

    emit showContactSignal();
    //emit showContactPage();
    emit ShrinkSignal();

    updateCheckState(1);
}

void FunctionWidget::ShowAccountWidgetSlots()
{
    ui->stackedWidget->setCurrentWidget(_p->accountBar);
    emit showAccountSignal();
    //emit showAccountPage();

    ui->stackedWidget->setVisible(true);
    emit RestoreSignal();

    updateCheckState(0);

    emit AccountDefaultSignal();
}

void FunctionWidget::ShowAdvanceWidgetSlots()
{
    ui->stackedWidget->setCurrentWidget(_p->advanceBar);

    emit showAdvanceSignal();

    ui->stackedWidget->setVisible(true);
    emit RestoreSignal();

    updateCheckState(2);
    emit AdvanceDefaultSignal();
}

void FunctionWidget::ShowExchangeWidgetSlots()
{
    ui->stackedWidget->setCurrentWidget(_p->exchangeBar);

    emit showExchangeSignal();

    ui->stackedWidget->setVisible(true);
    emit RestoreSignal();

    updateCheckState(3);
    emit ExchangeDefaultSignal();
}

void FunctionWidget::ShowGuardWidgetSlots()
{
    ui->stackedWidget->setCurrentWidget(_p->guardBar);

    emit showGuardSignal();

    ui->stackedWidget->setVisible(true);
    emit RestoreSignal();

    updateCheckState(4);
    emit GuardDefaultSignal();
}

void FunctionWidget::ShowMoreWidgetSlots()
{
    _p->contextMenu->clear();

    //设置更多按钮
    QAction* actionSet = _p->contextMenu->addAction(tr("Setting"));
    _p->contextMenu->addSeparator();
    QAction* actionLock = _p->contextMenu->addAction(tr("Lock"));
    _p->contextMenu->addSeparator();
    QAction* actionConsole = _p->contextMenu->addAction(tr("Console"));
    _p->contextMenu->addSeparator();
    QMenu *helpMenu = new QMenu(tr("Help"));
    helpMenu->setStyleSheet(MENU_STYLE);
    helpMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    helpMenu->setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | helpMenu->windowFlags());

    _p->contextMenu->addMenu(helpMenu);
    QAction* actionAbout = helpMenu->addAction(tr("About Us"));
    helpMenu->addSeparator();
    QAction* actionUpdate = helpMenu->addAction(tr("Update"));

    connect(actionLock,&QAction::triggered,this,&FunctionWidget::lock);
    connect(actionSet,&QAction::triggered,this,&FunctionWidget::ShowSettingWidgetSlots);
    connect(actionConsole,&QAction::triggered,this,&FunctionWidget::ShowConsoleWidgetSlots);
    connect(actionAbout,&QAction::triggered,this,&FunctionWidget::ShowAboutWidgetSlots);
    connect(actionUpdate,&QAction::triggered,this,&FunctionWidget::ShowUpdateWidgetSlots);
    _p->contextMenu->show();
    _p->contextMenu->close();



    _p->contextMenu->exec(mapToGlobal(QPoint(70,height()-_p->contextMenu->height())));


    //updateCheckState(4);
}

void FunctionWidget::ShowSettingWidgetSlots()
{
    SetDialog setDialog;
    connect(&setDialog,SIGNAL(settingSaved()),HXChain::getInstance()->mainFrame,SLOT(settingSaved()));
    setDialog.pop();
}

void FunctionWidget::ShowConsoleWidgetSlots()
{
    ConsoleDialog consoleDialog /*= new ConsoleDialog()*/;
    consoleDialog.pop();
}

void FunctionWidget::ShowAboutWidgetSlots()
{
    SetDialog setDialog;
    setDialog.setHelpFirst(true);
    connect(&setDialog,SIGNAL(settingSaved()),HXChain::getInstance()->mainFrame,SLOT(settingSaved()));
    setDialog.pop();

}

void FunctionWidget::ShowUpdateWidgetSlots()
{
    SetDialog setDialog;
    setDialog.setHelpFirst(true);
    connect(&setDialog,SIGNAL(settingSaved()),HXChain::getInstance()->mainFrame,SLOT(settingSaved()));
    setDialog.pop();

}

void FunctionWidget::updateCheckState(int buttonNumber)
{
    ui->toolButton_account->setChecked(0 == buttonNumber);
    ui->toolButton_contact->setChecked(1 == buttonNumber);
    ui->toolButton_advanced->setChecked(2 == buttonNumber);
    ui->toolButton_exchange->setChecked(3 == buttonNumber);
    ui->toolButton_guard->setChecked(4 == buttonNumber);
    ui->toolButton_more->setChecked(5 == buttonNumber);

    ui->toolButton_account->setToolButtonStyle( Qt::ToolButtonIconOnly );
    ui->toolButton_contact->setToolButtonStyle(Qt::ToolButtonIconOnly );
    ui->toolButton_advanced->setToolButtonStyle( Qt::ToolButtonIconOnly );
    ui->toolButton_exchange->setToolButtonStyle( Qt::ToolButtonIconOnly );
    ui->toolButton_guard->setToolButtonStyle( Qt::ToolButtonIconOnly );
    ui->toolButton_more->setToolButtonStyle(Qt::ToolButtonIconOnly );

}

void FunctionWidget::InitWidget()
{
    InitStyle();

    ui->toolButton_contact->setCheckable(true);
    ui->toolButton_account->setCheckable(true);
    ui->toolButton_advanced->setCheckable(true);
    ui->toolButton_exchange->setCheckable(true);
    ui->toolButton_guard->setCheckable(true);
    ui->toolButton_more->setCheckable(false);

    ui->stackedWidget->addWidget(_p->accountBar);
    ui->stackedWidget->addWidget(_p->advanceBar);
    ui->stackedWidget->addWidget(_p->exchangeBar);
    ui->stackedWidget->addWidget(_p->guardBar);

    //链接信号槽
    connect(ui->toolButton_contact,&QToolButton::clicked,this,&FunctionWidget::ShowContactWidgetSlots);
    connect(ui->toolButton_account,&QToolButton::clicked,this,&FunctionWidget::ShowAccountWidgetSlots);
    connect(ui->toolButton_advanced,&QToolButton::clicked,this,&FunctionWidget::ShowAdvanceWidgetSlots);
    connect(ui->toolButton_exchange,&QToolButton::clicked,this,&FunctionWidget::ShowExchangeWidgetSlots);
    connect(ui->toolButton_guard,&QToolButton::clicked,this,&FunctionWidget::ShowGuardWidgetSlots);
    connect(ui->toolButton_more,&QToolButton::clicked,this,&FunctionWidget::ShowMoreWidgetSlots);



    connect(_p->accountBar,&FunctionAccountWidget::showAccountSignal,this,&FunctionWidget::showAccountSignal);
    connect(_p->accountBar,&FunctionAccountWidget::showMinerSignal,this,&FunctionWidget::showMinerSignal);
    connect(_p->accountBar,&FunctionAccountWidget::showBonusSignal,this,&FunctionWidget::showBonusSignal);

    connect(_p->advanceBar,&FunctionAdvanceWidget::showPoundageSignal,this,&FunctionWidget::showPoundageSignal);
    connect(_p->advanceBar,&FunctionAdvanceWidget::showMultiSigSignal,this,&FunctionWidget::showMultiSigSignal);

    connect(_p->exchangeBar,&FunctionExchangeWidget::showOnchainOrderSignal,this,&FunctionWidget::showOnchainOrderSignal);
    connect(_p->exchangeBar,&FunctionExchangeWidget::showMyOrderSignal,this,&FunctionWidget::showMyOrderSignal);

    connect(_p->guardBar,&FunctionGuardWidget::showGuardAccountSignal,this,&FunctionWidget::showGuardAccountSignal);
    connect(_p->guardBar,&FunctionGuardWidget::showGuardIncomeSignal,this,&FunctionWidget::showGuardIncomeSignal);
    connect(_p->guardBar,&FunctionGuardWidget::showAssetSignal,this,&FunctionWidget::showAssetSignal);
    connect(_p->guardBar,&FunctionGuardWidget::showKeyManageSignal,this,&FunctionWidget::showKeyManageSignal);
    connect(_p->guardBar,&FunctionGuardWidget::showProposalSignal,this,&FunctionWidget::showProposalSignal);
    connect(_p->guardBar,&FunctionGuardWidget::showWithdrawConfirmSignal,this,&FunctionWidget::showWithdrawConfirmSignal);
    connect(_p->guardBar,&FunctionGuardWidget::showFeedPriceSignal,this,&FunctionWidget::showFeedPriceSignal);
    connect(_p->guardBar,&FunctionGuardWidget::showColdHotTransferSignal,this,&FunctionWidget::showColdHotTransferSignal);

    //链接二级菜单默认单击情况
    connect(this,&FunctionWidget::AccountDefaultSignal,_p->accountBar,&FunctionAccountWidget::DefaultShow);
    connect(this,&FunctionWidget::AdvanceDefaultSignal,_p->advanceBar,&FunctionAdvanceWidget::DefaultShow);
    connect(this,&FunctionWidget::ExchangeDefaultSignal,_p->exchangeBar,&FunctionExchangeWidget::DefaultShow);
    connect(this,&FunctionWidget::GuardDefaultSignal,_p->guardBar,&FunctionGuardWidget::DefaultShow);
    ui->toolButton_account->installEventFilter(this);
    ui->toolButton_advanced->installEventFilter(this);
    ui->toolButton_contact->installEventFilter(this);
    ui->toolButton_exchange->installEventFilter(this);
    ui->toolButton_guard->installEventFilter(this);
    ui->toolButton_more->installEventFilter(this);
}

void FunctionWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(72,97,220));
    setPalette(palette);

    setStyleSheet("\
    QToolButton{padding:4px 0px;background: rgb(72,97,220);border:none;color:white;font:65 12px \"Microsoft YaHei UI \";}\
    QToolButton:hover{background-color: rgb(84,116, 235);}\
    QToolButton:pressed{background-color: rgb(84,116, 235);}\
    QToolButton:checked{border-left:2px solid rgb(0,210,255);background-color: rgb(84,116, 235);}\
                  "

                  );
    _p->contextMenu->setAttribute(Qt::WA_TranslucentBackground);
    _p->contextMenu->setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | _p->contextMenu->windowFlags() );
    //_p->contextMenu->setMinimumWidth(88);
    _p->contextMenu->setStyleSheet("QMenu {border-radius:10px;width:88px;border-bottom-left-radius:0px;background-color:rgba(238,241,253,235);border: 0px solid red;}"\
                "QMenu::item {border-radius:10px;border: 0px solid green;background-color:transparent;padding:5px 21px;}"\
                "QMenu::item:selected {background-color:rgb(130,157,255);}"\
                "QMenu::separator {height: 2px;background-color: #FCFCFC;}"\
                "QMenu::right-arrow {padding:0px 10px;image:url(:/wallet_ui/right.png);}");


    ui->toolButton_account->setIconSize(QSize(26,26));
    ui->toolButton_advanced->setIconSize(QSize(26,26));
    ui->toolButton_contact->setIconSize(QSize(26,26));
    ui->toolButton_exchange->setIconSize(QSize(26,26));
    ui->toolButton_guard->setIconSize(QSize(26,26));
    ui->toolButton_more->setIconSize(QSize(26,26));

    ui->toolButton_account->setIcon(QIcon(":/functionBar/account.png"));
    ui->toolButton_advanced->setIcon(QIcon(":/functionBar/advance.png"));
    ui->toolButton_contact->setIcon(QIcon(":/functionBar/contact.png"));
    ui->toolButton_exchange->setIcon(QIcon(":/functionBar/exchange.png"));
    ui->toolButton_guard->setIcon(QIcon(":/functionBar/guard.png"));
    ui->toolButton_more->setIcon(QIcon(":/functionBar/more.png"));
}

bool FunctionWidget::eventFilter(QObject *watched,QEvent *e)
{

    if((watched == ui->toolButton_contact ||watched == ui->toolButton_account
        ||watched == ui->toolButton_advanced ||watched == ui->toolButton_exchange
        ||watched == ui->toolButton_guard
        ||watched == ui->toolButton_more)&& e->type() == QEvent::Enter)
    {
        if(QToolButton *button = qobject_cast<QToolButton*>(watched))
        {
            if(!button->isChecked())
            {
                button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            }
        }
    }
    else if((watched == ui->toolButton_contact ||watched == ui->toolButton_account
             ||watched == ui->toolButton_advanced ||watched == ui->toolButton_exchange
             ||watched == ui->toolButton_guard
             ||watched == ui->toolButton_more) && e->type() == QEvent::Leave)
    {
        qobject_cast<QToolButton*>(watched)->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    return QWidget::eventFilter(watched,e);
}
