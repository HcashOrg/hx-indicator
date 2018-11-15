#include "ProposalDetailDialog.h"
#include "ui_ProposalDetailDialog.h"

#include "wallet.h"

ProposalDetailDialog::ProposalDetailDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProposalDetailDialog)
{
    ui->setupUi(this);

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->closeBtn->setStyleSheet(CANCELBTN_STYLE);

    ui->voteStateTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);
    ui->voteStateTableWidget->installEventFilter(this);
    ui->voteStateTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->voteStateTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->voteStateTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->voteStateTableWidget->setFrameShape(QFrame::NoFrame);
    ui->voteStateTableWidget->setMouseTracking(true);
    ui->voteStateTableWidget->setShowGrid(false);//隐藏表格线

    ui->voteStateTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->voteStateTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->voteStateTableWidget->horizontalHeader()->setVisible(true);
    ui->voteStateTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->voteStateTableWidget->setColumnWidth(0,300);
    ui->voteStateTableWidget->setColumnWidth(1,80);

    ui->senatorLockBalanceTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);
    ui->senatorLockBalanceTableWidget->installEventFilter(this);
    ui->senatorLockBalanceTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->senatorLockBalanceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->senatorLockBalanceTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->senatorLockBalanceTableWidget->setFrameShape(QFrame::NoFrame);
    ui->senatorLockBalanceTableWidget->setMouseTracking(true);
    ui->senatorLockBalanceTableWidget->setShowGrid(false);//隐藏表格线

    ui->senatorLockBalanceTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->senatorLockBalanceTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->senatorLockBalanceTableWidget->horizontalHeader()->setVisible(true);
    ui->senatorLockBalanceTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->senatorLockBalanceTableWidget->setColumnWidth(0,190);
    ui->senatorLockBalanceTableWidget->setColumnWidth(1,190);

    ui->stackedWidget->setCurrentIndex(0);
    ui->typeStackedWidget->setCurrentIndex(0);

    ui->infoBtn->setCheckable(true);
    ui->voteStateBtn->setCheckable(true);
    ui->infoBtn->setChecked(true);
    ui->voteStateBtn->setChecked(false);
//    ui->infoBtn->setStyleSheet(PUSHBUTTON_CHECK_STYLE);
//    ui->voteStateBtn->setStyleSheet(PUSHBUTTON_CHECK_STYLE);
}

ProposalDetailDialog::~ProposalDetailDialog()
{
    delete ui;
}

void ProposalDetailDialog::pop()
{
    move(0,0);
    exec();
}

