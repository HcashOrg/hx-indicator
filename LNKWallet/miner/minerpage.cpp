#include "minerpage.h"
#include "ui_minerpage.h"

#include <QPainter>
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
#include <mutex>

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
    ui->lockBalancesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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

    ui->incomeTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->incomeTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->incomeTableWidget->horizontalHeader()->setVisible(true);
    ui->incomeTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->incomeTableWidget->setColumnWidth(0,140);
    ui->incomeTableWidget->setColumnWidth(1,140);
    ui->incomeTableWidget->setColumnWidth(2,120);

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


    pageWidget_income = new PageScrollWidget();
    ui->stackedWidget_income->addWidget(pageWidget_income);
    pageWidget_fore = new PageScrollWidget();
    ui->stackedWidget_fore->addWidget(pageWidget_fore);
    pageWidget_record = new PageScrollWidget();
    ui->stackedWidget_record->addWidget(pageWidget_record);
    connect(pageWidget_income,&PageScrollWidget::currentPageChangeSignal,this,&MinerPage::pageChangeSlot);
    connect(pageWidget_fore,&PageScrollWidget::currentPageChangeSignal,this,&MinerPage::pageChangeSlot);
    connect(pageWidget_record,&PageScrollWidget::currentPageChangeSignal,this,&MinerPage::pageChangeSlot);

    blankWidget_income = new BlankDefaultWidget(ui->incomeTableWidget);
    blankWidget_income->setTextTip(tr("There's no income!"));
    blankWidget_fore = new BlankDefaultWidget(ui->lockBalancesTableWidget);
    blankWidget_fore->setTextTip(tr("There's no foreclose information!"));
    blankWidget_record = new BlankDefaultWidget(ui->incomeRecordTableWidget);
    blankWidget_record->setTextTip(tr("There's no income record!"));

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->incomeRecordTableWidget);
    HXChain::getInstance()->mainFrame->installBlurEffect(ui->incomeTableWidget);
    HXChain::getInstance()->mainFrame->installBlurEffect(ui->lockBalancesTableWidget);
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
            ui->lockBalancesTableWidget->setItem(i,1,new QTableWidgetItem(assetInfo.symbol));

            ui->lockBalancesTableWidget->setItem(i,2,new QTableWidgetItem(getBigNumberString(lockAmount,assetInfo.precision)));

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

    if( id == "id-foreclose_balance_from_miner")
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
            errorResultDialog.setInfoText(tr("Fail to foreclose asset from miner!"));
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

        checkObtainAllBtnVisible();

        return;
    }

    if( id == "id-obtain_pay_back_balance")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        qDebug() << id << result;

        if(result.startsWith("\"result\":{"))
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of obtain-income has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setDetailText(result);

            if(result.contains("pay_back_obj.second.amount >= min_payback_balance: doesnt get enough pay back"))
            {
                errorResultDialog.setInfoText(tr("This account's mining income is less than %1 %2 ! You can not obtain it.")
                                              .arg(0.6).arg(ASSET_NAME));
            }
            else
            {
                errorResultDialog.setInfoText(tr("Fail to obtain mining income!"));
            }

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
    checkObtainAllBtnVisible();
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
        QJsonObject amountObject = object.take("pay_back_balance").toArray().at(0).toArray().at(1).toObject();
        unsigned long long amount = jsonValueToULL(amountObject.take("amount"));
        QString amountAssetId = amountObject.take("asset_id").toString();
        AssetInfo amountAssetInfo = HXChain::getInstance()->assetInfoMap.value(amountAssetId);
        QJsonObject feeObject = object.take("fee").toObject();
        unsigned long long feeAmount = jsonValueToULL(feeObject.take("amount"));

        ui->incomeRecordTableWidget->setItem(i,0, new QTableWidgetItem(QString::number(ts.blockNum)));
        ui->incomeRecordTableWidget->setItem(i,1, new QTableWidgetItem( "+" + getBigNumberString(amount, amountAssetInfo.precision) + " " + amountAssetInfo.symbol));
        ui->incomeRecordTableWidget->item(i,1)->setTextColor(QColor(0,255,0));
        ui->incomeRecordTableWidget->setItem(i,2, new QTableWidgetItem(getBigNumberString(feeAmount, ASSET_PRECISION) + " " + ASSET_NAME));
        ui->incomeRecordTableWidget->setItem(i,3, new QTableWidgetItem(transactionId));
        ui->incomeRecordTableWidget->setItem(i,4, new QTableWidgetItem(tr("confirmed")));
    }
    tableWidgetSetItemZebraColor(ui->incomeRecordTableWidget);

    unsigned int curPage = pageWidget_record->GetCurrentPage();
    pageWidget_record->SetTotalPage(calPage(ui->incomeRecordTableWidget));
    pageWidget_record->SetCurrentPage(curPage);
    pageWidget_record->setShowTip(ui->incomeRecordTableWidget->rowCount(),ROWNUMBER);

    blankWidget_record->setVisible(0 == size);
    pageWidget_record->setVisible(0 != size);
}

