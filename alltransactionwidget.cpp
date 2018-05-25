#include "alltransactionwidget.h"
#include "ui_alltransactionwidget.h"

#include "wallet.h"
#include "showcontentdialog.h"

AllTransactionWidget::AllTransactionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AllTransactionWidget)
{
    ui->setupUi(this);

    init();
}

AllTransactionWidget::~AllTransactionWidget()
{
    delete ui;
}

void AllTransactionWidget::init()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    ui->transactionsTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->transactionsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->transactionsTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->transactionsTableWidget->setFrameShape(QFrame::NoFrame);
    ui->transactionsTableWidget->setMouseTracking(true);
    ui->transactionsTableWidget->setShowGrid(false);//隐藏表格线

    ui->transactionsTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->transactionsTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->transactionsTableWidget->horizontalHeader()->setVisible(true);
    ui->transactionsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->transactionsTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);


    setStyleSheet("QToolButton{background:transparent;border:none;color: rgb(51,51,51);font:12px \"微软雅黑\";padding:0px 0px 1px 0px;}"
                  "QToolButton::checked{background-color:rgb(84,116,235);border-radius:12px;color:white;}");

    ui->searchLineEdit->setPlaceholderText(tr("address or transaction id"));


    ui->typeAllBtn->setCheckable(true);
    ui->typeTransferBtn->setCheckable(true);
    ui->typeDepositBtn->setCheckable(true);
    ui->typeWithdrawBtn->setCheckable(true);
    ui->typeContractBtn->setCheckable(true);
    ui->typeMineBtn->setCheckable(true);
    ui->typeGuaranteeBtn->setCheckable(true);
    ui->typeOtherBtn->setCheckable(true);
    ui->timeAllBtn->setCheckable(true);
    ui->timeDayBtn->setCheckable(true);
    ui->timeMonthBtn->setCheckable(true);
    ui->timeWeekBtn->setCheckable(true);

    ui->typeAllBtn->setChecked(true);
    ui->timeAllBtn->setChecked(true);

    ui->typeWithdrawBtn->adjustSize();
    ui->typeWithdrawBtn->setGeometry(ui->typeWithdrawBtn->x(),ui->typeWithdrawBtn->y(),ui->typeWithdrawBtn->width() < 50?50:ui->typeWithdrawBtn->width(),24);

    ui->accountComboBox->clear();
    QStringList accounts = UBChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    AccountInfo info = UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    ui->addressLabel->setText(info.address);

    inited = true;
}

void AllTransactionWidget::on_typeAllBtn_clicked()
{
    clearTypeChoice();
    ui->typeAllBtn->setChecked(true);
    typeChoice = AllType;
    showTransactions();
}

void AllTransactionWidget::on_typeTransferBtn_clicked()
{
    clearTypeChoice();
    ui->typeTransferBtn->setChecked(true);
    typeChoice = TransferType;
    showTransactions();
}

void AllTransactionWidget::on_typeDepositBtn_clicked()
{
    clearTypeChoice();
    ui->typeDepositBtn->setChecked(true);
    typeChoice = DepositType;
    showTransactions();
}

void AllTransactionWidget::on_typeWithdrawBtn_clicked()
{
    clearTypeChoice();
    ui->typeWithdrawBtn->setChecked(true);
    typeChoice = WithdrawType;
    showTransactions();
}

void AllTransactionWidget::on_typeContractBtn_clicked()
{
    clearTypeChoice();
    ui->typeContractBtn->setChecked(true);
    typeChoice = ContractType;
    showTransactions();
}

void AllTransactionWidget::on_typeMineBtn_clicked()
{
    clearTypeChoice();
    ui->typeMineBtn->setChecked(true);
    typeChoice = MineType;
    showTransactions();
}

void AllTransactionWidget::on_typeGuaranteeBtn_clicked()
{
    clearTypeChoice();
    ui->typeGuaranteeBtn->setChecked(true);
    typeChoice = GuaranteeType;
    showTransactions();
}

void AllTransactionWidget::on_typeOtherBtn_clicked()
{
    clearTypeChoice();
    ui->typeOtherBtn->setChecked(true);
    typeChoice = OtherType;
    showTransactions();
}

