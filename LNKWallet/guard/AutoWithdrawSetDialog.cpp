#include "AutoWithdrawSetDialog.h"
#include "ui_AutoWithdrawSetDialog.h"

#include <QLineEdit>
#include <QDebug>
#include "wallet.h"

AutoWithdrawSetDialog::AutoWithdrawSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoWithdrawSetDialog)
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

    ui->tableWidget->setColumnWidth(0,120);
    ui->tableWidget->setColumnWidth(1,160);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    showLimitAmount();
}

AutoWithdrawSetDialog::~AutoWithdrawSetDialog()
{
    delete ui;
}

void AutoWithdrawSetDialog::pop()
{
    move(0,0);
    exec();
}

void AutoWithdrawSetDialog::showLimitAmount()
{
    qDebug() << HXChain::getInstance()->senatorAutoWithdrawAmountMap;
    QStringList assets = HXChain::getInstance()->assetInfoMap.keys();
    assets.removeAll("1.3.0");
    int size = assets.size();
    ui->tableWidget->setRowCount(size);
    for(int i = 0; i < size; i++)
    {
        AssetInfo info = HXChain::getInstance()->assetInfoMap.value(assets.at(i));
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(info.symbol));

        QLineEdit* lineEdit = new QLineEdit;
        lineEdit->setAlignment(Qt::AlignHCenter);
        QRegExp regx("[0]|[1-9][0-9]{0,8}");
        QValidator *validator1 = new QRegExpValidator(regx, this);
        lineEdit->setValidator( validator1 );

        ui->tableWidget->setCellWidget(i,1, lineEdit);
        lineEdit->setText( QString::number( HXChain::getInstance()->getAssetAutoWithdrawLimit(info.symbol), 'g', 12));
    }
    tableWidgetSetItemZebraColor(ui->tableWidget);
}

void AutoWithdrawSetDialog::save()
{
    HXChain::getInstance()->configFile->beginGroup("/AutoWithdrawAmount");
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QString symbol = ui->tableWidget->item(i,0)->text();
        QLineEdit* lineEdit = static_cast<QLineEdit*>( ui->tableWidget->cellWidget(i,1));
        if(lineEdit)
        {
            HXChain::getInstance()->configFile->setValue(symbol, lineEdit->text().toDouble());
        }
    }

    HXChain::getInstance()->configFile->endGroup();
}

void AutoWithdrawSetDialog::on_okBtn_clicked()
{
    save();
    HXChain::getInstance()->loadAutoWithdrawAmount();
    close();
}

void AutoWithdrawSetDialog::on_cancelBtn_clicked()
{
    close();
}

void AutoWithdrawSetDialog::on_closeBtn_clicked()
{
    close();
}
