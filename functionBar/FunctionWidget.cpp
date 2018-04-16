#include "FunctionWidget.h"
#include "ui_FunctionWidget.h"

#include <QMenu>

#include "functionBar/FunctionAccountWidget.h"
#include "functionBar/FunctionAdvanceWidget.h"

#include "setdialog.h"
#include "consoledialog.h"
#include "wallet.h"

class FunctionWidget::FunctionWidgetPrivate
{
public:
    FunctionWidgetPrivate()
        :accountBar(new FunctionAccountWidget())
        ,advanceBar(new FunctionAdvanceWidget())
        ,contextMenu(new QMenu())
    {

    }
public:
    FunctionAccountWidget *accountBar;
    FunctionAdvanceWidget *advanceBar;
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

void FunctionWidget::ShowMoreWidgetSlots()
{
    _p->contextMenu->exec(mapToGlobal(QPoint(70,height()-136)));
    updateCheckState(3);
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

void FunctionWidget::updateCheckState(int buttonNumber)
{
    ui->toolButton_account->setChecked(0 == buttonNumber);
    ui->toolButton_contact->setChecked(1 == buttonNumber);
    ui->toolButton_advanced->setChecked(2 == buttonNumber);
    ui->toolButton_more->setChecked(3 == buttonNumber);
}

void FunctionWidget::InitWidget()
{
    InitStyle();

    ui->toolButton_contact->setCheckable(true);
    ui->toolButton_account->setCheckable(true);
    ui->toolButton_advanced->setCheckable(true);
    ui->toolButton_more->setCheckable(true);

    ui->stackedWidget->addWidget(_p->accountBar);
    ui->stackedWidget->addWidget(_p->advanceBar);

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
    connect(ui->toolButton_more,&QToolButton::clicked,this,&FunctionWidget::ShowMoreWidgetSlots);

    connect(actionLock,&QAction::triggered,this,&FunctionWidget::lock);
    connect(actionSet,&QAction::triggered,this,&FunctionWidget::ShowSettingWidgetSlots);
    connect(actionConsole,&QAction::triggered,this,&FunctionWidget::ShowConsoleWidgetSlots);

    connect(_p->accountBar,&FunctionAccountWidget::showMinerSignal,this,&FunctionWidget::showMinerSignal);

    connect(_p->advanceBar,&FunctionAdvanceWidget::showPoundageSignal,this,&FunctionWidget::showPoundageSignal);
    connect(_p->advanceBar,&FunctionAdvanceWidget::showMultiSigSignal,this,&FunctionWidget::showMultiSigSignal);
    connect(_p->advanceBar,&FunctionAdvanceWidget::showMyExchangeContractSignal,this,&FunctionWidget::showMyExchangeContractSignal);
}

void FunctionWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(72,97,220));
    setPalette(palette);

    setStyleSheet("\
    QToolButton{min-height:24px;min-width:24pt;background: rgb(72,97,220);border:none;color:white;font:MicrosoftYaHeiLight;}\
    QToolButton:hover{background-color: rgb(0,210, 255);}\
    QToolButton:pressed{background-color: rgb(84,116, 235);}\
    QToolButton:checked{background-color: rgb(84,116, 235);}\
                  "
                  );
    _p->contextMenu->setStyleSheet("QMenu{background-color:rgb(238,241,253);border: none;border-radius:10px;}"
                                   "QMenu::item {border:none;background-color:rgb(238,241,253);color:black;border-bottom:1px solid #DBDBDB;}"
                                   "QMenu::item::selected{border:none;background-color: #829DFF;}"
                                   "QMenu::indicator{margin:0px 4px;}");
                  /*
                      QToolButton#toolButton_account{border-image:url(:/ui/wallet_ui/accountBtn_unselected.png);}\
                      QToolButton#toolButton_account:checked{border-image:url(:/ui/wallet_ui/accountBtn.png);}\*/



    //ui->toolButton_contact->setStyleSheet("background:transparent;border:none;");
    //ui->toolButton_account->setStyleSheet("background:transparent;border:none;");
    //ui->toolButton_advanced->setStyleSheet("background:transparent;border:none;");
    //ui->toolButton_more->setStyleSheet("background:transparent;border:none;");

    //ui->toolButton_contact->setStyleSheet("QToolButton:hover{background:transparent;border:none;border-image:url(:/ui/wallet_ui/contactBtn.png);}");
    //ui->toolButton_account->setIcon(QIcon(":/ui/wallet_ui/accountBtn.png"));

    //ui->toolButton_advanced->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));
    //
    //ui->toolButton_contact->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));
    //
    //ui->toolButton_more->setIcon(QIcon(":/ui/wallet_ui/advanceBtn.png"));

}
