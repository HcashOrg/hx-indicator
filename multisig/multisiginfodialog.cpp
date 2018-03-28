#include "multisiginfodialog.h"
#include "ui_multisiginfodialog.h"

#include <QDebug>

#include "lnk.h"
#include "../commondialog.h"
#include "showcontentdialog.h"


MultiSigInfoDialog::MultiSigInfoDialog(QString _multiSigAddress, QWidget *parent) :
    QDialog(parent),
    multiSigAddress(_multiSigAddress),
    ui(new Ui::MultiSigInfoDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->infoTableWidget->installEventFilter(this);
    ui->infoTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->infoTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->infoTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->infoTableWidget->setFrameShape(QFrame::NoFrame);
    ui->infoTableWidget->setMouseTracking(true);
    ui->infoTableWidget->setShowGrid(false);//隐藏表格线

    ui->infoTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->infoTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->infoTableWidget->horizontalHeader()->setVisible(true);
    ui->infoTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    ui->infoTableWidget->setColumnWidth(0,300);
    ui->infoTableWidget->setColumnWidth(1,100);


    showMultiSigInfo();
}

MultiSigInfoDialog::~MultiSigInfoDialog()
{
    delete ui;
}

void MultiSigInfoDialog::pop()
{
    move(0,0);
    exec();
}

void MultiSigInfoDialog::showMultiSigInfo()
{
    MultiSigInfo info = UBChain::getInstance()->multiSigInfoMap.value(multiSigAddress);

    int size = info.owners.size();
    ui->infoTableWidget->setRowCount(size);

    ui->addressLabel->setText(multiSigAddress);
    ui->requiresLabel->setText(QString::number(info.requires));

    for(int i = 0; i < size; i++)
    {
        QString address = info.owners.at(i);

        ui->infoTableWidget->setRowHeight(i,37);

        ui->infoTableWidget->setItem(i,0,new QTableWidgetItem(address));
        ui->infoTableWidget->item(i,0)->setTextColor(QColor(192,196,212));

        if(UBChain::getInstance()->isMyAddress(address))
        {
            QString accountName = UBChain::getInstance()->addressToName(address);
            ui->infoTableWidget->setItem(i,1,new QTableWidgetItem(accountName));
            ui->infoTableWidget->item(i,1)->setTextColor(QColor(192,196,212));
        }
        else
        {
            ui->infoTableWidget->setItem(i,1,new QTableWidgetItem(tr("non-local")));
            ui->infoTableWidget->item(i,1)->setTextColor(QColor(255,34,76));
        }

        for( int j = 0; j < 2; j++)
        {
            ui->infoTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);

            if(i % 2)
            {
                ui->infoTableWidget->item(i,j)->setBackgroundColor(QColor(43,49,69));
            }
            else
            {
                ui->infoTableWidget->item(i,j)->setBackgroundColor(QColor(40,46,66));
            }
        }
    }

}

void MultiSigInfoDialog::on_okBtn_clicked()
{
    close();
}

void MultiSigInfoDialog::on_infoTableWidget_cellPressed(int row, int column)
{
    if( column == 0 )
    {
        ShowContentDialog showContentDialog( ui->infoTableWidget->item(row, column)->text(),this);

        int x = ui->infoTableWidget->columnViewportPosition(column) + ui->infoTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->infoTableWidget->rowViewportPosition(row) - 10 + ui->infoTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->infoTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void MultiSigInfoDialog::on_closeBtn_clicked()
{
    close();
}
