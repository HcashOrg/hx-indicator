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
    ui(new Ui::CitizenProposalPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

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

    ui->proposalTableWidget->setColumnWidth(0,80);
    ui->proposalTableWidget->setColumnWidth(1,80);
    ui->proposalTableWidget->setColumnWidth(2,80);
    ui->proposalTableWidget->setColumnWidth(3,85);
    ui->proposalTableWidget->setColumnWidth(4,80);
    ui->proposalTableWidget->setColumnWidth(5,65);
    ui->proposalTableWidget->setColumnWidth(6,55);
    ui->proposalTableWidget->setColumnWidth(7,55);
    ui->proposalTableWidget->setColumnWidth(8,60);
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
    QStringList keys = HXChain::getInstance()->citizenProposalInfoMap.keys();

    int size = keys.size();
    ui->proposalTableWidget->setRowCount(0);
    ui->proposalTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->proposalTableWidget->setRowHeight(i,40);

        ProposalInfo info = HXChain::getInstance()->citizenProposalInfoMap.value(keys.at(i));

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

        VoteStateLabel* voteStateLabel = new VoteStateLabel;
        voteStateLabel->setVoteNum(info.approvedKeys.size(), info.disapprovedKeys.size(), info.requiredAccounts.size());
        ui->proposalTableWidget->setCellWidget(i,3,voteStateLabel);

        QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
        if(address.isEmpty())
        {
            ui->proposalTableWidget->setItem(i,4,new QTableWidgetItem(tr("no citizen")));
        }
        else if(info.approvedKeys.contains(address))
        {
            ui->proposalTableWidget->setItem(i,4,new QTableWidgetItem(tr("approved")));
            ui->proposalTableWidget->item(i,4)->setTextColor(QColor(0,170,0));
        }
        else if(info.disapprovedKeys.contains(address))
        {
            ui->proposalTableWidget->setItem(i,4,new QTableWidgetItem(tr("disapproved")));
            ui->proposalTableWidget->item(i,4)->setTextColor(QColor(255,0,0));
        }
        else
        {
            ui->proposalTableWidget->setItem(i,4,new QTableWidgetItem(tr("not voted")));
        }

        ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(calProposalWeight(info)));
        if(!address.isEmpty())          // 如果有senator账户
        {
            ui->proposalTableWidget->setItem(i,6,new QTableWidgetItem(tr("approve")));
            ui->proposalTableWidget->setItem(i,7,new QTableWidgetItem(tr("disapprove")));
            ui->proposalTableWidget->setItem(i,8,new QTableWidgetItem(tr("addPledge")));


            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->proposalTableWidget->item(i,6)->text());
            ui->proposalTableWidget->setCellWidget(i,6,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,6));

            ToolButtonWidget *toolButton2 = new ToolButtonWidget();
            toolButton2->setText(ui->proposalTableWidget->item(i,7)->text());
            ui->proposalTableWidget->setCellWidget(i,7,toolButton2);
            connect(toolButton2,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,7));

            ToolButtonWidget *tooButton3 = new ToolButtonWidget();
            tooButton3->setText(ui->proposalTableWidget->item(i,8)->text());
            ui->proposalTableWidget->setCellWidget(i,8,tooButton3);
            ui->proposalTableWidget->item(i,8)->setData(Qt::UserRole,QVariant::fromValue<ProposalInfo>(info));
            connect(tooButton3,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,8));

            if(info.approvedKeys.contains(address))
            {
                toolButton->setEnabled(false);
            }

            if(info.disapprovedKeys.contains(address))
            {
                toolButton2->setEnabled(false);
            }
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
        AddPledgeDialog dia(ui->accountComboBox->currentText(),ui->proposalTableWidget->item(row,column)->data(Qt::UserRole).value<ProposalInfo>());
        dia.exec();
    }
}

void CitizenProposalPage::on_changeSenatorBtn_clicked()
{
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

QString CitizenProposalPage::calProposalWeight(const ProposalInfo &info) const
{
    unsigned long long allWeight = 0;
    unsigned long long alreadyWeight = 0;
    QMap<QString,MinerInfo> allMiner(HXChain::getInstance()->minerMap);
    foreach(QString requireKey,info.requiredAccounts){
        QMapIterator<QString, MinerInfo> i(allMiner);
        while (i.hasNext()) {
            i.next();
            if(requireKey == i.value().signingKey)
            {
                allWeight += i.value().pledgeWeight;
            }
            if(info.approvedKeys.contains(i.value().signingKey))
            {
                alreadyWeight += i.value().pledgeWeight;
            }
        }
    }
    return QString::number(alreadyWeight)+"/"+QString::number(allWeight);
}