void MinerPage::InitStyle()
{
    ui->forecloseInfoBtn->setCheckable(true);
    ui->incomeInfoBtn->setCheckable(true);
    ui->incomeRecordBtn->setCheckable(true);


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


    ui->obtainAllBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->lockToMinerBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->registerBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

}

void MinerPage::updateCheckState(int number)
{
    ui->incomeInfoBtn->setChecked(0 == number);
    ui->forecloseInfoBtn->setChecked(1 == number);
    ui->incomeRecordBtn->setChecked(2 == number);
}

void MinerPage::checkObtainAllBtnVisible()
{
    if(ui->stackedWidget->currentIndex() != 0 || ui->incomeTableWidget->rowCount() == 0)
    {
        ui->obtainAllBtn->hide();
        return;
    }

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
        LockToMinerDialog lockToMinerDialog(ui->accountComboBox->currentText());
        lockToMinerDialog.setMiner(ui->lockBalancesTableWidget->item(row,0)->text());
        lockToMinerDialog.setAsset(ui->lockBalancesTableWidget->item(row,1)->text());
        lockToMinerDialog.pop();
        return;
    }

    if(column == 4)
    {
        ForecloseDialog forecloseDialog(ui->accountComboBox->currentText());
        QString amountStr = forecloseDialog.pop();

        if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

        HXChain::getInstance()->postRPC( "id-foreclose_balance_from_miner",
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
        if(ui->incomeTableWidget->item(row,column)->text().isEmpty())       return;
        if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

        QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;

        FeeChargeWidget *feeCharge = new FeeChargeWidget(HXChain::getInstance()->feeChargeInfo.minerIncomeFee.toDouble(),
                                                         HXChain::getInstance()->feeType,ui->accountComboBox->currentText(),
                                                         HXChain::getInstance()->mainFrame);
        connect(feeCharge,&FeeChargeWidget::confirmSignal,[this,address,row](){
            QJsonArray array;
            QJsonArray array2;
            array2 << ui->incomeTableWidget->item(row,0)->text();
            QJsonObject object;
            QStringList amountStringList = this->ui->incomeTableWidget->item(row,1)->text().split(" ");
            object.insert("amount", ui->incomeTableWidget->item(row,1)->data(Qt::UserRole).toString());
            object.insert("asset_id", HXChain::getInstance()->getAssetId(amountStringList.at(1)));
            array2 << object;
            array << array2;
            HXChain::getInstance()->postRPC( "id-obtain_pay_back_balance",
                                             toJsonFormat( "obtain_pay_back_balance",
                                                           QJsonArray() << address
                                                           << array
                                                           << true ));
        });
        feeCharge->show();

        return;
    }
}

void MinerPage::on_incomeInfoBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    updateCheckState(0);

    checkObtainAllBtnVisible();
}

void MinerPage::on_forecloseInfoBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    updateCheckState(1);

    checkObtainAllBtnVisible();
}

void MinerPage::on_incomeRecordBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    updateCheckState(2);

    checkObtainAllBtnVisible();
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

    FeeChargeWidget *feeCharge = new FeeChargeWidget(HXChain::getInstance()->feeChargeInfo.minerIncomeFee.toDouble(),
                                                     HXChain::getInstance()->feeType,ui->accountComboBox->currentText(),
                                                     HXChain::getInstance()->mainFrame);
    connect(feeCharge,&FeeChargeWidget::confirmSignal,[this,address](){
        QJsonArray array;

        for(int i = 0; i < ui->incomeTableWidget->rowCount(); i++)
        {
            QJsonArray array2;
            array2 << ui->incomeTableWidget->item(i,0)->text();
            QJsonObject object;
            QStringList amountStringList = this->ui->incomeTableWidget->item(i,1)->text().split(" ");
            object.insert("amount", ui->incomeTableWidget->item(i,1)->data(Qt::UserRole).toString());
            object.insert("asset_id", HXChain::getInstance()->getAssetId(amountStringList.at(1)));
            array2 << object;
            array << array2;
        }

        HXChain::getInstance()->postRPC( "id-obtain_pay_back_balance",
                                         toJsonFormat( "obtain_pay_back_balance",
                                                       QJsonArray() << address
                                                       << array
                                                       << true ));
    });
    feeCharge->show();
}
