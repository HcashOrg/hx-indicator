#include "minerpage.h"
#include "ui_minerpage.h"

#include <QPainter>
#include <QScrollBar>
#include <mutex>
#include <QListView>
#include "wallet.h"
#include "registerdialog.h"
#include "locktominerdialog.h"
#include "foreclosedialog.h"
#include "commondialog.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/TransactionResultDialog.h"

#include "poundage/PageScrollWidget.h"
#include "ToolButtonWidget.h"
#include "depositpage/FeeChargeWidget.h"
#include "showcontentdialog.h"
#include "control/BlankDefaultWidget.h"


static const int ROWNUMBER = 6;
static std::mutex calMutex;
MinerPage::MinerPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MinerPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->lockBalancesTableWidget->installEventFilter(this);
    ui->lockBalancesTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->lockBalancesTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->lockBalancesTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->lockBalancesTableWidget->setFrameShape(QFrame::NoFrame);
    ui->lockBalancesTableWidget->setMouseTracking(true);
    ui->lockBalancesTableWidget->setShowGrid(false);//隐藏表格线

    ui->lockBalancesTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->lockBalancesTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->lockBalancesTableWidget->horizontalHeader()->setVisible(true);
    ui->lockBalancesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    //ui->lockBalancesTableWidget->setColumnCount(5);
    ui->lockBalancesTableWidget->setColumnWidth(0,160);
    ui->lockBalancesTableWidget->setColumnWidth(1,150);
    ui->lockBalancesTableWidget->setColumnWidth(2,160);
    ui->lockBalancesTableWidget->setColumnWidth(3,80);
    ui->lockBalancesTableWidget->setColumnWidth(4,80);


    ui->incomeTableWidget->installEventFilter(this);
    ui->incomeTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->incomeTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->incomeTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->incomeTableWidget->setFrameShape(QFrame::NoFrame);
    ui->incomeTableWidget->setMouseTracking(true);
    ui->incomeTableWidget->setShowGrid(false);//隐藏表格线
    ui->incomeTableWidget->verticalScrollBar()->setVisible(false);

    ui->incomeTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->incomeTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->incomeTableWidget->horizontalHeader()->setVisible(true);
//    ui->incomeTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->incomeTableWidget->setColumnWidth(0,220);
    ui->incomeTableWidget->setColumnWidth(1,220);
    ui->incomeTableWidget->setColumnWidth(2,209);

    ui->incomeRecordTableWidget->installEventFilter(this);
    ui->incomeRecordTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->incomeRecordTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->incomeRecordTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->incomeRecordTableWidget->setFrameShape(QFrame::NoFrame);
    ui->incomeRecordTableWidget->setMouseTracking(true);
    ui->incomeRecordTableWidget->setShowGrid(false);//隐藏表格线

    ui->incomeRecordTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->incomeRecordTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->incomeRecordTableWidget->horizontalHeader()->setVisible(true);
    ui->incomeRecordTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    ui->incomeRecordTableWidget->setColumnWidth(0,140);
//    ui->incomeRecordTableWidget->setColumnWidth(1,140);
//    ui->incomeRecordTableWidget->setColumnWidth(2,140);

    ui->citizenInfoTableWidget->installEventFilter(this);
    ui->citizenInfoTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->citizenInfoTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->citizenInfoTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->citizenInfoTableWidget->setFrameShape(QFrame::NoFrame);
    ui->citizenInfoTableWidget->setMouseTracking(true);
    ui->citizenInfoTableWidget->setShowGrid(false);//隐藏表格线

    ui->citizenInfoTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->citizenInfoTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->citizenInfoTableWidget->horizontalHeader()->setVisible(true);
    ui->citizenInfoTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    pageWidget_income = new PageScrollWidget();
    ui->stackedWidget_income->addWidget(pageWidget_income);
    pageWidget_fore = new PageScrollWidget();
    ui->stackedWidget_fore->addWidget(pageWidget_fore);
    pageWidget_record = new PageScrollWidget();
    ui->stackedWidget_record->addWidget(pageWidget_record);
    pageWidget_citizen = new PageScrollWidget();
    ui->stackedWidget_citizen->addWidget(pageWidget_citizen);
    connect(pageWidget_income,&PageScrollWidget::currentPageChangeSignal,this,&MinerPage::pageChangeSlot);
    connect(pageWidget_fore,&PageScrollWidget::currentPageChangeSignal,this,&MinerPage::pageChangeSlot);
    connect(pageWidget_record,&PageScrollWidget::currentPageChangeSignal,this,&MinerPage::pageChangeSlot);
    connect(pageWidget_citizen,&PageScrollWidget::currentPageChangeSignal,this,&MinerPage::pageChangeSlot);

    blankWidget_income = new BlankDefaultWidget(ui->incomeTableWidget);
    blankWidget_income->setTextTip(tr("There's no income!"));
    blankWidget_fore = new BlankDefaultWidget(ui->lockBalancesTableWidget);
    blankWidget_fore->setTextTip(tr("There's no foreclose information!"));
    blankWidget_record = new BlankDefaultWidget(ui->incomeRecordTableWidget);
    blankWidget_record->setTextTip(tr("There's no income record!"));
    blankWidget_citizen = new BlankDefaultWidget(ui->citizenInfoTableWidget);
    blankWidget_citizen->setTextTip(tr("There's no citizen!"));

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->incomeRecordTableWidget);
    HXChain::getInstance()->mainFrame->installBlurEffect(ui->incomeTableWidget);
    HXChain::getInstance()->mainFrame->installBlurEffect(ui->lockBalancesTableWidget);
    HXChain::getInstance()->mainFrame->installBlurEffect(ui->citizenInfoTableWidget);
    InitStyle();
    init();
}

