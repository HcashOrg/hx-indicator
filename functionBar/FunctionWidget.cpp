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
    {

    }
public:
    FunctionAccountWidget *accountBar;
    FunctionAdvanceWidget *advanceBar;
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
}

void FunctionWidget::ShowAccountWidgetSlots()
{
    ui->stackedWidget->setCurrentWidget(_p->accountBar);
    emit showAccountSignal();
    //emit showAccountPage();

    ui->stackedWidget->setVisible(true);
    emit RestoreSignal();
}

void FunctionWidget::ShowAdvanceWidgetSlots()
{
    ui->stackedWidget->setCurrentWidget(_p->advanceBar);

    emit showAdvanceSignal();

    ui->stackedWidget->setVisible(true);
    emit RestoreSignal();

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

void FunctionWidget::InitWidget()
{
    InitStyle();

    ui->stackedWidget->addWidget(_p->accountBar);
    ui->stackedWidget->addWidget(_p->advanceBar);

    //设置更多按钮
    QMenu *menu = new QMenu();
    QAction* actionSet = menu->addAction("Setting");
    QAction* actionLock = menu->addAction("Lock");
    QAction* actionConsole = menu->addAction("Console");
    QMenu *helpMenu = new QMenu("Help");
    menu->addMenu(helpMenu);
    QAction* actionAbout = helpMenu->addAction("About Us");
    QAction* actionUpdate = helpMenu->addAction("Update");
    ui->toolButton_more->setMenu(menu);
    ui->toolButton_more->setPopupMode(QToolButton::InstantPopup );

    //链接信号槽
    connect(ui->toolButton_contact,&QToolButton::clicked,this,&FunctionWidget::ShowContactWidgetSlots);
    connect(ui->toolButton_account,&QToolButton::clicked,this,&FunctionWidget::ShowAccountWidgetSlots);
    connect(ui->toolButton_advanced,&QToolButton::clicked,this,&FunctionWidget::ShowAdvanceWidgetSlots);

    connect(actionLock,&QAction::triggered,this,&FunctionWidget::lock);
    connect(actionSet,&QAction::triggered,this,&FunctionWidget::ShowSettingWidgetSlots);
    connect(actionConsole,&QAction::triggered,this,&FunctionWidget::ShowConsoleWidgetSlots);

    connect(_p->advanceBar,&FunctionAdvanceWidget::showPoundageSignal,this,&FunctionWidget::showPoundageSignal);
    connect(_p->advanceBar,&FunctionAdvanceWidget::showMultiSigSignal,this,&FunctionWidget::showMultiSigSignal);


}

void FunctionWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(16,17,24));
    setPalette(palette);

    ui->toolButton_contact->setIconSize(QSize(94,94));
    ui->toolButton_account->setIconSize(QSize(94,94));
    ui->toolButton_advanced->setIconSize(QSize(94,94));
    ui->toolButton_more->setIconSize(QSize(94,94));

    ui->toolButton_contact->setStyleSheet("background:transparent;border:none;");
    ui->toolButton_account->setStyleSheet("background:transparent;border:none;");
    ui->toolButton_advanced->setStyleSheet("background:transparent;border:none;");
    ui->toolButton_more->setStyleSheet("background:transparent;border:none;");

    //ui->toolButton_contact->setStyleSheet("QToolButton:hover{background:transparent;border:none;border-image:url(:/ui/wallet_ui/contactBtn.png);}");
    ui->toolButton_account->setIcon(QIcon(":/ui/wallet_ui/accountBtn.png"));

    ui->toolButton_advanced->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

    ui->toolButton_contact->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

    ui->toolButton_more->setIcon(QIcon(":/ui/wallet_ui/advanceBtn.png"));

}

void FunctionWidget::paintEvent(QPaintEvent *)
{
    //QPainter painter(this);
    //painter.setPen(QPen(QColor(16,17,24),Qt::SolidLine));
    //painter.setBrush(QBrush(QColor(16,17,24),Qt::SolidPattern));
    //
    //painter.drawRect(0,0,94,556);
    //
    //painter.setPen(QPen(QColor(24,28,45),Qt::SolidLine));
    //painter.setBrush(QBrush(QColor(24,28,45),Qt::SolidPattern));
    //painter.drawRect(95,0,125,556);
}
