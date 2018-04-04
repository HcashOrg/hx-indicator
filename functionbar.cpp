#include "functionbar.h"
#include "ui_functionbar.h"

#include "wallet.h"
#include "commondialog.h"
#include "dialog/functionbarchoicedialog.h"
#include "setdialog.h"
#include "consoledialog.h"


#include <QDebug>
#include <QLineEdit>
#include <QListView>


#define FUNCTIONPAGE_BTN_SELECTED       "background-color: rgb(20,170,255);color: rgb(255, 255, 255);border:none;"
#define FUNCTIONPAGE_BTN_UNSELECTED     "background-color: rgb(15,160,250);color: rgb(255, 255, 255);border:none;"
#define FUNCTIONPAGE_CHILD_BTN_SELECTED      "background-color: rgb(21,24,37);color: rgb(239,240,240);border:none;"
#define FUNCTIONPAGE_CHILD_BTN_UNSELECTED    "background-color: rgb(24,28,45);color: rgb(160,168,198);border:none;"

FunctionBar::FunctionBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionBar)
{
    
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(16,17,24));
    setPalette(palette);


    ui->accountBtn->setIconSize(QSize(103,94));
    ui->transferBtn->setIconSize(QSize(103,94));
    ui->contactBtn->setIconSize(QSize(103,94));
    ui->advanceBtn->setIconSize(QSize(103,94));


    ui->accountBtn->setStyleSheet("background:transparent;border:none;");
    ui->transferBtn->setStyleSheet("background:transparent;border:none;");
    ui->contactBtn->setStyleSheet("background:transparent;border:none;");
    ui->advanceBtn->setStyleSheet("background:transparent;border:none;");

    ui->logoLabel->setPixmap(QPixmap(":/ui/wallet_ui/logo_26x17.png"));
    ui->versionLabel->setText(QString("v") + WALLET_VERSION);



    ui->choiceBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/choice.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                 "QToolButton:hover{background-image:url(:/ui/wallet_ui/choice_hover.png);}");



    choosePage(1);

    refresh();

}

FunctionBar::~FunctionBar()
{
    delete ui;
}

void FunctionBar::on_accountBtn_clicked()
{
    if(currentPageIndex == 1 || currentPageIndex == 4)   return;

    on_accountListBtn_clicked();
}


void FunctionBar::on_transferBtn_clicked()
{
    mutexForAddressMap.lock();
    int size = UBChain::getInstance()->accountInfoMap.keys().size();
    mutexForAddressMap.unlock();
    if( size > 0)   // 有至少一个账户
    {
        choosePage(2);
        emit showTransferPage();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("No account for transfering,\nadd an account first"));
//        commonDialog.move( this->mapToGlobal(QPoint(318,150)));
        commonDialog.pop();
        on_accountBtn_clicked();
    }
}

void FunctionBar::on_contactBtn_clicked()
{
    choosePage(3);
    emit showContactPage();
}

void FunctionBar::on_smartContractBtn_clicked()
{
    choosePage(5);
    emit showSmartContractPage();
}


void FunctionBar::on_feedBtn_clicked()
{
    mutexForAddressMap.lock();
    int size = UBChain::getInstance()->addressMap.size();
    mutexForAddressMap.unlock();
    if( size > 0)   // 有至少一个账户
    {
        choosePage(6);
        emit showFeedPage();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("No accounts now. Please add an account first."));
//        commonDialog.move( this->mapToGlobal(QPoint(318,150)));
        commonDialog.pop();
        on_accountBtn_clicked();
    }
}


void FunctionBar::on_multiSigBtn_clicked()
{
    choosePage(7);
    emit showMultiSigPage();
}