MinerPage::~MinerPage()
{
    delete ui;
}

void MinerPage::refresh()
{
    updateAccounts();

    fetchLockBalance();
    fetchAccountIncome();
    showIncomeRecord();
    showCitizenInfo();

    HXChain::getInstance()->fetchCitizenPayBack();

//    count++;
//    if(count > 50)
//    {
//        autoObtain();
//        autoLockToCitizen();
//        count = 0;
//    }

}

void MinerPage::jsonDataUpdated(QString id)
{
    if( id == "id-get_account_lock_balance-" + ui->accountComboBox->currentText())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

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
            ui->lockBalancesTableWidget->setRowHeight(i,40);

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

            ui->lockBalancesTableWidget->setItem(i,0,new QTableWidgetItem(HXChain::getInstance()->getMinerNameFromId(minerId)));

            AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetId);
            ui->lockBalancesTableWidget->setItem(i,1,new QTableWidgetItem( revertERCSymbol( assetInfo.symbol)));
            ui->lockBalancesTableWidget->item(i,1)->setData(Qt::UserRole, assetId);

            ui->lockBalancesTableWidget->setItem(i,2,new QTableWidgetItem(getBigNumberString(lockAmount,assetInfo.precision)));
            ui->lockBalancesTableWidget->item(i,2)->setData(Qt::UserRole, lockAmount);

            ui->lockBalancesTableWidget->setItem(i,3,new QTableWidgetItem(tr("add")));

            ToolButtonWidget *buttonAdd = new ToolButtonWidget();
            buttonAdd->setText(ui->lockBalancesTableWidget->item(i,3)->text());
            ui->lockBalancesTableWidget->setCellWidget(i,3,buttonAdd);
            connect(buttonAdd,&ToolButtonWidget::clicked,std::bind(&MinerPage::on_lockBalancesTableWidget_cellPressed,this,i,3));

            ui->lockBalancesTableWidget->setItem(i,4,new QTableWidgetItem(tr("foreclose")));
            ToolButtonWidget *buttonfore = new ToolButtonWidget();
            buttonfore->setText(ui->lockBalancesTableWidget->item(i,4)->text());
            ui->lockBalancesTableWidget->setCellWidget(i,4,buttonfore);
            connect(buttonfore,&ToolButtonWidget::clicked,std::bind(&MinerPage::on_lockBalancesTableWidget_cellPressed,this,i,4));

            QString tooltip = getCitizenTooltip(ui->lockBalancesTableWidget->item(i,0)->text());
            for(int kk = 0;kk < 4; ++kk){
                ui->lockBalancesTableWidget->item(i,kk)->setToolTip(tooltip);
            }
        }

        tableWidgetSetItemZebraColor(ui->lockBalancesTableWidget);

        unsigned int curPage = pageWidget_fore->GetCurrentPage();
        pageWidget_fore->SetTotalPage(calPage(ui->lockBalancesTableWidget));
        pageWidget_fore->SetCurrentPage(curPage);

        pageWidget_fore->setShowTip(ui->lockBalancesTableWidget->rowCount(),ROWNUMBER);
        blankWidget_fore->setVisible(0 == size);
        pageWidget_fore->setVisible(0 != size);
        return;
    }

    if( id == "id-foreclose_balance_from_citizen")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        qDebug() << id << result;

        if(result.startsWith("\"result\":{"))
        {            
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of foreclose-asset has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Fail to foreclose asset from citizen!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        return;
    }

    if( id == "id-get_address_pay_back_balance-" + ui->accountComboBox->currentText())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

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
            ui->incomeTableWidget->setRowHeight(i,40);

            QJsonArray minerArray = array.at(i).toArray();
            QString miner = minerArray.at(0).toString();

            QJsonObject amountObject = minerArray.at(1).toObject();
            QString assetId = amountObject.value("asset_id").toString();
            unsigned long long amount = jsonValueToULL( amountObject.value("amount"));

            AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetId);
            ui->incomeTableWidget->setItem(i,0,new QTableWidgetItem(miner));

            ui->incomeTableWidget->setItem(i,1,new QTableWidgetItem(getBigNumberString(amount,assetInfo.precision) + " " + assetInfo.symbol));
            ui->incomeTableWidget->item(i,1)->setData(Qt::UserRole, QString::number(amount));

            if( amount > 0)
            {
                ui->incomeTableWidget->setItem(i,2,new QTableWidgetItem(tr("get income")));

                ToolButtonWidget *buttonInc = new ToolButtonWidget();
                buttonInc->setText(ui->incomeTableWidget->item(i,2)->text());
                buttonInc->setButtonFixSize(120,40);
                ui->incomeTableWidget->setCellWidget(i,2,buttonInc);
                connect(buttonInc,&ToolButtonWidget::clicked,std::bind(&MinerPage::on_incomeTableWidget_cellPressed,this,i,2));
            }
            else
            {
                ui->incomeTableWidget->setItem(i,2,new QTableWidgetItem(""));
            }


        }

        tableWidgetSetItemZebraColor(ui->incomeTableWidget);

        unsigned int curPage = pageWidget_income->GetCurrentPage();
        pageWidget_income->SetTotalPage(calPage(ui->incomeTableWidget));
        pageWidget_income->SetCurrentPage(curPage);

        pageWidget_income->setShowTip(ui->incomeTableWidget->rowCount(),ROWNUMBER);
        pageWidget_income->setVisible(0 != size);
        blankWidget_income->setVisible( 0 == size);

        checkBtnVisible();

        return;
    }

    if( id == "id-obtain_pay_back_balance")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        qDebug() << id << result;

        if(result.startsWith("\"result\":{"))
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of get-income has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setDetailText(result);

            if(result.contains("p_back.second.amount >= min_payback_balance"))
            {
                errorResultDialog.setInfoText(tr("This account's mining income is less than %1 %2 ! You can not get it.")
                                              .arg(500).arg(ASSET_NAME));
            }
            else
            {
                errorResultDialog.setInfoText(tr("Fail to get mining income!"));
            }

            errorResultDialog.pop();
        }

        return;
    }

    if( id == "MinerPage-lock_balance_to_citizen")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        qDebug() << id << result;
        return;
    }

    if( id == "MinerPage+foreclose_balance_from_citizens")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":{"))
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of foreclose-asset has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Fail to foreclose asset from citizen!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }
        return;
    }
}

