#include "CitizenProposalPage.h"
#include "ui_CitizenProposalPage.h"

#include "wallet.h"
#include "ToolButtonWidget.h"
#include "commondialog.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "guard/ProposalDetailDialog.h"
#include "guard/ProposalPage.h"
#include "citizen/ChangeSenatorDialog.h"
#include "citizen/AddPledgeDialog.h"
#include "poundage/PageScrollWidget.h"

static const int ROWNUMBER = 7;

Q_DECLARE_METATYPE(ProposalInfo)

CitizenProposalPage::CitizenProposalPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CitizenProposalPage),
    isCurrentTimeBigThanNextVoteTime(false)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));
    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    ui->proposalTableWidget->installEventFilter(this);
    ui->proposalTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->proposalTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->proposalTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->proposalTableWidget->setFrameShape(QFrame::NoFrame);
    ui->proposalTableWidget->setMouseTracking(true);
    ui->proposalTableWidget->setShowGrid(false);//隐藏表格线

    ui->proposalTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->proposalTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->proposalTableWidget->horizontalHeader()->setVisible(true);
    ui->proposalTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->proposalTableWidget->setColumnWidth(0,100);
    ui->proposalTableWidget->setColumnWidth(1,100);
    ui->proposalTableWidget->setColumnWidth(2,100);
    ui->proposalTableWidget->setColumnWidth(3,100);
    ui->proposalTableWidget->setColumnWidth(4,90);
    ui->proposalTableWidget->setColumnWidth(5,90);
    ui->proposalTableWidget->setColumnWidth(6,60);
//    ui->proposalTableWidget->setColumnWidth(7,70);
//    ui->proposalTableWidget->setColumnWidth(8,70);
    ui->proposalTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->changeSenatorBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->proposalTableWidget);


    pageWidget = new PageScrollWidget();
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&CitizenProposalPage::pageChangeSlot);

//    pageWidget = new PageScrollWidget();
//    ui->stackedWidget->addWidget(pageWidget);
//    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&ProposalPage::pageChangeSlot);

//    blankWidget = new BlankDefaultWidget(ui->proposalTableWidget);
//    blankWidget->setTextTip(tr("There are no proposals currently!"));
    init();
}

CitizenProposalPage::~CitizenProposalPage()
{
    delete ui;
}

void CitizenProposalPage::init()
{
    inited = false;

    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->getMyCitizens();
    if(accounts.size() > 0)
    {
        ui->accountComboBox->addItems(accounts);

        if(accounts.contains(HXChain::getInstance()->currentAccount))
        {
            ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
        }
    }
    else
    {
        ui->label->hide();
        ui->accountComboBox->hide();

        QLabel* label = new QLabel(this);
        label->setGeometry(QRect(ui->label->pos(), QSize(300,30)));
        label->setText(tr("There are no citizen accounts in the wallet."));
        label->setStyleSheet("QLabel{color: rgb(137,129,161);font: 11px \"Microsoft YaHei UI Light\";}");
    }

    inited = true;

    on_accountComboBox_currentIndexChanged(ui->accountComboBox->currentText());
}

void CitizenProposalPage::refresh()
{
    showProposals();
}

void CitizenProposalPage::jsonDataUpdated(QString id)
{
    if( id == "id-approve_proposal")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result.startsWith("\"result\":{"))             // 成功
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of voting for the proposal has been sent,please wait for confirmation"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Failed!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }
        return;
    }
    else if("query_object_state"==id)
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;


        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonObject object = jsonObject.value("result").toArray().at(0).toObject();
        QString nextTime = object.value("next_vote_time").toString();
        isCurrentTimeBigThanNextVoteTime = QDateTime::fromString(HXChain::getInstance()->currentBlockTime,"yyyy-MM-ddThh:mm:ss") >
                                           QDateTime::fromString(nextTime,"yyyy-MM-ddThh:mm:ss");

        qDebug()<<"aaaa"<<object;
        qDebug()<<"votetimessss"<<QDateTime::fromString(HXChain::getInstance()->currentBlockTime,"yyyy-MM-ddThh:mm:ss")<<QDateTime::fromString(nextTime,"yyyy-MM-ddThh:mm:ss")<<isCurrentTimeBigThanNextVoteTime;

    }

}

void CitizenProposalPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)  return;

    if(!ui->accountComboBox->currentText().isEmpty())
    {
        HXChain::getInstance()->currentAccount = ui->accountComboBox->currentText();
    }

    showProposals();
}

void CitizenProposalPage::showProposals()
{
    //查询当前vote状态
    HXChain::getInstance()->postRPC("query_object_state",toJsonFormat("get_object",QJsonArray()<<"2.1.0"));
    //查询白名单
    queryWhiteList();
//    QStringList keys = HXChain::getInstance()->citizenProposalInfoMap.keys();

//    int size = keys.size();
//    ui->proposalTableWidget->setRowCount(0);
//    ui->proposalTableWidget->setRowCount(size);

//    for(int i = 0; i < size; i++)
//    {
//        ui->proposalTableWidget->setRowHeight(i,40);

//        ProposalInfo info = HXChain::getInstance()->citizenProposalInfoMap.value(keys.at(i));

//        ui->proposalTableWidget->setItem(i,0,new QTableWidgetItem(toLocalTime(info.expirationTime)));
//        ui->proposalTableWidget->item(i,0)->setData(Qt::UserRole,info.proposalId);

//        QMap<QString,MinerInfo> allCitizen(HXChain::getInstance()->minerMap);
//        QMapIterator<QString, MinerInfo> it(allCitizen);
//        QString citizenName = "";
//        while (it.hasNext()) {
//            it.next();
//            if(info.proposer == it.value().accountId)
//            {
//                citizenName = it.key();
//                break;
//            }
//        }
//        ui->proposalTableWidget->setItem(i,1,new QTableWidgetItem(citizenName));

//        QString typeStr;
//        if(TRANSACTION_TYPE_CITIZEN_CHANGE_SENATOR == info.proposalOperationType)
//        {
//            typeStr = tr("change senator");
//        }
//        else
//        {
//            typeStr = tr("unknown");
//        }
//        ui->proposalTableWidget->setItem(i,2,new QTableWidgetItem(typeStr));
//        ui->proposalTableWidget->item(i,2)->setTextColor(QColor(84,116,235));

//        //投票状态
//        ui->proposalTableWidget->setItem(i,3,new QTableWidgetItem(calProposalWeight(info)));
////        VoteStateLabel* voteStateLabel = new VoteStateLabel;
////        voteStateLabel->setVoteNum(info.approvedKeys.size(), info.disapprovedKeys.size(), info.requiredAccounts.size());
////        ui->proposalTableWidget->setCellWidget(i,3,voteStateLabel);

//        //质押数量
//        ui->proposalTableWidget->setItem(i,4,new QTableWidgetItem(info.pledge));
//        //我的投票状态
//        QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
//        if(address.isEmpty())
//        {
//            ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(tr("no citizen")));
//        }
//        else if(info.approvedKeys.contains(address))
//        {
//            ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(tr("approved")));
//            ui->proposalTableWidget->item(i,5)->setTextColor(QColor(0,170,0));
//        }
//        else if(info.disapprovedKeys.contains(address))
//        {
//            ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(tr("not voted")));
//            ui->proposalTableWidget->item(i,5)->setTextColor(QColor(255,0,0));
//        }
//        else
//        {
//            ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(tr("not voted")));
//        }

////        ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(calProposalWeight(info)));
//        if(!address.isEmpty())          // 如果有senator账户
//        {
//            ui->proposalTableWidget->setItem(i,6,new QTableWidgetItem(tr("approve")));
//            ui->proposalTableWidget->setItem(i,7,new QTableWidgetItem(tr("disapprove")));
//            ui->proposalTableWidget->setItem(i,8,new QTableWidgetItem(tr("addPledge")));


//            ToolButtonWidget *toolButton = new ToolButtonWidget();
//            toolButton->setText(ui->proposalTableWidget->item(i,6)->text());
//            ui->proposalTableWidget->setCellWidget(i,6,toolButton);
//            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,6));

//            ToolButtonWidget *toolButton2 = new ToolButtonWidget();
//            toolButton2->setText(ui->proposalTableWidget->item(i,7)->text());
//            ui->proposalTableWidget->setCellWidget(i,7,toolButton2);
//            connect(toolButton2,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,7));

//            ToolButtonWidget *tooButton3 = new ToolButtonWidget();
//            tooButton3->setText(ui->proposalTableWidget->item(i,8)->text());
//            ui->proposalTableWidget->setCellWidget(i,8,tooButton3);
//            ui->proposalTableWidget->item(i,8)->setData(Qt::UserRole,QVariant::fromValue<ProposalInfo>(info));
//            connect(tooButton3,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,8));

//            if(info.approvedKeys.contains(address))
//            {
//                toolButton->setEnabled(false);
//                ui->proposalTableWidget->hideColumn(6);
//            }
//            else if(info.disapprovedKeys.contains(address))
//            {
//                ui->proposalTableWidget->hideColumn(7);
//                toolButton2->setEnabled(false);
//            }
//            else
//            {
//                ui->proposalTableWidget->hideColumn(7);
//                toolButton2->setEnabled(false);
//            }

//        }
//    }

//    int page = (ui->proposalTableWidget->rowCount()%ROWNUMBER==0 && ui->proposalTableWidget->rowCount() != 0) ?
//                ui->proposalTableWidget->rowCount()/ROWNUMBER : ui->proposalTableWidget->rowCount()/ROWNUMBER+1;
//    pageWidget->SetTotalPage(page);
//    pageWidget->setShowTip(ui->proposalTableWidget->rowCount(),ROWNUMBER);
//    pageChangeSlot(0);

//    pageWidget->setVisible(0 != ui->proposalTableWidget->rowCount());
//    tableWidgetSetItemZebraColor(ui->proposalTableWidget);


}

void CitizenProposalPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void CitizenProposalPage::on_proposalTableWidget_cellClicked(int row, int column)
{
    if(column == 2)
    {
        ProposalDetailDialog proposalDetailDialog;
        proposalDetailDialog.setProposal(ui->proposalTableWidget->item(row,0)->data(Qt::UserRole).toString());
        proposalDetailDialog.pop();
        return;
    }

    QWidget *wi = ui->proposalTableWidget->cellWidget(row,6);
    if(!wi || !dynamic_cast<ToolButtonWidget*>(wi))
    {
        return;
    }
    if(column == 6)
    {

        CommonDialog commonDialog(CommonDialog::YesOrNo);
        commonDialog.setText(tr("Sure to approve this proposal?"));
        if(commonDialog.pop())
        {
            QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
            QJsonArray array;
            array << address;
            QJsonObject object;
            object.insert("key_approvals_to_add", array);

            HXChain::getInstance()->postRPC( "id-approve_proposal", toJsonFormat( "approve_referendum",
                                    QJsonArray() << ui->accountComboBox->currentText()
                                    << ui->proposalTableWidget->item(row,0)->data(Qt::UserRole).toString()
                                    << object << true));
        }
        return;
    }

    if(column == 7)
    {
        CommonDialog commonDialog(CommonDialog::YesOrNo);
        commonDialog.setText(tr("Sure to disapprove this proposal?"));
        if(commonDialog.pop())
        {
            QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
            QJsonArray array;
            array << address;
            QJsonObject object;
            object.insert("key_approvals_to_remove", array);

            HXChain::getInstance()->postRPC( "id-approve_proposal", toJsonFormat( "approve_referendum",
                                    QJsonArray() << ui->accountComboBox->currentText()
                                    << ui->proposalTableWidget->item(row,0)->data(Qt::UserRole).toString()
                                    << object << true));
        }
        return;
    }

    if(8 == column)
    {
        AddPledgeDialog dia(ui->accountComboBox->currentText(),ui->proposalTableWidget->item(row,6)->data(Qt::UserRole).value<ProposalInfo>());
        dia.exec();
    }
}

