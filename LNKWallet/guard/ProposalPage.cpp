#include "ProposalPage.h"
#include "ui_ProposalPage.h"

#include "wallet.h"
#include "ToolButtonWidget.h"
#include "commondialog.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "ProposalDetailDialog.h"

static const int ROWNUMBER = 7;
ProposalPage::ProposalPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProposalPage)
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

    pageWidget = new PageScrollWidget();
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&ProposalPage::pageChangeSlot);

    blankWidget = new BlankDefaultWidget(ui->proposalTableWidget);
    blankWidget->setTextTip(tr("There are no proposals currently!"));
    init();
}

ProposalPage::~ProposalPage()
{
    delete ui;
}

void ProposalPage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->getMyFormalGuards();
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
        label->setText(tr("There are no senator accounts in the wallet."));
    }

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->proposalTableWidget);

    showProposals();
}

void ProposalPage::showProposals()
{
    QStringList keys = HXChain::getInstance()->proposalInfoMap.keys();

    int size = keys.size();
    ui->proposalTableWidget->setRowCount(0);
    ui->proposalTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->proposalTableWidget->setRowHeight(i,40);

        ProposalInfo info = HXChain::getInstance()->proposalInfoMap.value(keys.at(i));

        ui->proposalTableWidget->setItem(i,0,new QTableWidgetItem(info.expirationTime.replace("T","\n")));
        ui->proposalTableWidget->item(i,0)->setData(Qt::UserRole,info.proposalId);

        ui->proposalTableWidget->setItem(i,1,new QTableWidgetItem(HXChain::getInstance()->guardAccountIdToName(info.proposer)));

        QString typeStr;
        if(info.proposalOperationType == TRANSACTION_TYPE_COLDHOT)
        {
            typeStr = tr("cold-hot trx");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_CHANGE_ASSET_ACCOUNT)
        {
            typeStr = tr("update multisig");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_SET_PUBLISHER)
        {
            typeStr = tr("set price feeder");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_COLDHOT_CANCEL)
        {
            typeStr = tr("cancel cold-hot trx");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_WITHDRAW_CANCEL)
        {
            typeStr = tr("cancel withdraw trx");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_CREATE_GUARD)
        {
            typeStr = tr("create senator");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_RESIGN_GUARD)
        {
            typeStr = tr("resign senator");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_PROPOSAL_CONTRACT_TRANSFER_FEE)
        {
            typeStr = tr("set contract transfer fee");
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
            ui->proposalTableWidget->setItem(i,4,new QTableWidgetItem(tr("no senator")));
        }
        else if(info.approvedKeys.contains(address))
        {
            ui->proposalTableWidget->setItem(i,4,new QTableWidgetItem(tr("approved")));
            ui->proposalTableWidget->item(i,4)->setTextColor(QColor(0,255,0));
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
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ProposalPage::on_proposalTableWidget_cellClicked,this,i,5));

            ToolButtonWidget *toolButton2 = new ToolButtonWidget();
            toolButton2->setText(ui->proposalTableWidget->item(i,6)->text());
            ui->proposalTableWidget->setCellWidget(i,6,toolButton2);
            connect(toolButton2,&ToolButtonWidget::clicked,std::bind(&ProposalPage::on_proposalTableWidget_cellClicked,this,i,6));

            if(info.approvedKeys.contains(address))
            {
                toolButton->setEnabled(false);
            }

            if(info.disapprovedKeys.contains(address))
            {
                toolButton2->setEnabled(false);
            }
        }


        tableWidgetSetItemZebraColor(ui->proposalTableWidget);
    }

    int page = (ui->proposalTableWidget->rowCount()%ROWNUMBER==0 && ui->proposalTableWidget->rowCount() != 0) ?
                ui->proposalTableWidget->rowCount()/ROWNUMBER : ui->proposalTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(ui->proposalTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(0);

    pageWidget->setVisible(0 != ui->proposalTableWidget->rowCount());
    blankWidget->setVisible(ui->proposalTableWidget->rowCount() == 0);
}

void ProposalPage::refresh()
{
    showProposals();
}

void ProposalPage::jsonDataUpdated(QString id)
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

void ProposalPage::on_proposalTableWidget_cellClicked(int row, int column)
{
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

void ProposalPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    showProposals();
}

void ProposalPage::on_proposalTableWidget_cellPressed(int row, int column)
{
    if(column == 2)
    {
        ProposalDetailDialog proposalDetailDialog;
        proposalDetailDialog.setProposal(ui->proposalTableWidget->item(row,0)->data(Qt::UserRole).toString());
        proposalDetailDialog.pop();
        return;
    }
}

void ProposalPage::pageChangeSlot(unsigned int page)
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

void ProposalPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

VoteStateLabel::VoteStateLabel(QWidget *parent)
{
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

VoteStateLabel::~VoteStateLabel()
{

}

void VoteStateLabel::setVoteNum(int approve, int disapprove, int total)
{
    QString str;
    if( 3 * disapprove >= total )
    {
        str = QString("<body><font style=\"font-size:12px\" color=#ff0000>%1("
                      "<font style=\"font-size:12px\" color=#00ff00>%2</font>"
                      "/"
                      "<font style=\"font-size:12px\" color=#ff0000>%3</font>"
                      "/"
                      "%4"
                      ")</font></body>")
                .arg(tr("not passed")).arg(approve).arg(disapprove).arg(total);
    }
    else
    {
        str = QString("<body><font style=\"font-size:12px\" color=#ff8400>%1</font>("
                      "<font style=\"font-size:12px\" color=#00ff00>%2</font>"
                      "/"
                      "<font style=\"font-size:12px\" color=#ff0000>%3</font>"
                      "/"
                      "%4"
                      ")</body>")
                .arg(tr("voting")).arg(approve).arg(disapprove).arg(total);
    }

    setText(str);
}