void MinerPage::on_registerBtn_clicked()
{
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

    if(HXChain::getInstance()->getUnregisteredAccounts().isEmpty())
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
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));
    painter.drawRect(rect());

    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(243,241,250),Qt::SolidPattern));
    painter.drawRect(0,87,this->width(),24);
}

void MinerPage::init()
{
    updateAccounts();

    ui->stackedWidget->setCurrentIndex(0);
    updateCheckState(0);

    refresh();
    checkBtnVisible();
    ui->sortTypeComboBox->hide();
}

void MinerPage::updateAccounts()
{
    accountsUpdating = true;

    if( HXChain::getInstance()->getRegisteredAccounts().isEmpty())
    {
        ui->tipLabel->show();
        ui->stackedWidget->hide();
        ui->lockToMinerBtn->hide();
        ui->accountComboBox->hide();
    }
    else
    {
        ui->tipLabel->hide();
        ui->stackedWidget->show();
        ui->lockToMinerBtn->show();
        ui->accountComboBox->show();

        QStringList keys = HXChain::getInstance()->getRegisteredAccounts();
        ui->accountComboBox->clear();
        ui->accountComboBox->addItems(keys);

        if(keys.contains(HXChain::getInstance()->currentAccount))
        {
            ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
        }
    }

    accountsUpdating = false;
}

void MinerPage::fetchLockBalance()
{
    HXChain::getInstance()->postRPC( "id-get_account_lock_balance-" + ui->accountComboBox->currentText(),
                                     toJsonFormat( "get_account_lock_balance",QJsonArray() << ui->accountComboBox->currentText()));
}

void MinerPage::fetchAccountIncome()
{
    QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
    HXChain::getInstance()->postRPC( "id-get_address_pay_back_balance-" + ui->accountComboBox->currentText(),
                                     toJsonFormat( "get_address_pay_back_balance",QJsonArray() << address << ""));
}