void CitizenProposalPage::on_changeSenatorBtn_clicked()
{
    if(HXChain::getInstance()->walletInfo.blockHeight < 1400000 )
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Official Senator selection will begin on 8th February,2019. Please do no initiate any selection proposal before 8th in order to keep a friendly selection process. Your understanding is much appreciated."));
        commonDialog.pop();
        return;
    }

    ChangeSenatorDialog dia;
    dia.exec();
}

void CitizenProposalPage::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->proposalTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->proposalTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->proposalTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->proposalTableWidget->setRowHidden(i,true);
        }
    }
}

void CitizenProposalPage::httpReplied(QByteArray _data, int _status)
{
    QJsonArray arr  = QJsonDocument::fromJson(_data).object().value("result").toArray();
    QStringList whiteList;//accountid
    foreach (QJsonValue val, arr) {
        if(!val.isString()) continue;
        whiteList.append(val.toString());
    }


    QStringList keys = HXChain::getInstance()->citizenProposalInfoMap.keys();

    int size = keys.size();
    ui->proposalTableWidget->setRowCount(0);
    for(int i = 0; i < size; i++)
    {
        ProposalInfo info = HXChain::getInstance()->citizenProposalInfoMap.value(keys.at(i));
        QJsonObject object = QJsonDocument::fromJson(info.transactionStr.toLatin1()).object();
        QJsonArray arr = object.value("operations").toArray().at(0).toArray().at(1).toObject().value("replace_queue").toArray();
        //获取提案的新成员
        bool isAllNewInWhiteList = true;
        foreach (QJsonValue val, arr) {
            if(!whiteList.contains(val.toArray().at(0).toString()))
            {
                isAllNewInWhiteList = false;
                break;
            }
        }
        if(!isAllNewInWhiteList)
        {
            continue;
        }

        ui->proposalTableWidget->insertRow(ui->proposalTableWidget->rowCount());
        ui->proposalTableWidget->setRowHeight(i,40);

        ui->proposalTableWidget->setItem(i,0,new QTableWidgetItem(toLocalTime(info.expirationTime)));
        ui->proposalTableWidget->item(i,0)->setData(Qt::UserRole,info.proposalId);

        QMap<QString,MinerInfo> allCitizen(HXChain::getInstance()->minerMap);
        QMapIterator<QString, MinerInfo> it(allCitizen);
        QString citizenName = "";
        while (it.hasNext()) {
            it.next();
            if(info.proposer == it.value().accountId)
            {
                citizenName = it.key();
                break;
            }
        }
        ui->proposalTableWidget->setItem(i,1,new QTableWidgetItem(citizenName));

        QString typeStr;
        if(TRANSACTION_TYPE_CITIZEN_CHANGE_SENATOR == info.proposalOperationType)
        {
            typeStr = tr("change senator");
        }
        else
        {
            typeStr = tr("unknown");
        }
        ui->proposalTableWidget->setItem(i,2,new QTableWidgetItem(typeStr));
        ui->proposalTableWidget->item(i,2)->setTextColor(QColor(84,116,235));

        //投票状态
        ui->proposalTableWidget->setItem(i,3,new QTableWidgetItem(calProposalWeight(info)));
//        VoteStateLabel* voteStateLabel = new VoteStateLabel;
//        voteStateLabel->setVoteNum(info.approvedKeys.size(), info.disapprovedKeys.size(), info.requiredAccounts.size());
//        ui->proposalTableWidget->setCellWidget(i,3,voteStateLabel);

        //质押数量
        ui->proposalTableWidget->setItem(i,4,new QTableWidgetItem(info.pledge));
        //我的投票状态
        QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
        if(address.isEmpty())
        {
            ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(tr("no citizen")));
        }
        else if(info.approvedKeys.contains(address))
        {
            ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(tr("approved")));
            ui->proposalTableWidget->item(i,5)->setTextColor(QColor(0,170,0));
        }
        else if(info.disapprovedKeys.contains(address))
        {
            ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(tr("not voted")));
            ui->proposalTableWidget->item(i,5)->setTextColor(QColor(255,0,0));
        }
        else
        {
            ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(tr("not voted")));
        }

