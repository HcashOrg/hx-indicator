#include "ProposalPage.h"
#include "ui_ProposalPage.h"

#include "wallet.h"
#include "ToolButtonWidget.h"
#include "commondialog.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "ProposalDetailDialog.h"

ProposalPage::ProposalPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProposalPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

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

    ui->proposalTableWidget->setColumnWidth(0,120);
    ui->proposalTableWidget->setColumnWidth(1,100);
    ui->proposalTableWidget->setColumnWidth(2,100);
    ui->proposalTableWidget->setColumnWidth(3,100);
    ui->proposalTableWidget->setColumnWidth(4,80);
    ui->proposalTableWidget->setColumnWidth(5,80);
    ui->proposalTableWidget->setColumnWidth(6,80);
    ui->proposalTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    init();
}

ProposalPage::~ProposalPage()
{
    delete ui;
}

void ProposalPage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = UBChain::getInstance()->getMyFormalGuards();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    UBChain::getInstance()->mainFrame->installBlurEffect(ui->proposalTableWidget);

    showProposals();
}

void ProposalPage::showProposals()
{
    QStringList keys = UBChain::getInstance()->proposalInfoMap.keys();

    int size = keys.size();
    ui->proposalTableWidget->setRowCount(0);
    ui->proposalTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->proposalTableWidget->setRowHeight(i,40);

        ProposalInfo info = UBChain::getInstance()->proposalInfoMap.value(keys.at(i));

        ui->proposalTableWidget->setItem(i,0,new QTableWidgetItem(info.expirationTime.replace("T","\n")));
        ui->proposalTableWidget->item(i,0)->setData(Qt::UserRole,info.proposalId);

        ui->proposalTableWidget->setItem(i,1,new QTableWidgetItem(UBChain::getInstance()->guardAccountIdToName(info.proposer)));

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
            typeStr = tr("set publisher");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_COLDHOT_CANCEL)
        {
            typeStr = tr("cancel cold-hot trx");
        }
        else if(info.proposalOperationType == TRANSACTION_TYPE_WITHDRAW_CANCEL)
        {
            typeStr = tr("cancel withdraw trx");
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


        QString address = UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
        if(address.isEmpty())
        {
            ui->proposalTableWidget->setItem(i,4,new QTableWidgetItem(tr("no guard")));
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


        if(!address.isEmpty())          // 如果有guard账户
        {
            ui->proposalTableWidget->setItem(i,5,new QTableWidgetItem(tr("approve")));
            ui->proposalTableWidget->setItem(i,6,new QTableWidgetItem(tr("disapprove")));


            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->proposalTableWidget->item(i,5)->text());
//            toolButton->setBackgroundColor(itemColor);
            ui->proposalTableWidget->setCellWidget(i,5,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ProposalPage::on_proposalTableWidget_cellClicked,this,i,5));

            ToolButtonWidget *toolButton2 = new ToolButtonWidget();
            toolButton2->setText(ui->proposalTableWidget->item(i,6)->text());
//            toolButton2->setBackgroundColor(itemColor);
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


        for (int j : {0,1,2,4})
        {
            if(i%2)
            {
                ui->proposalTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                ui->proposalTableWidget->item(i,j)->setBackgroundColor(QColor(252,253,255));
            }
            else
            {
                ui->proposalTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                ui->proposalTableWidget->item(i,j)->setBackgroundColor(QColor("white"));
            }
        }

    }
}

void ProposalPage::refresh()
{
    showProposals();
}

void ProposalPage::jsonDataUpdated(QString id)
{
    if( id == "id-approve_proposal")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
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
            QString address = UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
            QJsonArray array;
            array << address;
            QJsonObject object;
            object.insert("key_approvals_to_add", array);

            UBChain::getInstance()->postRPC( "id-approve_proposal", toJsonFormat( "approve_proposal",
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
            QString address = UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
            QJsonArray array;
            array << address;
            QJsonObject object;
            object.insert("key_approvals_to_remove", array);

            UBChain::getInstance()->postRPC( "id-approve_proposal", toJsonFormat( "approve_proposal",
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

void ProposalPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

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