void MinerPage::showIncomeRecord()
{
    QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
    TransactionTypeIds typeIds = HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(address,TRANSACTION_TYPE_MINE_INCOME);


    // 根据区块高度排序
    TransactionTypeIds sortedTypeIds;
    for(int i = 0; i < typeIds.size(); i++)
    {
        if(sortedTypeIds.size() == 0)
        {
            sortedTypeIds.append(typeIds.at(i));
            continue;
        }

        TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct(typeIds.at(i).transactionId);
        for(int j = 0; j < sortedTypeIds.size(); j++)
        {
            TransactionStruct ts2 = HXChain::getInstance()->transactionDB.getTransactionStruct(sortedTypeIds.at(j).transactionId);
            if(ts2.blockNum == 0)   continue;   // 未确认的交易放前面
            if(ts.blockNum >= ts2.blockNum || ts.blockNum == 0)
            {
                sortedTypeIds.insert(j,typeIds.at(i));
                break;
            }

            if(j == sortedTypeIds.size() - 1)
            {
                sortedTypeIds.append(typeIds.at(i));
                break;
            }
        }
    }

    int size = sortedTypeIds.size();
    ui->incomeRecordTableWidget->setRowCount(0);
    ui->incomeRecordTableWidget->setRowCount(size);

    int rowCount = 0;
    for(int i = 0; i < size; i++)
    {
        ui->incomeRecordTableWidget->setRowHeight(i,40);

        QString transactionId = typeIds.at(size - i - 1).transactionId;
        TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct(transactionId);
        if(ts.type == -1)
        {
            qDebug() << "can not find transaction in transactionstruct db: " << transactionId;
            continue;
        }


        QJsonObject object = QJsonDocument::fromJson(ts.operationStr.toLatin1()).object();

        QString payBackOwner = object.value("pay_back_owner").toString();
        QJsonArray balanceArray = object.value("pay_back_balance").toArray();

        AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value( ui->accountComboBox->currentText());
        QString guaranteeOrderOwnerAddress = HXChain::getInstance()->transactionDB.getGuaranteeOrder(ts.guaranteeId).ownerAddress;
        int useGuaranteeOrderType = checkUseGuaranteeOrderType(payBackOwner, accountInfo.address, guaranteeOrderOwnerAddress);
        if(useGuaranteeOrderType == 2)  continue;

        //        if(balanceArray.size() <= 0)    continue;
        unsigned long long amount = 0;
        for(int j = 0; j < balanceArray.size(); j++)
        {
            QJsonArray minerAmountArray = balanceArray.at(j).toArray();
            QJsonObject amountObject = minerAmountArray.at(1).toObject();
            amount += jsonValueToULL( amountObject.value("amount"));
        }
        QString str = "+" + getBigNumberString(amount, ASSET_PRECISION) + " " + ASSET_NAME;
        if(balanceArray.size() > 1)
        {
            str.prepend(tr("total "));
        }

        QJsonObject feeObject = object.value("fee").toObject();
        unsigned long long feeAmount = jsonValueToULL(feeObject.value("amount"));

        ui->incomeRecordTableWidget->setItem(rowCount,0, new QTableWidgetItem(QString::number(ts.blockNum)));
        ui->incomeRecordTableWidget->setItem(rowCount,1, new QTableWidgetItem(str));
        ui->incomeRecordTableWidget->item(rowCount,1)->setTextColor(QColor(0,170,0));
        ui->incomeRecordTableWidget->setItem(rowCount,2, new QTableWidgetItem(getBigNumberString(feeAmount, ASSET_PRECISION) + " " + ASSET_NAME));
        ui->incomeRecordTableWidget->setItem(rowCount,3, new QTableWidgetItem(transactionId));
        ui->incomeRecordTableWidget->setItem(rowCount,4, new QTableWidgetItem(tr("confirmed")));
        rowCount++;

    }
    ui->incomeTableWidget->setRowCount(rowCount);
    tableWidgetSetItemZebraColor(ui->incomeRecordTableWidget);

    unsigned int curPage = pageWidget_record->GetCurrentPage();
    pageWidget_record->SetTotalPage(calPage(ui->incomeRecordTableWidget));
    pageWidget_record->SetCurrentPage(curPage);
    pageWidget_record->setShowTip(ui->incomeRecordTableWidget->rowCount(),ROWNUMBER);

    blankWidget_record->setVisible(0 == rowCount);
    pageWidget_record->setVisible(0 != rowCount);
}

