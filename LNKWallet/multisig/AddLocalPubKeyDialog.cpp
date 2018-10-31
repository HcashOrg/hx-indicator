#include "AddLocalPubKeyDialog.h"
#include "ui_AddLocalPubKeyDialog.h"

#include <QCheckBox>
#include "wallet.h"
#include "showcontentdialog.h"

AddLocalPubKeyDialog::AddLocalPubKeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLocalPubKeyDialog)
{
    ui->setupUi(this);

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->tableWidget->setFrameShape(QFrame::NoFrame);
    ui->tableWidget->setMouseTracking(true);
    ui->tableWidget->setShowGrid(false);//隐藏表格线

    ui->tableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->tableWidget->horizontalHeader()->setFixedHeight(40);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->tableWidget->setColumnWidth(0,80);
    ui->tableWidget->setColumnWidth(1,350);
    ui->tableWidget->setColumnWidth(2,30);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    showLocalAccountPubKeys();
}

AddLocalPubKeyDialog::~AddLocalPubKeyDialog()
{
    delete ui;
}

bool AddLocalPubKeyDialog::pop()
{
    move(0,0);
    exec();

    return yesOrNo;
}

void AddLocalPubKeyDialog::setAddedPubKeys(QVector<AccountPubKey> &accountPubKeys)
{
    for( int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        if(ui->tableWidget->cellWidget( i, 2) && ui->tableWidget->item( i, 1))
        {
            QString pubKey = ui->tableWidget->item(i,1)->text();
            if(accountPubKeys.contains(pubKey))
            {
                QCheckBox* cb = static_cast<QCheckBox*>(ui->tableWidget->cellWidget( i, 2));
                cb->setChecked(true);
            }
        }
    }
}

void AddLocalPubKeyDialog::showLocalAccountPubKeys()
{
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    int size = accounts.size();
    ui->tableWidget->setRowCount(size);
    for( int i = 0; i < size; i++)
    {
        AccountInfo info = HXChain::getInstance()->accountInfoMap.value(accounts.at(i));
        ui->tableWidget->setItem( i, 0, new QTableWidgetItem(accounts.at(i)));
        ui->tableWidget->setItem( i, 1, new QTableWidgetItem(info.pubKey));

        QCheckBox* cb = new QCheckBox;
        ui->tableWidget->setCellWidget( i, 2, cb);
    }

    tableWidgetSetItemZebraColor(ui->tableWidget);

}

void AddLocalPubKeyDialog::on_okBtn_clicked()
{
    for( int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        if(ui->tableWidget->cellWidget( i, 2) && ui->tableWidget->item( i, 0))
        {
            QCheckBox* cb = static_cast<QCheckBox*>(ui->tableWidget->cellWidget( i, 2));
            if(cb->isChecked())
            {
                addedAccounts += ui->tableWidget->item(i,0)->text();
            }
        }
    }


    yesOrNo = true;
    close();
}

void AddLocalPubKeyDialog::on_closeBtn_clicked()
{
    yesOrNo = false;
    close();
}

void AddLocalPubKeyDialog::on_cancelBtn_clicked()
{
    yesOrNo = false;
    close();
}

void AddLocalPubKeyDialog::on_tableWidget_cellClicked(int row, int column)
{
    if(column == 1)
    {
        if( !ui->tableWidget->item(row, column))    return;

        ShowContentDialog showContentDialog( ui->tableWidget->item(row, column)->text(),this);

        int x = ui->tableWidget->columnViewportPosition(column) + ui->tableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->tableWidget->rowViewportPosition(row) - 10 + ui->tableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->tableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();
    }
}
