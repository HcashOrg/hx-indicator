#include "contactdialog.h"
#include "ui_contactdialog.h"
#include "wallet.h"
#include <QDebug>


ContactDialog::ContactDialog(QWidget *parent) :
    QDialog(parent),
    previousColorRow(0),
    ui(new Ui::ContactDialog)
{
    
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);

    ui->contactsTableWidget->setShowGrid(false);
    ui->contactsTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->contactsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->contactsTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->contactsTableWidget->setColumnWidth(0,8);
    ui->contactsTableWidget->setColumnWidth(1,160);
    ui->contactsTableWidget->setColumnWidth(2,160);
    ui->contactsTableWidget->setMouseTracking(true);

    updateContactsList();
    
}

ContactDialog::~ContactDialog()
{
    
    delete ui;
    
}

void ContactDialog::updateContactsList()
{
    
    if( !UBChain::getInstance()->contactsFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "contact.dat not exist";
    }
    QString str = QByteArray::fromBase64( UBChain::getInstance()->contactsFile->readAll());
    QStringList strList = str.split(";");
    strList.removeLast();
    int size = strList.size();
    ui->contactsTableWidget->setRowCount(size);

    for( int i = 0; i < size; i++)
    {
        QString str2 = strList.at(i);
        if( str2.size() - 1 == str2.indexOf("="))    // 如果没有备注，第一列为空，第二列显示地址，第三列为空
        {
            ui->contactsTableWidget->setItem(i,1, new QTableWidgetItem( str2.left( str2.indexOf("="))));
            ui->contactsTableWidget->setItem(i,0, new QTableWidgetItem( ""));
            ui->contactsTableWidget->setItem(i,2, new QTableWidgetItem( ""));
        }
        else                                         // 如果有备注，第一列为空，第二列显示备注，第三列保存地址
        {
            ui->contactsTableWidget->setItem(i,1, new QTableWidgetItem( str2.mid( str2.indexOf("=") + 1)));
            ui->contactsTableWidget->setItem(i,0, new QTableWidgetItem( ""));
            ui->contactsTableWidget->setItem(i,2, new QTableWidgetItem( str2.left( str2.indexOf("="))));
        }
    }

    UBChain::getInstance()->contactsFile->close();

    
}


void ContactDialog::on_contactsTableWidget_cellClicked(int row, int column)
{
    if( ui->contactsTableWidget->item( row,2)->text().isEmpty())
    {
        emit contactSelected("",ui->contactsTableWidget->item( row,1)->text());
    }
    else
    {
        emit contactSelected(ui->contactsTableWidget->item( row,1)->text(),ui->contactsTableWidget->item( row,2)->text());
    }
    close();
}

void ContactDialog::on_contactsTableWidget_cellEntered(int row, int column)
{
    ui->contactsTableWidget->item(previousColorRow,0)->setBackgroundColor(QColor(255,255,255));
    ui->contactsTableWidget->item(previousColorRow,1)->setTextColor(QColor(0,0,0));
    ui->contactsTableWidget->item(previousColorRow,1)->setBackgroundColor(QColor(255,255,255));

    ui->contactsTableWidget->item(row,0)->setBackgroundColor(QColor(10,148,254));
    ui->contactsTableWidget->item(row,1)->setTextColor(QColor(255,255,255));
    ui->contactsTableWidget->item(row,1)->setBackgroundColor(QColor(10,148,254));
    previousColorRow = row;
}
