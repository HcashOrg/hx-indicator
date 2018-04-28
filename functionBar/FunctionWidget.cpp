#include "FunctionWidget.h"
#include "ui_FunctionWidget.h"

#include <QMenu>

#include "functionBar/FunctionAccountWidget.h"
#include "functionBar/FunctionAdvanceWidget.h"
#include "functionBar/FunctionExchangeWidget.h"

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
        ,contextMenu(new QMenu())
    {

    }
public:
    FunctionAccountWidget *accountBar;
    FunctionAdvanceWidget *advanceBar;
    FunctionExchangeWidget *exchangeBar;
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
}

void FunctionWidget::contactShowTransferPageSlots()
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
}

void FunctionWidget::ShowAdvanceWidgetSlots()
{
    ui->stackedWidget->setCurrentWidget(_p->advanceBar);

    emit showAdvanceSignal();

    ui->stackedWidget->setVisible(true);
    emit RestoreSignal();

    updateCheckState(2);
}

void FunctionWidget::ShowExchangeWidgetSlots()
{
    ui->stackedWidget->setCurrentWidget(_p->exchangeBar);

    emit showExchangeSignal();

    ui->stackedWidget->setVisible(true);
    emit RestoreSignal();

    updateCheckState(3);
}

void FunctionWidget::ShowMoreWidgetSlots()
{
    _p->contextMenu->exec(mapToGlobal(QPoint(70,height()-136)));
    updateCheckState(4);
}

void FunctionWidget::ShowSettingWidgetSlots()
{
    SetDialog setDialog;
    connect(&setDialog,SIGNAL(settingSaved()),UBChain::getInstance()->mainFrame,SLOT(settingSaved()));
    setDialog.pop();
}

void FunctionWidget::ShowConsoleWidgetSlots()
{
    ConsoleDialog consoleDialog;
    consoleDialog.pop();
}

void FunctionWidget::ShowAboutWidgetSlots()
{
    SetDialog setDialog;
    setDialog.setHelpFirst(true);
    connect(&setDialog,SIGNAL(settingSaved()),UBChain::getInstance()->mainFrame,SLOT(settingSaved()));
    setDialog.pop();

}

void FunctionWidget::ShowUpdateWidgetSlots()
{
    SetDialog setDialog;
    setDialog.setHelpFirst(true);
    connect(&setDialog,SIGNAL(settingSaved()),UBChain::getInstance()->mainFrame,SLOT(settingSaved()));
    setDialog.pop();

}

void FunctionWidget::updateCheckState(int buttonNumber)
{
    ui->toolButton_account->setChecked(0 == buttonNumber);
    ui->toolButton_contact->setChecked(1 == buttonNumber);
    ui->toolButton_advanced->setChecked(2 == buttonNumber);
    ui->toolButton_exchange->setChecked(3 == buttonNumber);
    ui->toolButton_more->setChecked(4 == buttonNumber);
}

