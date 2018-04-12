#include "minerpage.h"
#include "ui_minerpage.h"

#include <QPainter>
#include "wallet.h"
#include "registerdialog.h"
#include "locktominerdialog.h"
#include "foreclosedialog.h"
#include "commondialog.h"

MinerPage::MinerPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MinerPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->lockBalancesTableWidget->installEventFilter(this);
    ui->lockBalancesTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->lockBalancesTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->lockBalancesTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->lockBalancesTableWidget->setFrameShape(QFrame::NoFrame);
    ui->lockBalancesTableWidget->setMouseTracking(true);
    ui->lockBalancesTableWidget->setShowGrid(false);//隐藏表格线

    ui->lockBalancesTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->lockBalancesTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->lockBalancesTableWidget->horizontalHeader()->setVisible(true);
    ui->lockBalancesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->lockBalancesTableWidget->setColumnWidth(0,140);
    ui->lockBalancesTableWidget->setColumnWidth(1,140);
    ui->lockBalancesTableWidget->setColumnWidth(2,80);
    ui->lockBalancesTableWidget->setColumnWidth(3,80);
    ui->lockBalancesTableWidget->setColumnWidth(4,80);
    ui->lockBalancesTableWidget->setColumnWidth(5,80);



    init();
}

MinerPage::~MinerPage()
{
    delete ui;
}

void MinerPage::refresh()
{
    getLockBalance();
}

void MinerPage::jsonDataUpdated(QString id)
{
    if( id == "id-get_account_lock_balance-" + ui->accountComboBox->currentText())
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        qDebug() << id << result;

        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonArray array = jsonObject.take("result").toArray();
        int size = array.size();
        ui->lockBalancesTableWidget->setRowCount(0);
        ui->lockBalancesTableWidget->setRowCount(size);

        for(int i = 0; i < size; i++)
        {
            QJsonObject object = array.at(i).toObject();
            QString minerId = object.take("lockto_miner_account").toString();
            QString assetId = object.take("lock_asset_id").toString();
            unsigned long long lockAmount = 0;
            QJsonValue value = object.take("lock_asset_amount");
            if(value.isString())
            {
                lockAmount = value.toString().toULongLong();
            }
            else
            {
                lockAmount = QString::number(value.toDouble(),'g',10).toULongLong();
            }

            ui->lockBalancesTableWidget->setItem(i,0,new QTableWidgetItem(UBChain::getInstance()->getMinerNameFromId(minerId)));

            AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(assetId);
            ui->lockBalancesTableWidget->setItem(i,1,new QTableWidgetItem(assetInfo.symbol));

            ui->lockBalancesTableWidget->setItem(i,2,new QTableWidgetItem(getBigNumberString(lockAmount,assetInfo.precision)));

            ui->lockBalancesTableWidget->setItem(i,3,new QTableWidgetItem(tr("add")));
            ui->lockBalancesTableWidget->setItem(i,4,new QTableWidgetItem(tr("foreclose")));

        }


        return;
    }

    if( id == "id-foreclose_balance_from_miner")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        qDebug() << id << result;

        if(result.startsWith("\"result\":{"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Foreclose balance from miner successfully!"));
            commonDialog.pop();
        }

        return;
    }
}

void MinerPage::on_registerBtn_clicked()
{
    RegisterDialog registerDialog;
    registerDialog.pop();
}

void MinerPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));

    painter.drawRect(rect());
}

void MinerPage::init()
{
    if( UBChain::getInstance()->getRegisteredAccounts().isEmpty())
    {
        ui->tipLabel->show();
    }
    else
    {
        ui->tipLabel->hide();

        QStringList keys = UBChain::getInstance()->getRegisteredAccounts();
        ui->accountComboBox->addItems(keys);
    }
}

void MinerPage::getLockBalance()
{
    UBChain::getInstance()->postRPC( "id-get_account_lock_balance-" + ui->accountComboBox->currentText(),
                                     toJsonFormat( "get_account_lock_balance",QJsonArray() << ui->accountComboBox->currentText()));
}

void MinerPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    getLockBalance();
}

void MinerPage::on_lockToMinerBtn_clicked()
{
    if(ui->accountComboBox->currentText().isEmpty())    return;

    LockToMinerDialog lockToMinerDialog(ui->accountComboBox->currentText());
    lockToMinerDialog.pop();
}

void MinerPage::on_lockBalancesTableWidget_cellPressed(int row, int column)
{
    if(column == 3)
    {
        LockToMinerDialog lockToMinerDialog(ui->accountComboBox->currentText());
        lockToMinerDialog.setMiner(ui->lockBalancesTableWidget->item(row,0)->text());
        lockToMinerDialog.setAsset(ui->lockBalancesTableWidget->item(row,1)->text());
        lockToMinerDialog.pop();
        return;
    }

    if(column == 4)
    {
        ForecloseDialog forecloseDialog;
        QString amountStr = forecloseDialog.pop();

        UBChain::getInstance()->postRPC( "id-foreclose_balance_from_miner",
                                         toJsonFormat( "foreclose_balance_from_miner",
                                                       QJsonArray() << ui->lockBalancesTableWidget->item(row,0)->text()
                                                       << ui->accountComboBox->currentText()
                                                       << amountStr << ui->lockBalancesTableWidget->item(row,1)->text()
                                                       << true ));

        return;
    }
}
