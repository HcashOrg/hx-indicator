#include "CitizenProposalPage.h"
#include "ui_CitizenProposalPage.h"

#include "wallet.h"
#include "ToolButtonWidget.h"
#include "commondialog.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "guard/ProposalDetailDialog.h"
#include "guard/ProposalPage.h"

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

    ui->proposalTableWidget->setColumnWidth(0,110);
    ui->proposalTableWidget->setColumnWidth(1,90);
    ui->proposalTableWidget->setColumnWidth(2,100);
    ui->proposalTableWidget->setColumnWidth(3,100);
    ui->proposalTableWidget->setColumnWidth(4,80);
    ui->proposalTableWidget->setColumnWidth(5,80);
    ui->proposalTableWidget->setColumnWidth(6,80);
    ui->proposalTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

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
        label->setGeometry(QRect(ui->accountComboBox->pos(), QSize(300,30)));
        label->setText(tr("There are no citizen accounts in the wallet."));
    }

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->proposalTableWidget);

    showProposals();
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

        ui->proposalTableWidget->setItem(i,0,new QTableWidgetItem(info.expirationTime.replace("T","\n")));
        ui->proposalTableWidget->item(i,0)->setData(Qt::UserRole,info.proposalId);

        ui->proposalTableWidget->setItem(i,1,new QTableWidgetItem(HXChain::getInstance()->guardAccountIdToName(info.proposer)));

        QString typeStr;
        if(info.proposalOperationType ==  TRANSACTION_TYPE_FORMAL_GUARD)
        {
            typeStr = tr("set senator formal/informal");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_CROSSCHAIN_FEE)
        {
            typeStr = tr("set crosschain fee");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_SET_LOCKBALANCE)
        {
            typeStr = tr("set locked balance of senator");
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


        if(!address.isEmpty())          // 如果有senator账户
        {
            ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(tr("approve")));
            ui->proposalTableWidget->setItem(i,6,new QTableWidgetItem(tr("disapprove")));


            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->proposalTableWidget->item(i,5)->text());
            ui->proposalTableWidget->setCellWidget(i,5,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,5));

            ToolButtonWidget *toolButton2 = new ToolButtonWidget();
            toolButton2->setText(ui->proposalTableWidget->item(i,6)->text());
            ui->proposalTableWidget->setCellWidget(i,6,toolButton2);
            connect(toolButton2,&ToolButtonWidget::clicked,std::bind(&CitizenProposalPage::on_proposalTableWidget_cellClicked,this,i,6));

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

    if(column == 5)
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

            HXChain::getInstance()->postRPC( "id-approve_proposal", toJsonFormat( "approve_proposal",
                                    QJsonArray() << ui->accountComboBox->currentText()
                                    << ui->proposalTableWidget->item(row,0)->data(Qt::UserRole).toString()
                                    << object << true));
        }
        return;
    }

    if(column == 6)
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

            HXChain::getInstance()->postRPC( "id-approve_proposal", toJsonFormat( "approve_proposal",
                                    QJsonArray() << ui->accountComboBox->currentText()
                                    << ui->proposalTableWidget->item(row,0)->data(Qt::UserRole).toString()
                                    << object << true));
        }
        return;
    }
}
