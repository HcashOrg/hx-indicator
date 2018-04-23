#include "AccountManagerWidget.h"
#include "ui_AccountManagerWidget.h"

#include "poundage/PageScrollWidget.h"

#include "wallet.h"

#include "ExportDialog.h"
#include "dialog/BackupWalletDialog.h"

#include <ToolButtonWidget.h>
#include <QToolButton>
#include <QDebug>

Q_DECLARE_METATYPE(AccountInfo)

class AccountManagerWidget::AccountManagerWidgetPrivate
{
public:
    AccountManagerWidgetPrivate()
        :pageWidget(new PageScrollWidget(5))
    {

    }
public:
    PageScrollWidget *pageWidget;
};

AccountManagerWidget::AccountManagerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountManagerWidget),
    _p(new AccountManagerWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

AccountManagerWidget::~AccountManagerWidget()
{
    delete _p;
    delete ui;
}

void AccountManagerWidget::pageChangeSlot(unsigned int page)
{
    RefreshTableShow(page);
}

void AccountManagerWidget::deleteButtonSlots()
{
    int row = ui->tableWidget->rowAt(ui->tableWidget->mapFromGlobal(QCursor::pos()).y());
    qDebug()<<"clicked          "<<row;
    UBChain::getInstance()->postRPC( "id-remove_local_account",
                                     toJsonFormat( "remove_local_account", QJsonArray()
                                     <<ui->tableWidget->item(row,0)->data(Qt::UserRole).value<AccountInfo>().name));
}

void AccountManagerWidget::exportButtonSlots()
{
    int row = ui->tableWidget->rowAt(ui->tableWidget->mapFromGlobal(QCursor::pos()).y());
    qDebug()<<"clicked          "<<row;
    ExportDialog exportDialog(ui->tableWidget->item(row,0)->data(Qt::UserRole).value<AccountInfo>().name);
    exportDialog.pop();
}

void AccountManagerWidget::backupButtonSlots()
{
    BackupWalletDialog backupWalletDialog;
    backupWalletDialog.pop();
}

void AccountManagerWidget::RefreshTableShow(unsigned int page)
{
    for(int i = 0;i < ui->tableWidget->rowCount();++i)
    {
        if(i < page*3)
        {
            ui->tableWidget->setRowHidden(i,true);
        }
        else if(page * 3 <= i && i < page*3 + 3)
        {
            ui->tableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->tableWidget->setRowHidden(i,true);
        }
    }
}

void AccountManagerWidget::InitWidget()
{
    InitStyle();

    //初始化账户信息
    int totalPage = UBChain::getInstance()->accountInfoMap.size()%3 == 0 ?
                    UBChain::getInstance()->accountInfoMap.size()/3 :
                    UBChain::getInstance()->accountInfoMap.size()/3 + 1;
    _p->pageWidget->SetTotalPage(totalPage);
    ui->stackedWidget->addWidget(_p->pageWidget);
    ui->stackedWidget->setCurrentWidget(_p->pageWidget);

    foreach (AccountInfo info, UBChain::getInstance()->accountInfoMap) {
        QTableWidgetItem *item = new QTableWidgetItem(info.name+"\n"+info.address);
        item->setData(Qt::UserRole,QVariant::fromValue<AccountInfo>(info));
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setRowHeight(ui->tableWidget->rowCount()-1,90);
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,item);

        ToolButtonWidget *button1 = new ToolButtonWidget();
        button1->setText(tr("delete"));
        button1->setInitGray(true);
        ui->tableWidget->setIndexWidget(ui->tableWidget->model()->index(ui->tableWidget->rowCount()-1,1),button1);

        ToolButtonWidget *button2 = new ToolButtonWidget();
        button2->setText(tr("export"));
        ui->tableWidget->setIndexWidget(ui->tableWidget->model()->index(ui->tableWidget->rowCount()-1,2),button2);

        connect(button1,&ToolButtonWidget::clicked,this,&AccountManagerWidget::deleteButtonSlots);
        connect(button2,&ToolButtonWidget::clicked,this,&AccountManagerWidget::exportButtonSlots);
    }
    RefreshTableShow(0);
    connect(_p->pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&AccountManagerWidget::pageChangeSlot);
    connect(ui->toolButton,&QToolButton::clicked,this,&AccountManagerWidget::backupButtonSlots);
}

void AccountManagerWidget::InitStyle()
{
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setColumnWidth(0,480);
    ui->tableWidget->setColumnWidth(1,80);
    ui->tableWidget->setColumnWidth(2,80);
    ui->tableWidget->setStyleSheet("QTableView{border:none;}");

    ui->toolButton->setStyleSheet("QToolButton{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                                  "QToolButton:hover{background-color:#00D2FF;}");

    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
}