void MinerPage::showCitizenInfo()
{
    QStringList citizens = HXChain::getInstance()->minerMap.keys();

    QStringList sortedKeysByPledgeWeight;
    for(int i = 0; i < citizens.size(); i++)
    {
        if(sortedKeysByPledgeWeight.size() == 0)
        {
            sortedKeysByPledgeWeight.append(citizens.at(i));
            continue;
        }

        for(int j = 0; j < sortedKeysByPledgeWeight.size(); j++)
        {
            if(HXChain::getInstance()->minerMap.value(sortedKeysByPledgeWeight.at(j)).pledgeWeight
                    < HXChain::getInstance()->minerMap.value(citizens.at(i)).pledgeWeight)
            {
                sortedKeysByPledgeWeight.insert( j, citizens.at(i));
                break;
            }

            if(j == sortedKeysByPledgeWeight.size() - 1)
            {
                sortedKeysByPledgeWeight.append( citizens.at(i));
                break;
            }
        }
    }

    QStringList sortedKeys;     // 按权重排序
    for(int i = 0; i < sortedKeysByPledgeWeight.size(); i++)
    {
        if(sortedKeys.size() == 0)
        {
            sortedKeys.append(sortedKeysByPledgeWeight.at(i));
            continue;
        }

        switch (ui->sortTypeComboBox->currentIndex())
        {
        case 0:
            for(int j = 0; j < sortedKeys.size(); j++)
            {
                if(HXChain::getInstance()->minerMap.value(sortedKeys.at(j)).pledgeWeight
                        < HXChain::getInstance()->minerMap.value(sortedKeysByPledgeWeight.at(i)).pledgeWeight)
                {
                    sortedKeys.insert( j, sortedKeysByPledgeWeight.at(i));
                    break;
                }

                if(j == sortedKeys.size() - 1)
                {
                    sortedKeys.append( sortedKeysByPledgeWeight.at(i));
                    break;
                }
            }
            break;
        case 1:
            for(int j = 0; j < sortedKeys.size(); j++)
            {
                if(HXChain::getInstance()->minerMap.value(sortedKeys.at(j)).totalProduced
                        < HXChain::getInstance()->minerMap.value(sortedKeysByPledgeWeight.at(i)).totalProduced)
                {
                    sortedKeys.insert( j, sortedKeysByPledgeWeight.at(i));
                    break;
                }

                if(j == sortedKeys.size() - 1)
                {
                    sortedKeys.append( sortedKeysByPledgeWeight.at(i));
                    break;
                }
            }
            break;
        case 2:
            for(int j = 0; j < sortedKeys.size(); j++)
            {
                if(HXChain::getInstance()->minerMap.value(sortedKeys.at(j)).totalMissed
                        > HXChain::getInstance()->minerMap.value(sortedKeysByPledgeWeight.at(i)).totalMissed)
                {
                    sortedKeys.insert( j, sortedKeysByPledgeWeight.at(i));
                    break;
                }

                if(j == sortedKeys.size() - 1)
                {
                    sortedKeys.append( sortedKeysByPledgeWeight.at(i));
                    break;
                }
            }
            break;
        case 3:
            for(int j = 0; j < sortedKeys.size(); j++)
            {
                if(HXChain::getInstance()->minerMap.value(sortedKeys.at(j)).payBack
                        > HXChain::getInstance()->minerMap.value(sortedKeysByPledgeWeight.at(i)).payBack)
                {
                    sortedKeys.insert( j, sortedKeysByPledgeWeight.at(i));
                    break;
                }

                if(j == sortedKeys.size() - 1)
                {
                    sortedKeys.append( sortedKeysByPledgeWeight.at(i));
                    break;
                }
            }
            break;
        case 4:
            for(int j = 0; j < sortedKeys.size(); j++)
            {
                if(sortedKeys.at(j) > sortedKeysByPledgeWeight.at(i))
                {
                    sortedKeys.insert( j, sortedKeysByPledgeWeight.at(i));
                    break;
                }

                if(j == sortedKeys.size() - 1)
                {
                    sortedKeys.append( sortedKeysByPledgeWeight.at(i));
                    break;
                }
            }
            break;
        default:
            break;
        }


    }



    int size = sortedKeys.size();
    ui->citizenInfoTableWidget->setRowCount(0);
    ui->citizenInfoTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->citizenInfoTableWidget->setRowHeight(i,40);
        MinerInfo minerInfo = HXChain::getInstance()->minerMap.value(sortedKeys.at(i));
        ui->citizenInfoTableWidget->setItem(i,0, new QTableWidgetItem(sortedKeys.at(i)));
        ui->citizenInfoTableWidget->setItem(i,1, new QTableWidgetItem( toEasyRead(minerInfo.pledgeWeight, 5, 4)));
        ui->citizenInfoTableWidget->setItem(i,2, new QTableWidgetItem( QString::number(minerInfo.totalProduced)));
        ui->citizenInfoTableWidget->setItem(i,3, new QTableWidgetItem( QString::number(minerInfo.totalMissed)));
        ui->citizenInfoTableWidget->setItem(i,4, new QTableWidgetItem( QString::number(minerInfo.lastBlock)));
        if(minerInfo.payBack >= 0)
        {
            ui->citizenInfoTableWidget->setItem(i,5, new QTableWidgetItem( QString("%1 %").arg(minerInfo.payBack)));
        }
        else
        {
            ui->citizenInfoTableWidget->setItem(i,5, new QTableWidgetItem( tr("unknown")));
        }
        ui->citizenInfoTableWidget->setItem(i,6,new QTableWidgetItem(tr("VOTE")));

        ToolButtonWidget *buttonInc = new ToolButtonWidget();
        buttonInc->setText(ui->citizenInfoTableWidget->item(i,6)->text());
        buttonInc->setButtonFixSize(100,40);
        ui->citizenInfoTableWidget->setCellWidget(i,6,buttonInc);
        QString minerName = ui->citizenInfoTableWidget->item(i,0)->text();
        connect(buttonInc,&ToolButtonWidget::clicked,[minerName,this](){
            LockToMinerDialog lockToMinerDialog(ui->accountComboBox->currentText());
            lockToMinerDialog.setMiner(minerName);
            lockToMinerDialog.pop();
        });
    }

    tableWidgetSetItemZebraColor(ui->citizenInfoTableWidget);

    unsigned int curPage = pageWidget_citizen->GetCurrentPage();
    pageWidget_citizen->SetTotalPage(calPage(ui->citizenInfoTableWidget));
    pageWidget_citizen->SetCurrentPage(curPage);
    pageWidget_citizen->setShowTip(ui->citizenInfoTableWidget->rowCount(),ROWNUMBER);

    blankWidget_citizen->setVisible(0 == size);
    pageWidget_citizen->setVisible(0 != size);
}