void AllTransactionWidget::on_timeAllBtn_clicked()
{
    clearTimeChoice();
    ui->timeAllBtn->setChecked(true);
    timeChoice = AllTime;
    showTransactions();
}

void AllTransactionWidget::on_timeDayBtn_clicked()
{
    clearTimeChoice();
    ui->timeDayBtn->setChecked(true);
    timeChoice = DayTime;
    showTransactions();
}

void AllTransactionWidget::on_timeWeekBtn_clicked()
{
    clearTimeChoice();
    ui->timeWeekBtn->setChecked(true);
    timeChoice = WeekTime;
    showTransactions();
}

void AllTransactionWidget::on_timeMonthBtn_clicked()
{
    clearTimeChoice();
    ui->timeMonthBtn->setChecked(true);
    timeChoice = MonthTime;
    showTransactions();
}

void AllTransactionWidget::clearTypeChoice()
{
    ui->typeAllBtn->setChecked(false);
    ui->typeTransferBtn->setChecked(false);
    ui->typeDepositBtn->setChecked(false);
    ui->typeWithdrawBtn->setChecked(false);
    ui->typeContractBtn->setChecked(false);
    ui->typeMineBtn->setChecked(false);
    ui->typeGuaranteeBtn->setChecked(false);
    ui->typeOtherBtn->setChecked(false);
}

void AllTransactionWidget::clearTimeChoice()
{
    ui->timeAllBtn->setChecked(false);
    ui->timeDayBtn->setChecked(false);
    ui->timeWeekBtn->setChecked(false);
    ui->timeMonthBtn->setChecked(false);
}

void AllTransactionWidget::hideFilteredTransactions()
{
    QString filterStr = ui->searchLineEdit->text().simplified();

    if(filterStr.isEmpty())     return;

    for(int i = 0; i < ui->transactionsTableWidget->rowCount(); i++)
    {
        if( !ui->transactionsTableWidget->item(i,2)->text().contains(filterStr)
                && !ui->transactionsTableWidget->item(i,5)->text().contains(filterStr))
        {
            ui->transactionsTableWidget->hideRow(i);
        }
        else
        {
            ui->transactionsTableWidget->showRow(i);
        }
    }
}