//        ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(calProposalWeight(info)));
        qDebug()<<"proproadddd"<<address;
        if(!address.isEmpty())          // 如果有senator账户
        {
            if(isCurrentTimeBigThanNextVoteTime)
            {
                if(!info.approvedKeys.contains(address))
                {
                    qDebug()<<"1";
                    ui->proposalTableWidget->setItem(i,6,new QTableWidgetItem(tr("approve")));
                    ToolButtonWidget *toolButton = new ToolButtonWidget();
                    toolButton->setText(ui->proposalTableWidget->item(i,6)->text());
                    ui->proposalTableWidget->setCellWidget(i,6,toolButton);
                    ui->proposalTableWidget->item(i,6)->setData(Qt::UserRole,QVariant::fromValue<ProposalInfo>(info));
                    connect(toolButton,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,6));
                }
                else
                {
                    qDebug()<<"2";
                    ui->proposalTableWidget->setItem(i,6,new QTableWidgetItem(tr("disapprove")));
        //            ui->proposalTableWidget->setItem(i,7,new QTableWidgetItem(tr("disapprove")));
        //            ui->proposalTableWidget->setItem(i,8,new QTableWidgetItem(tr("addPledge")));


                    ToolButtonWidget *toolButton = new ToolButtonWidget();
                    toolButton->setText(ui->proposalTableWidget->item(i,6)->text());
                    ui->proposalTableWidget->setCellWidget(i,6,toolButton);
                    ui->proposalTableWidget->item(i,6)->setData(Qt::UserRole,QVariant::fromValue<ProposalInfo>(info));
                    connect(toolButton,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,7));
                }

            }
            else
            {
                //提案人address
                QString proposeAccountID = info.proposer;
                if(HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).id == proposeAccountID)
                {
                    qDebug()<<"3";
                    ui->proposalTableWidget->setItem(i,6,new QTableWidgetItem(tr("addPledge")));
                    ToolButtonWidget *toolButton = new ToolButtonWidget();
                    toolButton->setText(ui->proposalTableWidget->item(i,6)->text());
                    ui->proposalTableWidget->setCellWidget(i,6,toolButton);
                    ui->proposalTableWidget->item(i,6)->setData(Qt::UserRole,QVariant::fromValue<ProposalInfo>(info));
                    connect(toolButton,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,8));
                }
                else
                {
                    qDebug()<<"5";
                    ui->proposalTableWidget->setItem(i,6,new QTableWidgetItem(("")));
                }
            }
        }
        else
        {
            ui->proposalTableWidget->hideColumn(6);
        }
    }

    int page = (ui->proposalTableWidget->rowCount()%ROWNUMBER==0 && ui->proposalTableWidget->rowCount() != 0) ?
                ui->proposalTableWidget->rowCount()/ROWNUMBER : ui->proposalTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(ui->proposalTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(0);

    pageWidget->setVisible(0 != ui->proposalTableWidget->rowCount());
    tableWidgetSetItemZebraColor(ui->proposalTableWidget);

}


QString CitizenProposalPage::calProposalWeight(const ProposalInfo &info) const
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
    if(0 != allWeight)
    {
        return QString::number(alreadyWeight*1.0/allWeight,'f',2)+"%";
    }
    else
    {
        return "0";
    }
}

void CitizenProposalPage::queryWhiteList()
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","Zchain.Plugin.QueryWhiteListSenatorId");
    httpManager.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());
}