void MinerPage::InitStyle()
{
    ui->forecloseInfoBtn->setCheckable(true);
    ui->incomeInfoBtn->setCheckable(true);
    ui->incomeRecordBtn->setCheckable(true);
    ui->citizenInfoBtn->setCheckable(true);

    setStyleSheet(PUSHBUTTON_CHECK_STYLE
                  TABLEWIDGET_STYLE_1);

    ui->incomeInfoBtn->adjustSize();
    ui->incomeInfoBtn->resize(ui->incomeInfoBtn->width(), 18);
    ui->forecloseInfoBtn->adjustSize();
    ui->forecloseInfoBtn->resize(ui->forecloseInfoBtn->width(), 18);
    ui->forecloseInfoBtn->move(ui->incomeInfoBtn->x() + ui->incomeInfoBtn->width() + 30, ui->forecloseInfoBtn->y());
    ui->incomeRecordBtn->adjustSize();
    ui->incomeRecordBtn->resize(ui->incomeRecordBtn->width(), 18);
    ui->incomeRecordBtn->move(ui->forecloseInfoBtn->x() + ui->forecloseInfoBtn->width() + 30, ui->incomeRecordBtn->y());
    ui->citizenInfoBtn->adjustSize();
    ui->citizenInfoBtn->resize(ui->citizenInfoBtn->width(), 18);
    ui->citizenInfoBtn->move(ui->incomeRecordBtn->x() + ui->incomeRecordBtn->width() + 30, ui->incomeRecordBtn->y());

    ui->obtainAllBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->lockToMinerBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->registerBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->forecloseAllBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
}

void MinerPage::updateCheckState(int number)
{
    ui->incomeInfoBtn->setChecked(0 == number);
    ui->forecloseInfoBtn->setChecked(1 == number);
    ui->incomeRecordBtn->setChecked(2 == number);
    ui->citizenInfoBtn->setChecked(3 == number);
}

void MinerPage::checkBtnVisible()
{
    if(ui->stackedWidget->currentIndex() != 0 || ui->incomeTableWidget->rowCount() == 0)
    {
        ui->obtainAllBtn->hide();
    }
    else
    {
        bool visible = false;
        for(int i = 0; i < ui->incomeTableWidget->rowCount(); i++)
        {
            QTableWidgetItem* item = ui->incomeTableWidget->item(i, 2);
            if(item)
            {
                if(!item->text().isEmpty())
                {
                    visible = true;
                }
            }
        }
        ui->obtainAllBtn->setVisible(visible);
    }

    if(ui->stackedWidget->currentIndex() != 1 || ui->lockBalancesTableWidget->rowCount() == 0)
    {
        ui->forecloseAllBtn->hide();
    }
    else
    {
        ui->forecloseAllBtn->show();
    }
}

void MinerPage::autoLockToCitizen()
{
    AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    unsigned long long amount = accountInfo.assetAmountMap.value( HXChain::getInstance()->getAssetId(ASSET_NAME)).amount;

    QStringList citizens;
    citizens << "taurus" << "sagittarius" << "serpens" << "hydrus" << "lacerta" << "monoceros" << "uma";
    QString randomCitizen = citizens.at(qrand() % citizens.size());

    HXChain::getInstance()->postRPC( "MinerPage-lock_balance_to_citizen",
                                     toJsonFormat( "lock_balance_to_citizen",
                                                   QJsonArray() << randomCitizen << ui->accountComboBox->currentText()
                                                   << getBigNumberString(amount,ASSET_PRECISION) << ASSET_NAME
                                                   << true ));
}

void MinerPage::autoObtain()
{
    QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;

    QJsonArray array;
    for(int i = 0; i < ui->incomeTableWidget->rowCount(); i++)
    {
        QJsonArray array2;
        array2 << ui->incomeTableWidget->item(i,0)->text();
        QJsonObject object;
        QStringList amountStringList = this->ui->incomeTableWidget->item(i,1)->text().split(" ");
        QString amountStr = ui->incomeTableWidget->item(i,1)->data(Qt::UserRole).toString();
        if(amountStr.toULongLong() < 1)   continue;
        object.insert("amount", amountStr);
        object.insert("asset_id", HXChain::getInstance()->getAssetId(amountStringList.at(1)));
        array2 << object;
        array << array2;
    }

    HXChain::getInstance()->postRPC( "MinerPage-obtain_pay_back_balance",
                                     toJsonFormat( "obtain_pay_back_balance",
                                                   QJsonArray() << address
                                                   << array
                                                   << true ));
}


unsigned int MinerPage::calPage(const QTableWidget * const table) const
{
    std::lock_guard<std::mutex> gurd(calMutex);
    if(!table) return 0;
    int page = (table->rowCount()%ROWNUMBER==0 && table->rowCount() != 0) ?
                table->rowCount()/ROWNUMBER : table->rowCount()/ROWNUMBER+1;
    qDebug()<<table->rowCount()<<page;
    return static_cast<unsigned int>(page);
}


void MinerPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(accountsUpdating)        return;

    HXChain::getInstance()->currentAccount = ui->accountComboBox->currentText();

    fetchLockBalance();
    fetchAccountIncome();
    qDebug() << "fetchAccountIncome ";
    showIncomeRecord();
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
        if(ui->lockBalancesTableWidget->item(row,0) == NULL || ui->lockBalancesTableWidget->item(row,1) == NULL)    return;

        LockToMinerDialog lockToMinerDialog(ui->accountComboBox->currentText());
        lockToMinerDialog.setMiner(ui->lockBalancesTableWidget->item(row,0)->text());
        lockToMinerDialog.setAsset( getRealAssetSymbol( ui->lockBalancesTableWidget->item(row,1)->text()));
        lockToMinerDialog.pop();
        return;
    }

    if(column == 4)
    {
        if(ui->lockBalancesTableWidget->item(row,0) == NULL || ui->lockBalancesTableWidget->item(row,1) == NULL
                || ui->lockBalancesTableWidget->item(row,2) == NULL)
            return;

        QString citizenName = ui->lockBalancesTableWidget->item(row,0)->text();
        QString assetSymbol = getRealAssetSymbol( ui->lockBalancesTableWidget->item(row,1)->text());
        ForecloseDialog forecloseDialog(ui->accountComboBox->currentText(), assetSymbol,
                                        ui->lockBalancesTableWidget->item(row,2)->text());
        if(!forecloseDialog.pop())  return;

        if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

        HXChain::getInstance()->postRPC( "id-foreclose_balance_from_citizen",
                                         toJsonFormat( "foreclose_balance_from_citizen",
                                                       QJsonArray() << citizenName
                                                       << ui->accountComboBox->currentText()
                                                       << forecloseDialog.amountStr << assetSymbol
                                                       << true ));

        return;
    }
}