void ProposalDetailDialog::setProposal(QString _proposalId)
{
    ProposalInfo info;
    if(HXChain::getInstance()->senatorProposalInfoMap.contains(_proposalId))
    {
        info = HXChain::getInstance()->senatorProposalInfoMap.value(_proposalId);
    }
    else if(HXChain::getInstance()->citizenProposalInfoMap.contains(_proposalId))
    {
        info = HXChain::getInstance()->citizenProposalInfoMap.value(_proposalId);
    }
    else
    {
        return;
    }

    ui->proposalIdLabel->setText(info.proposalId);
    ui->proposerIdLabel->setText(info.proposer);
    ui->expirationTimeLabel->setText(toLocalTime(info.expirationTime));

    int size = info.requiredAccounts.size();
    ui->voteStateTableWidget->setRowCount(0);
    ui->voteStateTableWidget->setRowCount(size);
    for(int i = 0; i < size; i++)
    {
        ui->voteStateTableWidget->setRowHeight(i,40);

        QString address = info.requiredAccounts.at(i);
        ui->voteStateTableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1\n%2").
                                  arg(HXChain::getInstance()->guardAddressToName(address)).arg(address)));

        if(info.approvedKeys.contains(address))
        {
            ui->voteStateTableWidget->setItem(i, 1, new QTableWidgetItem(tr("approved")));
            ui->voteStateTableWidget->item(i,1)->setTextColor(QColor(0,170,0));
        }
        else if(info.disapprovedKeys.contains(address))
        {
            ui->voteStateTableWidget->setItem(i, 1, new QTableWidgetItem(tr("disapproved")));
            ui->voteStateTableWidget->item(i,1)->setTextColor(QColor(255,0,0));
        }
        else
        {
            ui->voteStateTableWidget->setItem(i, 1, new QTableWidgetItem(tr("not voted")));
        }

    }

    tableWidgetSetItemZebraColor(ui->voteStateTableWidget);

    QJsonObject object = QJsonDocument::fromJson(info.transactionStr.toLatin1()).object();
    switch (info.proposalOperationType)
    {
    case TRANSACTION_TYPE_COLDHOT:
    {
        ui->typeLabel->setText(tr("cold-hot trx"));
        ui->typeStackedWidget->setCurrentIndex(0);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        ui->assetLabel_hotCold->setText(operationObject.value("asset_symbol").toString());
        ui->amountLabel_hotCold->setText(operationObject.value("amount").toString());
        ui->sendLabel_hotCold->setText(operationObject.value("multi_account_withdraw").toString());
        ui->receiveLabel_hotCold->setText(operationObject.value("multi_account_deposit").toString());
        ui->memoLabel_hotCold->setText(operationObject.value("memo").toString());
    }
        break;
    case TRANSACTION_TYPE_CHANGE_ASSET_ACCOUNT:
    {
        ui->typeLabel->setText(tr("update multisig-address"));
        ui->typeStackedWidget->setCurrentIndex(1);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        ui->assetLabel_updateMS->setText(operationObject.value("chain_type").toString());
        ui->coldAddressLabel_updateMS->setText(operationObject.value("cold").toString());
        ui->hotAddressLabel_updateMS->setText(operationObject.value("hot").toString());
    }
        break;
    case TRANSACTION_TYPE_SET_PUBLISHER:
    {
        ui->typeLabel->setText(tr("set price feeder"));
        ui->typeStackedWidget->setCurrentIndex(2);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        ui->assetLabel_publisher->setText(operationObject.value("asset_symbol").toString());
        ui->publisherLabel_publisher->setText(operationObject.value("publisher").toString());
    }
        break;
    case TRANSACTION_TYPE_COLDHOT_CANCEL:
    {
        ui->typeLabel->setText(tr("cancel cold-hot trx"));
        ui->typeStackedWidget->setCurrentIndex(3);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        ui->coldHotTrxIdLabel->setText(operationObject.value("trx_id").toString());
    }
        break;
    case TRANSACTION_TYPE_WITHDRAW_CANCEL:
    {
        ui->typeLabel->setText(tr("cancel withdraw trx"));
        ui->typeStackedWidget->setCurrentIndex(4);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        ui->withdrawTrxIdLabel->setText(operationObject.value("not_enough_sign_trx_id").toString());
    }
        break;
    case TRANSACTION_TYPE_CREATE_GUARD:
    {
        ui->typeLabel->setText(tr("create senator"));
        ui->typeStackedWidget->setCurrentIndex(5);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        ui->newGuardLabel->setText(operationObject.value("guard_member_account").toString());
    }
        break;
    case TRANSACTION_TYPE_RESIGN_GUARD:
    {
        ui->typeLabel->setText(tr("resign senator"));
        ui->typeStackedWidget->setCurrentIndex(6);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        ui->resignGuardLabel->setText(operationObject.value("guard_member_account").toString());
    }
        break;
    case TRANSACTION_TYPE_PROPOSAL_CONTRACT_TRANSFER_FEE:
    {
        ui->typeLabel->setText(tr("set contract transfer fee"));
        ui->typeStackedWidget->setCurrentIndex(7);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        unsigned long long feeRate = jsonValueToULL( operationObject.value("fee_rate"));
        ui->contractTransferFeeLabel->setText( getBigNumberString(feeRate, ASSET_PRECISION));
    }
        break;
    case TRANSACTION_TYPE_FORMAL_GUARD:
    {
        ui->typeLabel->setText(tr("set senator formal/informal"));
        ui->typeStackedWidget->setCurrentIndex(8);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        ui->senatorAddressLabel->setText( operationObject.value("owner_addr").toString());
        ui->isFormalLabel->setText( operationObject.value("formal").toBool()?"true":"false");
    }
        break;
    case TRANSACTION_TYPE_CROSSCHAIN_FEE:
    {
        ui->typeLabel->setText(tr("set crosschain fee"));
        ui->typeStackedWidget->setCurrentIndex(9);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        QString assetSymbol = operationObject.value("asset_symbol").toString();
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(assetSymbol));
        ui->coinTypeLabel->setText( assetSymbol);
        ui->crosschainFeeLabel->setText( getBigNumberString( jsonValueToULL(operationObject.value("crosschain_fee")), assetInfo.precision));
    }
        break;
    case TRANSACTION_TYPE_SET_LOCKBALANCE:
    {
        ui->typeLabel->setText(tr("set locked balance of senator"));
        ui->typeStackedWidget->setCurrentIndex(10);

        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        QJsonArray lockBalanceArray = operationObject.value("lockbalance").toArray();
        int size = lockBalanceArray.size();
        ui->senatorLockBalanceTableWidget->setRowCount(size);
        for(int i = 0; i < size; i++)
        {
            QJsonObject assetObject = lockBalanceArray.at(i).toArray().at(1).toObject();
            QString assetId = assetObject.value("asset_id").toString();
            AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetId);
            ui->senatorLockBalanceTableWidget->setItem(i, 0, new QTableWidgetItem( assetInfo.symbol));
            ui->senatorLockBalanceTableWidget->setItem(i, 1, new QTableWidgetItem( getBigNumberString( jsonValueToULL(assetObject.value("amount")), assetInfo.precision)) );
        }
        tableWidgetSetItemZebraColor(ui->senatorLockBalanceTableWidget);
    }
        break;
    case TRANSACTION_TYPE_CITIZEN_CHANGE_SENATOR:
    {
        ui->typeLabel->setText(tr("citizen vote for changing senator"));
        ui->typeStackedWidget->setCurrentIndex(11);

        ui->label_weight->setText(calProposalWeight(info));
        QString changeStr = tr("%1      replaced by        %2");
        ui->label_fee->setText(info.pledge);
        QJsonObject operationObject = object.value("operations").toArray().at(0).toArray().at(1).toObject();
        QJsonArray arr = operationObject.value("replace_queue").toArray();

        QMap<QString,GuardInfo> allGuard(HXChain::getInstance()->allGuardMap);
        QMap<QString,QString> replaceMap;
        foreach (QJsonValue val, arr) {
            QString oldSenator ,newSenator;
            QMapIterator<QString, GuardInfo> it(allGuard);
            while (it.hasNext()) {
                it.next();
                if(val.toArray().at(1).toString() == it.value().accountId)
                {
                    oldSenator = it.key();
                }
                else if(val.toArray().at(0).toString() == it.value().accountId)
                {
                    newSenator = it.key();
                }
            }
            replaceMap[oldSenator] = newSenator;
        }

        int i = 0;
        ui->label_change1->setVisible(false);
        ui->label_change2->setVisible(false);
        ui->label_change3->setVisible(false);
        QMapIterator<QString, QString> it(replaceMap);
        while (it.hasNext()) {
            it.next();
            if(0 == i)
            {
                ui->label_change1->setText(changeStr.arg(it.key()).arg(it.value()));
                ui->label_change1->setVisible(true);
            }
            else if(1 == i)
            {
                ui->label_change2->setText(changeStr.arg(it.key()).arg(it.value()));
                ui->label_change2->setVisible(true);
            }
            else if(2 == i)
            {
                ui->label_change3->setText(changeStr.arg(it.key()).arg(it.value()));
                ui->label_change3->setVisible(true);
            }
            ++i;
        }

    }
        break;
    default:
        ui->typeLabel->setText(tr("unknown(%1)").arg(info.proposalOperationType));
        ui->typeStackedWidget->setCurrentIndex(12);
        break;
    }
}