void FunctionBar::choosePage(int pageIndex)
{
    currentPageIndex = pageIndex;
    switch (pageIndex) {
    case 1:
        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn_unselected.png"));

        ui->accountListBtn->show();
        ui->accountListBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_SELECTED);
        ui->billInfoBtn->show();
        ui->billInfoBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);

        ui->smartContractBtn->hide();
        ui->feedBtn->hide();
        ui->multiSigBtn->hide();

        ui->currentChoiceLabel->setText(tr("Account"));

        break;
    case 2:
        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn_unselected.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn_unselected.png"));

        ui->accountListBtn->hide();
        ui->billInfoBtn->hide();
        ui->smartContractBtn->hide();
        ui->feedBtn->hide();
        ui->multiSigBtn->hide();

        ui->currentChoiceLabel->setText(tr("Transfer"));

        break;
    case 3:
        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn_unselected.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn_unselected.png"));

        ui->accountListBtn->hide();
        ui->billInfoBtn->hide();
        ui->smartContractBtn->hide();
        ui->feedBtn->hide();
        ui->multiSigBtn->hide();

        ui->currentChoiceLabel->setText(tr("Contacts"));

        break;
    case 4:
        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn_unselected.png"));

        ui->accountListBtn->show();
        ui->accountListBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);
        ui->billInfoBtn->show();
        ui->billInfoBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_SELECTED);

        ui->smartContractBtn->hide();
        ui->feedBtn->hide();
        ui->multiSigBtn->hide();

        ui->currentChoiceLabel->setText(tr("Account"));

        break;
    case 5:

        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn_unselected.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn.png"));

        ui->accountListBtn->hide();
        ui->billInfoBtn->hide();

        ui->smartContractBtn->show();
        ui->smartContractBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_SELECTED);
        ui->feedBtn->show();
        ui->feedBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);
        ui->multiSigBtn->show();
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);

        ui->currentChoiceLabel->setText(tr("Advanced"));


        break;
    case 6:

        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn_unselected.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn.png"));

        ui->accountListBtn->hide();
        ui->billInfoBtn->hide();

        ui->smartContractBtn->show();
        ui->smartContractBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);
        ui->feedBtn->show();
        ui->feedBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_SELECTED);
        ui->multiSigBtn->show();
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);

        ui->currentChoiceLabel->setText(tr("Advanced"));


        break;
    case 7:

        ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/accountBtn_unselected.png"));

        ui->transferBtn->setIcon(QIcon(":/ui/wallet_ui/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/ui/wallet_ui/contactBtn_unselected.png"));

        ui->advanceBtn->setIcon(QIcon(":/ui/wallet_ui/advanceBtn.png"));

        ui->accountListBtn->hide();
        ui->billInfoBtn->hide();

        ui->smartContractBtn->show();
        ui->smartContractBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);
        ui->feedBtn->show();
        ui->feedBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_UNSELECTED);
        ui->multiSigBtn->show();
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_CHILD_BTN_SELECTED);

        ui->currentChoiceLabel->setText(tr("Advanced"));

        break;
    default:
        break;
    }
}

void FunctionBar::retranslator()
{
    ui->retranslateUi(this);
}


void FunctionBar::refresh()
{

}

void FunctionBar::on_advanceBtn_clicked()
{
    if(currentPageIndex == 5 || currentPageIndex == 6 ||currentPageIndex == 7)      return;

    on_smartContractBtn_clicked();
}

void FunctionBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(16,17,24),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(16,17,24),Qt::SolidPattern));
    painter.drawRect(0,0,60,556);

    painter.setPen(QPen(QColor(24,28,45),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(24,28,45),Qt::SolidPattern));
    painter.drawRect(61,0,160,556);
}

void FunctionBar::on_accountListBtn_clicked()
{
    choosePage(1);
    emit showMainPage();
}

void FunctionBar::on_billInfoBtn_clicked()
{
    choosePage(4);
    emit showAccountPage();
}

void FunctionBar::on_choiceBtn_clicked()
{
    FunctionBarChoiceDialog functionBarChoiceDialog(this);
    functionBarChoiceDialog.move( ui->choiceBtn->mapToGlobal(QPoint(-32,-10 - functionBarChoiceDialog.height())));
    functionBarChoiceDialog.exec();

    switch (functionBarChoiceDialog.choice)
    {
    case 1:
        emit lock();
        break;
    case 2:
    {
        ConsoleDialog consoleDialog;
        consoleDialog.pop();
    }
        break;
    case 3:
    {
        SetDialog setDialog;
        connect(&setDialog,SIGNAL(settingSaved()),UBChain::getInstance()->mainFrame,SLOT(settingSaved()));
        setDialog.pop();
    }
        break;
    default:
        break;
    }
}