void MinerPage::on_incomeTableWidget_cellPressed(int row, int column)
{
    if(column == 2)
    {
        if(ui->incomeTableWidget->item(row,0) == NULL || ui->incomeTableWidget->item(row,1) == NULL
                || ui->incomeTableWidget->item(row,2) == NULL)       return;
        if(!HXChain::getInstance()->ValidateOnChainOperation()) return;


        QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
        QString citizenName = ui->incomeTableWidget->item(row,0)->text();
        QStringList amountStringList = this->ui->incomeTableWidget->item(row,1)->text().split(" ");
        QString amountStr = ui->incomeTableWidget->item(row,1)->data(Qt::UserRole).toString();

//        FeeChargeWidget *feeCharge = new FeeChargeWidget(HXChain::getInstance()->feeChargeInfo.minerIncomeFee.toDouble(),
//                                                         HXChain::getInstance()->feeType,ui->accountComboBox->currentText(),
//                                                         HXChain::getInstance()->mainFrame);
//        connect(feeCharge,&FeeChargeWidget::confirmSignal,[this,address,row](){

        CommonDialog commonDialog(CommonDialog::YesOrNo);
        commonDialog.setText(tr("You need to pay %1 %2 for this operation. Continue?").arg(HXChain::getInstance()->feeChargeInfo.minerIncomeFee).arg(ASSET_NAME));
        if(commonDialog.pop())
        {
            QJsonArray array;
            QJsonArray array2;
            array2 << citizenName;
            QJsonObject object;
            object.insert("amount", amountStr);
            object.insert("asset_id", HXChain::getInstance()->getAssetId(amountStringList.at(1)));
            array2 << object;
            array << array2;
            HXChain::getInstance()->postRPC( "id-obtain_pay_back_balance",
                                             toJsonFormat( "obtain_pay_back_balance",
                                                           QJsonArray() << address
                                                           << array
                                                           << true ));
        }

//        });
//        feeCharge->show();

        return;
    }
}

void MinerPage::on_incomeInfoBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    updateCheckState(0);

    checkBtnVisible();
    ui->sortTypeComboBox->hide();
}

void MinerPage::on_forecloseInfoBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    updateCheckState(1);

    checkBtnVisible();
    ui->sortTypeComboBox->hide();
}

void MinerPage::on_incomeRecordBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    updateCheckState(2);

    checkBtnVisible();
    ui->sortTypeComboBox->hide();
}


void MinerPage::on_citizenInfoBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    updateCheckState(3);

    checkBtnVisible();
    ui->sortTypeComboBox->show();
}

void MinerPage::pageChangeSlot(unsigned int page)
{
    QTableWidget *table = nullptr;
    if(sender() == pageWidget_income)
    {//当前收益翻页
        table = ui->incomeTableWidget;
    }
    else if(sender() == pageWidget_fore)
    {
        table = ui->lockBalancesTableWidget;
    }
    else if(sender() == pageWidget_record)
    {
        table = ui->incomeRecordTableWidget;
    }
    else if(sender() == pageWidget_citizen)
    {
        table = ui->citizenInfoTableWidget;
    }
    if(!table) return;

    for(int i = 0;i < table->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            table->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            table->setRowHidden(i,false);
        }
        else
        {
            table->setRowHidden(i,true);
        }
    }
}

