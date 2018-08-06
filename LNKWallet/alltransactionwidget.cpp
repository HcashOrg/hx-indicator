#include "alltransactionwidget.h"
#include "ui_alltransactionwidget.h"

#include "wallet.h"
#include "showcontentdialog.h"
#include "control/BlankDefaultWidget.h"
#include "poundage/PageScrollWidget.h"

static const int ROWNUMBER = 5;
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
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);



    ui->transactionsTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->transactionsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->transactionsTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->transactionsTableWidget->setFrameShape(QFrame::NoFrame);
    ui->transactionsTableWidget->setMouseTracking(true);
    ui->transactionsTableWidget->setShowGrid(false);//隐藏表格线

    ui->transactionsTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->transactionsTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->transactionsTableWidget->horizontalHeader()->setVisible(true);
    ui->transactionsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->transactionsTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    setStyleSheet("QPushButton[checkable=\"true\"]{font:8px \"微软雅黑\";background:transparent;border:1px solid rgb(137,129,161);border-radius:9px;color: rgb(137,129,161);}"
                  "QPushButton::checked{background-color:rgb(83,61,138);color:white;}");

    ui->searchLineEdit->setPlaceholderText(tr("address or transaction id"));

    pageWidget = new PageScrollWidget();
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&AllTransactionWidget::pageChangeSlot);

    blankWidget = new BlankDefaultWidget(ui->transactionsTableWidget);
    blankWidget->setTextTip(tr("There's no record!"));


    ui->typeAllBtn->setCheckable(true);
    ui->typeTransferBtn->setCheckable(true);
    ui->typeDepositBtn->setCheckable(true);
    ui->typeWithdrawBtn->setCheckable(true);
    ui->typeContractBtn->setCheckable(true);
    ui->typeMineBtn->setCheckable(true);
    ui->typeGuaranteeBtn->setCheckable(true);
    ui->typeOtherBtn->setCheckable(true);
    ui->typeFeedPriceBtn->setCheckable(true);
    ui->typeProposalBtn->setCheckable(true);
    ui->typeBonusBtn->setCheckable(true);

    ui->timeAllBtn->setCheckable(true);
    ui->timeDayBtn->setCheckable(true);
    ui->timeMonthBtn->setCheckable(true);
    ui->timeWeekBtn->setCheckable(true);

    ui->typeAllBtn->setChecked(true);
    ui->timeAllBtn->setChecked(true);

//    ui->typeWithdrawBtn->adjustSize();
//    ui->typeWithdrawBtn->setGeometry(ui->typeWithdrawBtn->x(),ui->typeWithdrawBtn->y(),ui->typeWithdrawBtn->width() < 50?50:ui->typeWithdrawBtn->width(),24);

    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    ui->addressLabel->setText(info.address);

    inited = true;

    on_accountComboBox_currentTextChanged(ui->accountComboBox->currentText());

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->transactionsTableWidget);
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

void AllTransactionWidget::on_typeFeedPriceBtn_clicked()
{
    clearTypeChoice();
    ui->typeFeedPriceBtn->setChecked(true);
    typeChoice = FeedPriceType;
    showTransactions();
}


void AllTransactionWidget::on_typeProposalBtn_clicked()
{
    clearTypeChoice();
    ui->typeProposalBtn->setChecked(true);
    typeChoice = ProposalType;
    showTransactions();
}


void AllTransactionWidget::on_typeBonusBtn_clicked()
{
    clearTypeChoice();
    ui->typeBonusBtn->setChecked(true);
    typeChoice = BonusType;
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
    ui->typeFeedPriceBtn->setChecked(false);
    ui->typeProposalBtn->setChecked(false);
    ui->typeBonusBtn->setChecked(false);
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
    showRows.clear();
    QString filterStr = ui->searchLineEdit->text().simplified();

    for(int i = 0; i < ui->transactionsTableWidget->rowCount(); i++)
    {
        if(ui->transactionsTableWidget->item(i,2) == NULL || ui->transactionsTableWidget->item(i,5) == NULL)
        {
            continue;
        }

        if( !ui->transactionsTableWidget->item(i,2)->text().contains(filterStr)
                && !ui->transactionsTableWidget->item(i,5)->text().contains(filterStr))
        {
            ui->transactionsTableWidget->hideRow(i);
        }
        else
        {
            ui->transactionsTableWidget->showRow(i);
            showRows.push_back(i);
        }
    }

    int page = (showRows.size()%ROWNUMBER==0 && showRows.size() != 0) ?
                showRows.size()/ROWNUMBER : showRows.size()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(showRows.size(),ROWNUMBER);
    pageChangeSlot(0);
    pageWidget->setVisible(showRows.size() != 0);

    blankWidget->setVisible(showRows.size() == 0);

}