void ProposalDetailDialog::on_infoBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);   
    ui->infoBtn->setChecked(true);
    ui->voteStateBtn->setChecked(false);
}

void ProposalDetailDialog::on_voteStateBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->infoBtn->setChecked(false);
    ui->voteStateBtn->setChecked(true);
}

void ProposalDetailDialog::on_closeBtn_clicked()
{
    close();
}

QString ProposalDetailDialog::calProposalWeight(const ProposalInfo &info) const
{
    unsigned long long allWeight = 0;
    unsigned long long alreadyWeight = 0;
    QMap<QString,MinerInfo> allMiner(HXChain::getInstance()->minerMap);
    foreach(QString requireAddress,info.requiredAccounts){//该requireaccounts
        QMapIterator<QString, MinerInfo> i(allMiner);
        while (i.hasNext()) {
            i.next();
            if(requireAddress == i.value().address)
            {
                allWeight += i.value().pledgeWeight;
            }
            if(info.approvedKeys.contains(i.value().address))
            {
                alreadyWeight += i.value().pledgeWeight;
            }
        }
    }
//    qDebug()<<"bbbbbbb"<<alreadyWeight<<allWeight;
//    unsigned long long gcd = std::min<unsigned long long>(allWeight,alreadyWeight);
//    if(0 == gcd)
//    {
        return QString::number(alreadyWeight)+"/\n"+QString::number(allWeight);
//    }
//    else
//    {
//        while(allWeight%gcd != 0 || alreadyWeight%gcd != 0)
//        {
//            --gcd;
//        }
//        qDebug()<<"aaaaa"<<gcd<<alreadyWeight/gcd<<allWeight/gcd;
//        return QString::number(alreadyWeight/gcd)+"/\n"+QString::number(allWeight/gcd);
//    }
}