void MinerPage::on_incomeRecordTableWidget_cellPressed(int row, int column)
{
    if( column == 3 )
    {
        ShowContentDialog showContentDialog( ui->incomeRecordTableWidget->item(row, column)->text(),this);

        int x = ui->incomeRecordTableWidget->columnViewportPosition(column) + ui->incomeRecordTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->incomeRecordTableWidget->rowViewportPosition(row) - 10 + ui->incomeRecordTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->incomeRecordTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void MinerPage::on_obtainAllBtn_clicked()
{
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

    QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;

//    FeeChargeWidget *feeCharge = new FeeChargeWidget(HXChain::getInstance()->feeChargeInfo.minerIncomeFee.toDouble(),
//                                                     HXChain::getInstance()->feeType,ui->accountComboBox->currentText(),
//                                                     HXChain::getInstance()->mainFrame);
//    connect(feeCharge,&FeeChargeWidget::confirmSignal,[this,address](){

    CommonDialog commonDialog(CommonDialog::YesOrNo);
    commonDialog.setText(tr("You need to pay %1 %2 for this operation. Continue?").arg(HXChain::getInstance()->feeChargeInfo.minerIncomeFee).arg(ASSET_NAME));
    if(commonDialog.pop())
    {
        QJsonArray array;
        for(int i = 0; i < ui->incomeTableWidget->rowCount(); i++)
        {
            if(ui->incomeTableWidget->item(i,0) == NULL || ui->incomeTableWidget->item(i,1) == NULL)    continue;
            QJsonArray array2;
            array2 << ui->incomeTableWidget->item(i,0)->text();
            QJsonObject object;
            QStringList amountStringList = this->ui->incomeTableWidget->item(i,1)->text().split(" ");
            QString amountStr = ui->incomeTableWidget->item(i,1)->data(Qt::UserRole).toString();
            if(amountStr.toULongLong() < 1)   continue;
            object.insert("amount", amountStr);
            object.insert("asset_id", HXChain::getInstance()->getAssetId(amountStringList.at(1)));
            array2 << object;
            array << array2;
        }

        HXChain::getInstance()->postRPC( "id-obtain_pay_back_balance",
                                         toJsonFormat( "obtain_pay_back_balance",
                                                       QJsonArray() << address
                                                       << array
                                                       << true ));
    }
//    });
//    feeCharge->show();
}


void MinerPage::on_forecloseAllBtn_clicked()
{
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

    CommonDialog commonDialog(CommonDialog::YesOrNo);
    commonDialog.setText(tr("Sure to foreclose all assets that you have voted?"));
    if(commonDialog.pop())
    {
        QMap<QString,QJsonArray>    map;
        for(int i = 0; i < ui->lockBalancesTableWidget->rowCount(); i++)
        {
            QJsonObject object;
            QString assetId = ui->lockBalancesTableWidget->item(i,1)->data(Qt::UserRole).toString();
            unsigned long long amount = ui->lockBalancesTableWidget->item(i,2)->data(Qt::UserRole).toULongLong();
            object.insert("amount", QString::number(amount));
            object.insert("asset_id", assetId);

            QString citizen = ui->lockBalancesTableWidget->item(i,0)->text();
            map[citizen] << object;
        }

        QJsonArray array;
        foreach(QString key, map.keys())
        {
            QJsonArray array2;
            array2 << key << map.value(key);
            array << array2;
        }

        HXChain::getInstance()->postRPC( "MinerPage+foreclose_balance_from_citizens",
                                         toJsonFormat( "foreclose_balance_from_citizens",
                                                       QJsonArray() << ui->accountComboBox->currentText()
                                                       << array
                                                       << true ));

        qDebug() << toJsonFormat( "foreclose_balance_from_citizens",
                                  QJsonArray() << ui->accountComboBox->currentText()
                                  << array
                                  << true );
    }


}


void MinerPage::on_sortTypeComboBox_currentIndexChanged(const QString &arg1)
{
    showCitizenInfo();
}

QString MinerPage::getCitizenTooltip(const QString &citizenName)
{
    if(!HXChain::getInstance()->minerMap.contains(citizenName)) return "";
    MinerInfo citizenInfo = HXChain::getInstance()->minerMap.value(citizenName);
    QString name = citizenName;
    QString fee = QString::number(citizenInfo.payBack) + "%";
    QString lastBl = QString::number(citizenInfo.lastBlock);
    QString weight = getBigNumberString(citizenInfo.pledgeWeight,0);
    QString total = QString::number(citizenInfo.totalProduced);
    QString miss = QString::number(citizenInfo.totalMissed);
//    modifyStringLength(name,total,true);
//    modifyStringLength(fee,miss,true);
//    modifyStringLength(weight,lastBl,true);


    QString nameTi = tr("Citizen:");
    QString feeTi = tr("Fee:");
    QString weightTi = tr("Weight:");
    QString lastBlTi = tr("LastProduce:");
    QString missTi = tr("Missed:");
    QString totalTi = tr("Total:");
//    modifyStringLength(nameTi,totalTi,false);
//    modifyStringLength(feeTi,missTi,false);
//    modifyStringLength(weightTi,lastBlTi,false);


    QString tipTemplate = "%1%2     %3%4\n%5%6    %7%8\n%9%10    %11%12";
    QString tooltip = tipTemplate.arg(nameTi).arg(name).arg(feeTi).arg(fee).arg(weightTi).arg(weight)
                                 .arg(totalTi).arg(total).arg(missTi).arg(miss).arg(lastBlTi).arg(lastBl);
    return tooltip;
}

void MinerPage::modifyStringLength(QString &first, QString &second, bool fillAppend)
{
    int Len = std::max<int>(first.length(),second.length());
    if(first.length() < Len)
    {
        int fitstLen = first.length();
        for(int i = 0;i < Len-fitstLen;++i)
        {
            if(fillAppend)
            {
                first.append(" ");
            }
            else
            {
                first.insert(0," ");
            }
        }
    }
    else if(second.length() < Len)
    {
        int fitstLen = second.length();
        for(int i = 0;i < Len-fitstLen;++i)
        {
            if(fillAppend)
            {
                second.append(" ");
            }
            else
            {
                second.insert(0," ");
            }
        }
    }
}