void AllTransactionWidget::showTransactions()
{
    TransactionTypeIds typeIds;

    switch (typeChoice)
    {
    case AllType:
        typeIds = UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text());
        break;
    case TransferType:
        typeIds = UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_NORMAL);
        break;
    case DepositType:
        typeIds = UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_DEPOSIT);
        break;
    case WithdrawType:
        typeIds = UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_WITHDRAW);
        break;
    case ContractType:
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_CONTRACT_REGISTER);
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_CONTRACT_INVOKE);
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_CONTRACT_TRANSFER);
        break;
    case MineType:
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_LOCKBALANCE);
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_FORECLOSE);
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_MINE_INCOME);
        break;
    case GuaranteeType:
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_CREATE_GUARANTEE);
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_CANCEL_GUARANTEE);
        break;
    case OtherType:
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_REGISTER_ACCOUNT);
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_BIND_TUNNEL);
        typeIds += UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_UNBIND_TUNNEL);
        break;
    default:
        break;
    }

    // 根据时间过滤
    int earliestBlock = 0;
    switch (timeChoice)
    {
    case AllTime:
        break;
    case DayTime:
        earliestBlock = UBChain::getInstance()->walletInfo.blockHeight - 12 * 60 * 24;
        break;
    case WeekTime:
        earliestBlock = UBChain::getInstance()->walletInfo.blockHeight - 12 * 60 * 24 * 7;
        break;
    case MonthTime:
        earliestBlock = UBChain::getInstance()->walletInfo.blockHeight - 12 * 60 * 24 * 30;
        break;
    default:
        break;
    }
    TransactionTypeIds filteredTypeIds;
    foreach (TransactionTypeId id, typeIds)
    {
        TransactionStruct ts = UBChain::getInstance()->transactionDB.getTransactionStruct(id.transactionId);
        if(ts.blockNum >= earliestBlock)
        {
            filteredTypeIds.append(id);
        }
    }


    // 根据区块高度排序
    TransactionTypeIds sortedTypeIds;
    for(int i = 0; i < filteredTypeIds.size(); i++)
    {
        if(sortedTypeIds.size() == 0)
        {
            sortedTypeIds.append(filteredTypeIds.at(i));
            continue;
        }

        TransactionStruct ts = UBChain::getInstance()->transactionDB.getTransactionStruct(filteredTypeIds.at(i).transactionId);
        for(int j = 0; j < sortedTypeIds.size(); j++)
        {
            TransactionStruct ts2 = UBChain::getInstance()->transactionDB.getTransactionStruct(sortedTypeIds.at(j).transactionId);
            if(ts.blockNum >= ts2.blockNum)
            {
                sortedTypeIds.insert(j,filteredTypeIds.at(i));
                break;
            }

            if(j == sortedTypeIds.size() - 1)
            {
                sortedTypeIds.append(filteredTypeIds.at(i));
                break;
            }
        }
    }

    int size = sortedTypeIds.size();
    ui->transactionsTableWidget->setRowCount(0);
    ui->transactionsTableWidget->setRowCount(size);


    for(int i = 0; i < size; i++)
    {
        QString transactionId = sortedTypeIds.at(i).transactionId;
        TransactionStruct ts = UBChain::getInstance()->transactionDB.getTransactionStruct(transactionId);
        if(ts.type == -1)
        {
            qDebug() << "can not find transaction in transactionstruct db: " << transactionId;
            continue;
        }

        ui->transactionsTableWidget->setItem(i,0, new QTableWidgetItem(QString::number(ts.blockNum)));
        ui->transactionsTableWidget->setItem(i,1, new QTableWidgetItem(ts.expirationTime));
        ui->transactionsTableWidget->setItem(i,4, new QTableWidgetItem(getBigNumberString(ts.feeAmount, ASSET_PRECISION)));
        ui->transactionsTableWidget->setItem(i,5, new QTableWidgetItem(transactionId));
        ui->transactionsTableWidget->setItem(i,6, new QTableWidgetItem(tr("confirmed")));


        QJsonObject operationObject = QJsonDocument::fromJson(ts.operationStr.toLatin1()).object();

        switch (ts.type)
        {
        case TRANSACTION_TYPE_NORMAL:
        {
            QJsonObject amountObject = operationObject.take("amount").toObject();
            unsigned long long amount = jsonValueToULL(amountObject.take("amount"));
            QString amountAssetId = amountObject.take("asset_id").toString();
            AssetInfo amountAssetInfo = UBChain::getInstance()->assetInfoMap.value(amountAssetId);

            QString fromAddress = operationObject.take("from_addr").toString();
            QString toAddress   = operationObject.take("to_addr").toString();
            if(fromAddress == ui->addressLabel->text())
            {
                ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(toAddress));

                if(toAddress == ui->addressLabel->text())    // 如果是自己转自己
                {
                    QTableWidgetItem* item = new QTableWidgetItem(getBigNumberString(amount, amountAssetInfo.precision) + " " + amountAssetInfo.symbol);
                    ui->transactionsTableWidget->setItem(i,3, item);
                    item->setTextColor(QColor(255,255,0));

                    ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("transfer to self")));
                }
                else
                {
                    // 如果是转出
                    QTableWidgetItem* item = new QTableWidgetItem( "- " + getBigNumberString(amount, amountAssetInfo.precision) + " " + amountAssetInfo.symbol);
                    ui->transactionsTableWidget->setItem(i,3, item);
                    item->setTextColor(QColor(255,0,0));

                    ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("transfer-out")));
                }
            }
            else
            {
                ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(fromAddress));

                // 如果是转入
                QTableWidgetItem* item = new QTableWidgetItem( "+ " + getBigNumberString(amount, amountAssetInfo.precision) + " " + amountAssetInfo.symbol);
                ui->transactionsTableWidget->setItem(i,3, item);
                item->setTextColor(QColor(0,255,0));

                ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("transfer-in")));
            }
        }
            break;
        case TRANSACTION_TYPE_REGISTER_ACCOUNT:
        {
            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem("-"));
            ui->transactionsTableWidget->setItem(i,3, new QTableWidgetItem("-"));
            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("register account")));
        }
            break;
        case TRANSACTION_TYPE_BIND_TUNNEL:
        {
            QString crosschainType = operationObject.take("crosschain_type").toString();
            QString tunnelAddress  = operationObject.take("tunnel_address").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(tr("bind %1 address: %2").arg(crosschainType).arg(tunnelAddress)));
            ui->transactionsTableWidget->setItem(i,3, new QTableWidgetItem("-"));
            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("bind tunnel address")));
        }
            break;
        case TRANSACTION_TYPE_UNBIND_TUNNEL:
        {
            QString crosschainType = operationObject.take("crosschain_type").toString();
            QString tunnelAddress  = operationObject.take("tunnel_address").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(tr("unbind %1 address: %2").arg(crosschainType).arg(tunnelAddress)));
            ui->transactionsTableWidget->setItem(i,3, new QTableWidgetItem("-"));
            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("unbind tunnel address")));
        }
            break;
        case TRANSACTION_TYPE_LOCKBALANCE:
        {
            unsigned long long lockAmount = jsonValueToULL(operationObject.take("lock_asset_amount"));
            QString lockAssetId = operationObject.take("lock_asset_id").toString();
            AssetInfo lockAssetInfo = UBChain::getInstance()->assetInfoMap.value(lockAssetId);
            QString minerId = operationObject.take("lockto_miner_account").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(UBChain::getInstance()->getMinerNameFromId(minerId)));

            QTableWidgetItem* item = new QTableWidgetItem( "- " + getBigNumberString(lockAmount, lockAssetInfo.precision) + " " + lockAssetInfo.symbol);
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(255,0,0));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("lock asset to miner")));
        }
            break;
        case TRANSACTION_TYPE_FORECLOSE:
        {
            unsigned long long forecloseAmount = jsonValueToULL(operationObject.take("foreclose_asset_amount"));
            QString forecloseAssetId = operationObject.take("foreclose_asset_id").toString();
            AssetInfo forecloseAssetInfo = UBChain::getInstance()->assetInfoMap.value(forecloseAssetId);
            QString minerId = operationObject.take("foreclose_miner_account").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(UBChain::getInstance()->getMinerNameFromId(minerId)));

            QTableWidgetItem* item = new QTableWidgetItem( "+ " + getBigNumberString(forecloseAmount, forecloseAssetInfo.precision) + " " + forecloseAssetInfo.symbol);
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(0,255,0));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("foreclose asset from miner")));
        }
            break;
        case TRANSACTION_TYPE_DEPOSIT:
        {
            QJsonObject crossChainTxObject  = operationObject.take("cross_chain_trx").toObject();
            QString     fromAccount         = crossChainTxObject.take("from_account").toString();
            QString     amountStr           = crossChainTxObject.take("amount").toString();
            QString     crosschainSymbol    = crossChainTxObject.take("asset_symbol").toString();
            QString     assetSymbol         = operationObject.take("asset_symbol").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(tr("%1 account: %2").arg(crosschainSymbol).arg(fromAccount)));

            QTableWidgetItem* item = new QTableWidgetItem( "+ " + amountStr + " " + assetSymbol);
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(0,255,0));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("deposit %1").arg(assetSymbol)));
        }
            break;
        case TRANSACTION_TYPE_WITHDRAW:
        {
            QString     crosschainAccount   = operationObject.take("crosschain_account").toString();
            QString     amountStr           = operationObject.take("amount").toString();
            QString     assetSymbol         = operationObject.take("asset_symbol").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(tr("%1 account: %2").arg(assetSymbol).arg(crosschainAccount)));

            QTableWidgetItem* item = new QTableWidgetItem( "- " + amountStr + " " + assetSymbol);
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(255,0,0));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("withdraw %1").arg(assetSymbol)));
        }
            break;
        case TRANSACTION_TYPE_MINE_INCOME:
        {
            QJsonObject amountObject = operationObject.take("pay_back_balance").toArray().at(0).toArray().at(1).toObject();
            unsigned long long amount = jsonValueToULL(amountObject.take("amount"));
            QString amountAssetId = amountObject.take("asset_id").toString();
            AssetInfo amountAssetInfo = UBChain::getInstance()->assetInfoMap.value(amountAssetId);

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem("-"));

            QTableWidgetItem* item = new QTableWidgetItem( "+" + getBigNumberString(amount, amountAssetInfo.precision) + " " + amountAssetInfo.symbol);
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(0,255,0));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("get mining income")));
        }
            break;
        case TRANSACTION_TYPE_CONTRACT_REGISTER:
        {
            QString contractId = operationObject.take("contract_id").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(contractId));

            QTableWidgetItem* item = new QTableWidgetItem( "-");
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(255,0,0));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("register contract")));
        }
            break;
        case TRANSACTION_TYPE_CONTRACT_INVOKE:
        {
            QString contractId = operationObject.take("contract_id").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(contractId));

            QTableWidgetItem* item = new QTableWidgetItem( "-");
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(255,0,0));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("call contract")));
        }
            break;
        case TRANSACTION_TYPE_CONTRACT_TRANSFER:
        {
            QString contractId          = operationObject.take("contract_id").toString();
            QJsonObject amountObject    = operationObject.take("amount").toObject();
            unsigned long long amount   = jsonValueToULL( amountObject.take("amount"));
            QString assetId             = amountObject.take("asset_id").toString();
            AssetInfo assetInfo         = UBChain::getInstance()->assetInfoMap.value(assetId);

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(contractId));

            QTableWidgetItem* item = new QTableWidgetItem( "- " + getBigNumberString(amount,assetInfo.precision) + " " + assetInfo.symbol);
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(255,0,0));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("transfer to contract")));
        }
            break;
        case TRANSACTION_TYPE_CREATE_GUARANTEE:
        {
            QJsonObject amountObject    = operationObject.take("asset_origin").toObject();
            unsigned long long amount   = jsonValueToULL( amountObject.take("amount"));
            QString assetId             = amountObject.take("asset_id").toString();
            AssetInfo assetInfo         = UBChain::getInstance()->assetInfoMap.value(assetId);
            QString acceptSymbol        = operationObject.take("symbol").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem("-"));

            QTableWidgetItem* item = new QTableWidgetItem( "- " + getBigNumberString(amount,assetInfo.precision) + " " + assetInfo.symbol);
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(255,0,0));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("create %1 acceptance").arg(acceptSymbol)));
        }
            break;
        case TRANSACTION_TYPE_CANCEL_GUARANTEE:
        {
            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem("-"));

            QTableWidgetItem* item = new QTableWidgetItem( "-");
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(255,0,0));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("cancel acceptance")));
        }
            break;
        default:
            break;
        }

    }
    //设置表格内容居中
    for(int i = 0;i < ui->transactionsTableWidget->rowCount();++i)
    {
        for(int j = 0;j < ui->transactionsTableWidget->columnCount();++j)
        {
            ui->transactionsTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }

    }

    hideFilteredTransactions();
}




void AllTransactionWidget::on_accountComboBox_currentTextChanged(const QString &arg1)
{
    UBChain::getInstance()->currentAccount = ui->accountComboBox->currentText();

    AccountInfo info = UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    ui->addressLabel->setText(info.address);

    showTransactions();
}

void AllTransactionWidget::on_transactionsTableWidget_cellPressed(int row, int column)
{
    if( column == 1  || column == 2 || column == 5 || column == 7)
    {
        ShowContentDialog showContentDialog( ui->transactionsTableWidget->item(row, column)->text(),this);

        int x = ui->transactionsTableWidget->columnViewportPosition(column) + ui->transactionsTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->transactionsTableWidget->rowViewportPosition(row) - 10 + ui->transactionsTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->transactionsTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

}

void AllTransactionWidget::on_searchBtn_clicked()
{
    hideFilteredTransactions();
}

void AllTransactionWidget::on_searchLineEdit_textChanged(const QString &arg1)
{
    hideFilteredTransactions();
}