void AllTransactionWidget::showTransactions()
{
    TransactionTypeIds typeIds;

    switch (typeChoice)
    {
    case AllType:
        typeIds = HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text());
        break;
    case TransferType:
        typeIds = HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_NORMAL);
        break;
    case DepositType:
        typeIds = HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_DEPOSIT);
        break;
    case WithdrawType:
        typeIds = HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_WITHDRAW);
        break;
    case ContractType:
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_CONTRACT_REGISTER);
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_CONTRACT_INVOKE);
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_CONTRACT_TRANSFER);
        break;
    case MineType:
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_LOCKBALANCE);
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_FORECLOSE);
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_MINE_INCOME);
        break;
    case GuaranteeType:
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_CREATE_GUARANTEE);
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_CANCEL_GUARANTEE);
        break;
    case OtherType:
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_REGISTER_ACCOUNT);
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_BIND_TUNNEL);
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_UNBIND_TUNNEL);
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_ISSUE_ASSET);
        break;
    case FeedPriceType:
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_FEED_PRICE);
        break;
    case ProposalType:
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_SPONSOR_PROPOSAL);
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_PROPOSAL_APPROVE);
        break;
    case BonusType:
        typeIds += HXChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(ui->addressLabel->text(), TRANSACTION_TYPE_OBTAIN_BONUS);
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
        earliestBlock = HXChain::getInstance()->walletInfo.blockHeight - 12 * 60 * 24;
        break;
    case WeekTime:
        earliestBlock = HXChain::getInstance()->walletInfo.blockHeight - 12 * 60 * 24 * 7;
        break;
    case MonthTime:
        earliestBlock = HXChain::getInstance()->walletInfo.blockHeight - 12 * 60 * 24 * 30;
        break;
    default:
        break;
    }
    TransactionTypeIds filteredTypeIds;
    foreach (TransactionTypeId id, typeIds)
    {
        TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct(id.transactionId);
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

        TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct(filteredTypeIds.at(i).transactionId);
        for(int j = 0; j < sortedTypeIds.size(); j++)
        {
            TransactionStruct ts2 = HXChain::getInstance()->transactionDB.getTransactionStruct(sortedTypeIds.at(j).transactionId);
            if(ts2.blockNum == 0)   continue;   // 未确认的交易放前面
            if(ts.blockNum >= ts2.blockNum || ts.blockNum == 0)
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
        ui->transactionsTableWidget->setRowHeight(i,40);

        QString transactionId = sortedTypeIds.at(i).transactionId;
        TransactionStruct ts = HXChain::getInstance()->transactionDB.getTransactionStruct(transactionId);
        if(ts.type == -1)
        {
            qDebug() << "can not find transaction in transactionstruct db: " << transactionId;
            continue;
        }

        ui->transactionsTableWidget->setItem(i,0, new QTableWidgetItem(QString::number(ts.blockNum)));
        ui->transactionsTableWidget->setItem(i,1, new QTableWidgetItem(ts.expirationTime.replace("T", " ")));
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
            AssetInfo amountAssetInfo = HXChain::getInstance()->assetInfoMap.value(amountAssetId);

            QString fromAddress = operationObject.take("from_addr").toString();
            QString toAddress   = operationObject.take("to_addr").toString();
            if(fromAddress == ui->addressLabel->text())
            {
                ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(toAddress));

                if(toAddress == ui->addressLabel->text())    // 如果是自己转自己
                {
                    QTableWidgetItem* item = new QTableWidgetItem(getBigNumberString(amount, amountAssetInfo.precision) + " " + amountAssetInfo.symbol);
                    ui->transactionsTableWidget->setItem(i,3, item);
                    item->setTextColor(QColor(202,135,0));

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
                item->setTextColor(QColor(0,170,0));

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
        case TRANSACTION_TYPE_FEED_PRICE:
        {
            QJsonObject feedObject = operationObject.value("feed").toObject();
            QJsonObject settlementObject = feedObject.value("settlement_price").toObject();
            QJsonObject baseObject = settlementObject.value("base").toObject();
            QString     baseAssetid = baseObject.value("asset_id").toString();
            unsigned long long baseAmount = jsonValueToULL(baseObject.value("amount"));
            QJsonObject quoteObject = settlementObject.value("quote").toObject();
            unsigned long long quoteAmount = jsonValueToULL(quoteObject.value("amount"));

            AssetInfo info = HXChain::getInstance()->assetInfoMap.value(baseAssetid);
            QString str = tr("asset feed price: %1:%2  %3:%4").arg(info.symbol).arg(ASSET_NAME).arg( getBigNumberString(quoteAmount, ASSET_PRECISION))
                    .arg( getBigNumberString(baseAmount, info.precision));

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(str));
            ui->transactionsTableWidget->setItem(i,3, new QTableWidgetItem("-"));
            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("asset feed price")));
        }
            break;
        case TRANSACTION_TYPE_SPONSOR_PROPOSAL:
        {
            QJsonObject proposedOpObject = operationObject.value("proposed_ops").toArray().at(0).toObject();
            int opType = proposedOpObject.value("op").toArray().at(0).toInt();
            QString str = tr("type: ");
            switch (opType)
            {
            case TRANSACTION_TYPE_COLDHOT:
                str += tr("cold-hot wallet trx");
                break;
            case TRANSACTION_TYPE_CHANGE_ASSET_ACCOUNT:
                str += tr("change cold-hot wallet multisig-address");
                break;
            case TRANSACTION_TYPE_SET_PUBLISHER:
                str += tr("set price feeder");
                break;
            case TRANSACTION_TYPE_COLDHOT_CANCEL:
                str += tr("cancel cold-hot trx");
                break;
            case TRANSACTION_TYPE_WITHDRAW_CANCEL:
                str += tr("cancel withdraw trx");
                break;
            case TRANSACTION_TYPE_CREATE_GUARD:
                str += tr("create senator");
                break;
            case TRANSACTION_TYPE_FORMAL_GUARD:
                str += tr("set senator formal or not");
                break;
            case TRANSACTION_TYPE_RESIGN_GUARD:
                str += tr("resign senator");
                break;
            case TRANSACTION_TYPE_PROPOSAL_CONTRACT_TRANSFER_FEE:
                str += tr("set contract transfer fee");
                break;
            default:
                str += tr("%1 (unkown)").arg(opType);
                break;
            }

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(str));
            ui->transactionsTableWidget->setItem(i,3, new QTableWidgetItem("-"));
            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("sponsor a proposal")));
        }
            break;
        case TRANSACTION_TYPE_PROPOSAL_APPROVE:
        {
            QString proposalId = operationObject.value("proposal").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(tr("proposal ID: %1").arg(proposalId)));
            ui->transactionsTableWidget->setItem(i,3, new QTableWidgetItem("-"));
            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("vote for a proposal")));
        }
            break;
        case TRANSACTION_TYPE_LOCKBALANCE:
        {
            unsigned long long lockAmount = jsonValueToULL(operationObject.take("lock_asset_amount"));
            QString lockAssetId = operationObject.take("lock_asset_id").toString();
            AssetInfo lockAssetInfo = HXChain::getInstance()->assetInfoMap.value(lockAssetId);
            QString minerId = operationObject.take("lockto_miner_account").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(HXChain::getInstance()->getMinerNameFromId(minerId)));

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
            AssetInfo forecloseAssetInfo = HXChain::getInstance()->assetInfoMap.value(forecloseAssetId);
            QString minerId = operationObject.take("foreclose_miner_account").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(HXChain::getInstance()->getMinerNameFromId(minerId)));

            QTableWidgetItem* item = new QTableWidgetItem( "+ " + getBigNumberString(forecloseAmount, forecloseAssetInfo.precision) + " " + forecloseAssetInfo.symbol);
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(0,170,0));

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
            item->setTextColor(QColor(0,170,0));

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
            QJsonArray balanceArray = operationObject.value("pay_back_balance").toArray();
            if(balanceArray.size() > 0)
            {
                unsigned long long amount = 0;
                for(int i = 0; i < balanceArray.size(); i++)
                {
                    QJsonArray minerAmountArray = balanceArray.at(i).toArray();
                    QJsonObject amountObject = minerAmountArray.at(1).toObject();
                    amount += jsonValueToULL( amountObject.value("amount"));
                }

                QString str = "+" + getBigNumberString(amount, ASSET_PRECISION) + " " + ASSET_NAME;
                if(balanceArray.size() > 1)
                {
                    str.prepend(tr("total "));
                }

                QTableWidgetItem* item = new QTableWidgetItem( str);
                ui->transactionsTableWidget->setItem(i,3, item);
                item->setTextColor(QColor(0,170,0));
            }

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem("-"));
            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("get mining income")));
        }
            break;
        case TRANSACTION_TYPE_ISSUE_ASSET:
        {
            QString assetSymbol = operationObject.take("symbol").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(tr("issue \"%1\"").arg(assetSymbol)));
            ui->transactionsTableWidget->setItem(i,3, new QTableWidgetItem("-"));
            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("issue asset")));
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
            QString contractAPI = operationObject.take("contract_api").toString();
            QString arguments = operationObject.take("contract_arg").toString();

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem(contractId));

            if(contractAPI == "withdrawAsset")
            {
                 QString assetSymbol    = arguments.split(",").at(0);
                 QString amountStr      = arguments.split(",").at(1);

                 AssetInfo info  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(assetSymbol));
                 QTableWidgetItem* item = new QTableWidgetItem( QString("+ %1 %2").arg( getBigNumberString(amountStr.toULongLong(), info.precision)).arg(assetSymbol));
                 ui->transactionsTableWidget->setItem(i,3, item);
                 item->setTextColor(QColor(0,170,0));

                 ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("exechange contract withdrawAsset")));
            }
            else
            {
                QTableWidgetItem* item = new QTableWidgetItem( "-");
                ui->transactionsTableWidget->setItem(i,3, item);
                item->setTextColor(QColor(255,0,0));

                ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("call contract")));
            }
        }
            break;
        case TRANSACTION_TYPE_CONTRACT_TRANSFER:
        {
            QString contractId          = operationObject.take("contract_id").toString();
            QJsonObject amountObject    = operationObject.take("amount").toObject();
            unsigned long long amount   = jsonValueToULL( amountObject.take("amount"));
            QString assetId             = amountObject.take("asset_id").toString();
            AssetInfo assetInfo         = HXChain::getInstance()->assetInfoMap.value(assetId);

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
            AssetInfo assetInfo         = HXChain::getInstance()->assetInfoMap.value(assetId);
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
        case TRANSACTION_TYPE_OBTAIN_BONUS:
        {
            QJsonArray bonusBalanceArray = operationObject.value("bonus_balance").toArray();
            QString str;
            foreach (QJsonValue v, bonusBalanceArray)
            {
                QJsonArray assetAmountArray = v.toArray();
                QString assetSymbol = assetAmountArray.at(0).toString();
                unsigned long long amount = jsonValueToULL( assetAmountArray.at(1));
                AssetInfo amountAssetInfo = HXChain::getInstance()->assetInfoMap.value( HXChain::getInstance()->getAssetId( assetSymbol));
                str = "+" + getBigNumberString(amount, amountAssetInfo.precision) + " " + assetSymbol + "  ";
            }
            QTableWidgetItem* item = new QTableWidgetItem( str);
            ui->transactionsTableWidget->setItem(i,3, item);
            item->setTextColor(QColor(0,170,0));

            ui->transactionsTableWidget->setItem(i,2, new QTableWidgetItem("-"));

            ui->transactionsTableWidget->setItem(i,7, new QTableWidgetItem(tr("get bonus")));
        }
            break;
        default:
            break;
        }

    }

    tableWidgetSetItemZebraColor(ui->transactionsTableWidget);

    hideFilteredTransactions();
}




void AllTransactionWidget::on_accountComboBox_currentTextChanged(const QString &arg1)
{
    if(!inited)     return;

    HXChain::getInstance()->currentAccount = ui->accountComboBox->currentText();

    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    ui->addressLabel->setText(info.address);

    showTransactions();
}

void AllTransactionWidget::on_transactionsTableWidget_cellPressed(int row, int column)
{
    if( column == 1  || column == 2 || column == 3 || column == 5 || column == 7)
    {
        if( !ui->transactionsTableWidget->item(row, column))    return;

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

void AllTransactionWidget::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < showRows.size();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->transactionsTableWidget->setRowHidden(showRows[i],true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->transactionsTableWidget->setRowHidden(showRows[i],false);
        }
        else
        {
            ui->transactionsTableWidget->setRowHidden(showRows[i],true);
        }
    }

}

void AllTransactionWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(222,218,236),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(243,241,250),Qt::SolidPattern));

    painter.drawRect(0,90,this->width(),98);
}



