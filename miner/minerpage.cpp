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


    ui->incomeTableWidget->installEventFilter(this);
    ui->incomeTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->incomeTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->incomeTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->incomeTableWidget->setFrameShape(QFrame::NoFrame);
    ui->incomeTableWidget->setMouseTracking(true);
    ui->incomeTableWidget->setShowGrid(false);//隐藏表格线

    ui->incomeTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->incomeTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->incomeTableWidget->horizontalHeader()->setVisible(true);
    ui->incomeTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->incomeTableWidget->setColumnWidth(0,140);
    ui->incomeTableWidget->setColumnWidth(1,140);
    ui->incomeTableWidget->setColumnWidth(2,80);

    init();
}

MinerPage::~MinerPage()
{
    delete ui;
}

void MinerPage::refresh()
{
    fetchLockBalance();
    fetchAccountIncome();
}

void MinerPage::jsonDataUpdated(QString id)
{
    if( id == "id-get_account_lock_balance-" + ui->accountComboBox->currentText())
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

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

    if( id == "id-get_address_pay_back_balance-" + ui->accountComboBox->currentText())
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonArray array = jsonObject.take("result").toArray();
        int size = array.size();
        ui->incomeTableWidget->setRowCount(0);
        ui->incomeTableWidget->setRowCount(size);

        for(int i = 0; i < size; i++)
        {
            QJsonObject object = array.at(i).toObject();
            QString assetId = object.take("asset_id").toString();
            unsigned long long amount = 0;
            QJsonValue value = object.take("amount");
            if(value.isString())
            {
                amount = value.toString().toULongLong();
            }
            else
            {
                amount = QString::number(value.toDouble(),'g',10).toULongLong();
            }

            AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(assetId);
            ui->incomeTableWidget->setItem(i,0,new QTableWidgetItem(assetInfo.symbol));

            ui->incomeTableWidget->setItem(i,1,new QTableWidgetItem(getBigNumberString(amount,assetInfo.precision)));

            ui->incomeTableWidget->setItem(i,2,new QTableWidgetItem(tr("obtain")));

        }


        return;
    }

    if( id == "id-obtain_pay_back_balance")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        qDebug() << id << result;

        if(result.startsWith("\"result\":{"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("The transaction of obtain-income has been sent out!"));
            commonDialog.pop();
        }

        return;
    }
}

void MinerPage::on_registerBtn_clicked()
{
    if(UBChain::getInstance()->getUnregisteredAccounts().isEmpty())
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("There are no unregistered accounts in the wallet!"));
        commonDialog.pop();
    }
    else
    {
        RegisterDialog registerDialog;
        registerDialog.pop();
    }

}

void MinerPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}

void MinerPage::init()
{
    if( UBChain::getInstance()->getRegisteredAccounts().isEmpty())
    {
        ui->tipLabel->show();
        ui->stackedWidget->hide();
        ui->incomeInfoBtn->hide();
        ui->forecloseInfoBtn->hide();
        ui->lockToMinerBtn->hide();
        ui->accountComboBox->hide();
        ui->accountLabel->hide();
    }
    else
    {
        ui->tipLabel->hide();
        ui->stackedWidget->show();
        ui->incomeInfoBtn->show();
        ui->forecloseInfoBtn->show();
        ui->lockToMinerBtn->show();
        ui->accountComboBox->show();
        ui->accountLabel->show();

        QStringList keys = UBChain::getInstance()->getRegisteredAccounts();
        ui->accountComboBox->addItems(keys);
    }

    ui->stackedWidget->setCurrentIndex(0);
}

void MinerPage::fetchLockBalance()
{
    UBChain::getInstance()->postRPC( "id-get_account_lock_balance-" + ui->accountComboBox->currentText(),
                                     toJsonFormat( "get_account_lock_balance",QJsonArray() << ui->accountComboBox->currentText()));
}

void MinerPage::fetchAccountIncome()
{
    QString address = UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
    UBChain::getInstance()->postRPC( "id-get_address_pay_back_balance-" + ui->accountComboBox->currentText(),
                                     toJsonFormat( "get_address_pay_back_balance",QJsonArray() << address << ""));
}

void MinerPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    fetchLockBalance();
    fetchAccountIncome();
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

void MinerPage::on_incomeTableWidget_cellPressed(int row, int column)
{
    if(column == 2)
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText(tr("Sure to obtain all the current income?"));
        if(commonDialog.pop())
        {
            QString address = UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
            UBChain::getInstance()->postRPC( "id-obtain_pay_back_balance",
                                             toJsonFormat( "obtain_pay_back_balance",
                                                           QJsonArray() << address
                                                           << ui->incomeTableWidget->item(row,1)->text()
                                                           << ui->incomeTableWidget->item(row,0)->text()
                                                           << true ));
        }

        return;
    }
}

void MinerPage::on_incomeInfoBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MinerPage::on_forecloseInfoBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