void FunctionWidget::InitWidget()
{
    InitStyle();

    ui->toolButton_contact->setCheckable(true);
    ui->toolButton_account->setCheckable(true);
    ui->toolButton_advanced->setCheckable(true);
    ui->toolButton_exchange->setCheckable(true);
    ui->toolButton_more->setCheckable(true);

    ui->stackedWidget->addWidget(_p->accountBar);
    ui->stackedWidget->addWidget(_p->advanceBar);
    ui->stackedWidget->addWidget(_p->exchangeBar);

    //设置更多按钮
    QAction* actionSet = _p->contextMenu->addAction("Setting");
    QAction* actionLock = _p->contextMenu->addAction("Lock");
    QAction* actionConsole = _p->contextMenu->addAction("Console");
    QMenu *helpMenu = new QMenu("Help",_p->contextMenu);
    _p->contextMenu->addMenu(helpMenu);
    QAction* actionAbout = helpMenu->addAction("About Us");
    QAction* actionUpdate = helpMenu->addAction("Update");

    //ui->toolButton_more->setMenu(_p->contextMenu);
    ////ui->toolButton_more->setArrowType(Qt::RightArrow);
    //ui->toolButton_more->setPopupMode(QToolButton::InstantPopup );

    //链接信号槽
    connect(ui->toolButton_contact,&QToolButton::clicked,this,&FunctionWidget::ShowContactWidgetSlots);
    connect(ui->toolButton_account,&QToolButton::clicked,this,&FunctionWidget::ShowAccountWidgetSlots);
    connect(ui->toolButton_advanced,&QToolButton::clicked,this,&FunctionWidget::ShowAdvanceWidgetSlots);
    connect(ui->toolButton_exchange,&QToolButton::clicked,this,&FunctionWidget::ShowExchangeWidgetSlots);
    connect(ui->toolButton_more,&QToolButton::clicked,this,&FunctionWidget::ShowMoreWidgetSlots);

    connect(actionLock,&QAction::triggered,this,&FunctionWidget::lock);
    connect(actionSet,&QAction::triggered,this,&FunctionWidget::ShowSettingWidgetSlots);
    connect(actionConsole,&QAction::triggered,this,&FunctionWidget::ShowConsoleWidgetSlots);
    connect(actionAbout,&QAction::triggered,this,&FunctionWidget::ShowAboutWidgetSlots);
    connect(actionUpdate,&QAction::triggered,this,&FunctionWidget::ShowUpdateWidgetSlots);

    connect(_p->accountBar,&FunctionAccountWidget::showAccountSignal,this,&FunctionWidget::showAccountSignal);
    connect(_p->accountBar,&FunctionAccountWidget::showMinerSignal,this,&FunctionWidget::showMinerSignal);

    connect(_p->advanceBar,&FunctionAdvanceWidget::showPoundageSignal,this,&FunctionWidget::showPoundageSignal);
    connect(_p->advanceBar,&FunctionAdvanceWidget::showMultiSigSignal,this,&FunctionWidget::showMultiSigSignal);

    connect(_p->exchangeBar,&FunctionExchangeWidget::showOnchainOrderSignal,this,&FunctionWidget::showOnchainOrderSignal);
    connect(_p->exchangeBar,&FunctionExchangeWidget::showMyOrderSignal,this,&FunctionWidget::showMyOrderSignal);

}

void FunctionWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(72,97,220));
    setPalette(palette);

    setStyleSheet("\
    QToolButton{min-height:24px;min-width:24px;background: rgb(72,97,220);border:none;color:white;font:Microsoft YaHei UI Light;}\
    QToolButton:hover{background-color: rgb(0,210, 255);}\
    QToolButton:pressed{background-color: rgb(84,116, 235);}\
    QToolButton:checked{background-color: rgb(84,116, 235);}\
                  "

                  );
    _p->contextMenu->setStyleSheet("QMenu{background-color:rgb(238,241,253);border: none;border-radius:10px;}"
                                   "QMenu::item {border:none;background-color:rgb(238,241,253);color:black;border-bottom:1px solid #DBDBDB;}"
                                   "QMenu::item::selected{border:none;background-color: #829DFF;}"
                                   "QMenu::indicator{margin:0px 4px;}");

    ui->toolButton_account->setIconSize(QSize(24,24));
    ui->toolButton_advanced->setIconSize(QSize(24,24));
    ui->toolButton_contact->setIconSize(QSize(24,24));
    ui->toolButton_exchange->setIconSize(QSize(24,24));
    ui->toolButton_more->setIconSize(QSize(24,24));

    ui->toolButton_account->setIcon(QIcon(":/functionBar/account.png"));
    ui->toolButton_advanced->setIcon(QIcon(":/functionBar/advance.png"));
    ui->toolButton_contact->setIcon(QIcon(":/functionBar/contact.png"));
    ui->toolButton_exchange->setIcon(QIcon(":/functionBar/exchange.png"));
    ui->toolButton_more->setIcon(QIcon(":/functionBar/more.png"));

}
