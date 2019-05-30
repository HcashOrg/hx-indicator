#include "WithdrawConfirmPage.h"
#include "ui_WithdrawConfirmPage.h"

#include "wallet.h"
#include "ToolButtonWidget.h"
#include "commondialog.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "showcontentdialog.h"
#include "WithdrawInfoDialog.h"
#include "ColdHotTransferPage.h"
#include "control/AssetIconItem.h"
#include "AutoWithdrawSetDialog.h"

static const int ROWNUMBER = 6;
WithdrawConfirmPage::WithdrawConfirmPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WithdrawConfirmPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));
    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    ui->crosschainTransactionTableWidget->installEventFilter(this);
    ui->crosschainTransactionTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->crosschainTransactionTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->crosschainTransactionTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->crosschainTransactionTableWidget->setMouseTracking(true);
    ui->crosschainTransactionTableWidget->setShowGrid(false);//隐藏表格线

    ui->crosschainTransactionTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->crosschainTransactionTableWidget->horizontalHeader()->setVisible(true);
    ui->crosschainTransactionTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->crosschainTransactionTableWidget->setColumnWidth(0,110);
    ui->crosschainTransactionTableWidget->setColumnWidth(1,90);
    ui->crosschainTransactionTableWidget->setColumnWidth(2,100);
    ui->crosschainTransactionTableWidget->setColumnWidth(3,100);
    ui->crosschainTransactionTableWidget->setColumnWidth(4,80);
    ui->crosschainTransactionTableWidget->setColumnWidth(5,80);
    ui->crosschainTransactionTableWidget->setColumnWidth(6,80);
    ui->crosschainTransactionTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->ethFinalTrxTableWidget->installEventFilter(this);
    ui->ethFinalTrxTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->ethFinalTrxTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ethFinalTrxTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->ethFinalTrxTableWidget->setMouseTracking(true);
    ui->ethFinalTrxTableWidget->setShowGrid(false);//隐藏表格线

    ui->ethFinalTrxTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->ethFinalTrxTableWidget->horizontalHeader()->setVisible(true);
    ui->ethFinalTrxTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->ethFinalTrxTableWidget->setColumnWidth(0,120);
    ui->ethFinalTrxTableWidget->setColumnWidth(1,360);
    ui->ethFinalTrxTableWidget->setColumnWidth(2,80);
    ui->ethFinalTrxTableWidget->setColumnWidth(3,100);
    ui->ethFinalTrxTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->typeCurrentBtn->setCheckable(true);
    ui->typeWaitingBtn->setCheckable(true);
    ui->typeETHBtn->setCheckable(true);

    setStyleSheet(PUSHBUTTON_CHECK_STYLE);

    ui->typeCurrentBtn->adjustSize();
    ui->typeCurrentBtn->resize(ui->typeCurrentBtn->width(), 18);
    ui->typeWaitingBtn->adjustSize();
    ui->typeWaitingBtn->resize(ui->typeWaitingBtn->width(), 18);
    ui->typeWaitingBtn->move(ui->typeCurrentBtn->x() + ui->typeCurrentBtn->width() + 30, ui->typeWaitingBtn->y());
    ui->typeETHBtn->adjustSize();
    ui->typeETHBtn->resize(ui->typeETHBtn->width(), 18);
    ui->typeETHBtn->move(ui->typeWaitingBtn->x() + ui->typeWaitingBtn->width() + 30, ui->typeETHBtn->y());

    pageWidget = new PageScrollWidget();
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&WithdrawConfirmPage::pageChangeSlot);

    ui->stackedWidget2->setCurrentIndex(0);

    blankWidget = new BlankDefaultWidget(ui->crosschainTransactionTableWidget);
    blankWidget->setTextTip(tr("There are no withdraw trxs to authorize currently!"));
    init();
}

WithdrawConfirmPage::~WithdrawConfirmPage()
{
    delete ui;
}

void WithdrawConfirmPage::init()
{

    QStringList assets = HXChain::getInstance()->getETHAssets();
    ui->assetComboBox->addItems(assets);
    connect(ui->assetComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(onAssetComboBoxCurrentIndexChanged(QString)));
    ui->assetComboBox->hide();
    ui->assetLabel->hide();

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
        label->setGeometry(QRect(ui->label->pos(), QSize(280,18)));
        label->setText(tr("There are no senator accounts in the wallet."));
    }


    HXChain::getInstance()->mainFrame->installBlurEffect(ui->crosschainTransactionTableWidget);
    HXChain::getInstance()->mainFrame->installBlurEffect(ui->ethFinalTrxTableWidget);

    ui->typeCurrentBtn->setChecked(true);

    ui->autoSignCheckBox->setChecked(HXChain::getInstance()->autoWithdrawConfirm);
    ui->autoSignSetBtn->setVisible(ui->autoSignCheckBox->isChecked());


    HXChain::getInstance()->fetchCrosschainTransactions();

    inited = true;
}

void WithdrawConfirmPage::refresh()
{
    fetchEthBalance();
}


void WithdrawConfirmPage::jsonDataUpdated(QString id)
{
    if(id == "WithdrawConfirmPage-senator_sign_crosschain_transaction")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {            
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of senator-sign-crosschain has been sent out!"));
            commonDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Failed!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        return;
    }

    if(id.startsWith("WithdrawConfirmPage-get_multi_address_obj-" + ui->assetComboBox->currentText() + "-"))
    {
        QString accountId = id.mid(QString("WithdrawConfirmPage-get_multi_address_obj-" + ui->assetComboBox->currentText() + "-").size());
        AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
        if(accountId != accountInfo.id)     return;

        QString result = HXChain::getInstance()->jsonDataValue(id);
        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonObject ethMultisigObject = jsonObject.value("result").toArray().last().toObject();

        QString hotAddress = ethMultisigObject.value("new_address_hot").toString();
        QString coldAddress = ethMultisigObject.value("new_address_cold").toString();

        if(ui->assetComboBox->currentText() == "ETH")
        {
            fetchCoinBalance(1011,"ETH",hotAddress);
            fetchCoinBalance(1012,"ETH",coldAddress);
        }
        else
        {
            fetchCoinBalance(1011,"ETH",hotAddress);
            fetchCoinBalance(1012,"ETH",coldAddress);
//            fetchCoinBalance(1013,ui->assetComboBox->currentText(),hotAddress);
//            fetchCoinBalance(1014,ui->assetComboBox->currentText(),coldAddress);

        }

        return;
    }

    if(id == "id-senator_sign_eths_final_trx")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":null"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of senator-sign-eth-withdraw-final has been sent out!"));
            commonDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Failed!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        return;
    }

    if(id.startsWith("WithdrawConfirmPage+dump_crosschain_private_key+"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        QStringList strList = id.mid(QString("WithdrawConfirmPage+dump_crosschain_private_key+").size()).split("+");
        QString signer = strList.at(0);
        QString rowStr = strList.at(1);
        int row = rowStr.toInt();

        if(ui->ethFinalTrxTableWidget->item(row,1) && ui->ethFinalTrxTableWidget->item(row,3))
        {
            if(ui->ethFinalTrxTableWidget->item(row,1)->text() == signer)
            {
                if(result == "\"result\":[]")       // 钱包内无对应私钥
                {
                    ui->ethFinalTrxTableWidget->item(row,3)->setText(tr("no key"));

                    ToolButtonWidget* tbw = static_cast<ToolButtonWidget*>(ui->ethFinalTrxTableWidget->cellWidget(row,3));
                    tbw->setText(ui->ethFinalTrxTableWidget->item(row,3)->text());
                    tbw->setEnabled(false);
                }
                else if(result.startsWith("\"result\":[["))     //有私钥
                {
                    ui->ethFinalTrxTableWidget->item(row,3)->setText(tr("sign"));

                    ToolButtonWidget* tbw = static_cast<ToolButtonWidget*>(ui->ethFinalTrxTableWidget->cellWidget(row,3));
                    tbw->setText(ui->ethFinalTrxTableWidget->item(row,3)->text());
                    tbw->setEnabled(true);
                }
            }
        }

        return;
    }

    if(id == "WithdrawConfirmPage-refund_request")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of cancel-withdraw has been sent out!"));
            commonDialog.pop();
        }
        else if(result.startsWith("\"error\":"))
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Failed!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        return;
    }
}

void WithdrawConfirmPage::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    int id = object.value("id").toInt();
    QJsonObject resultObject = object.value("result").toObject();
    QString assetSymbol = resultObject.value("chainId").toString().toUpper();
    QString balance = resultObject.value("balance").toString();
    QString address = resultObject.value("address").toString();

    if(id == 1011)
    {
        ui->hotAddressBalanceLabel->setText( QString("%1 (%2 ETH)").arg(address).arg(balance));
    }
    else if(id == 1012)
    {
        ui->coldAddressBalanceLabel->setText( QString("%1 (%2 ETH)").arg(address).arg(balance));
    }
}

void WithdrawConfirmPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));
    painter.drawRect(rect());

    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(243,241,250),Qt::SolidPattern));
    painter.drawRect(0,87,this->width(),24);
}

void WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellClicked(int row, int column)
{
    if(column == 5)
    {
        if(ui->crosschainTransactionTableWidget->item(row,0))
        {
            QString trxId = ui->crosschainTransactionTableWidget->item(row,0)->data(Qt::UserRole).toString();

            qDebug() << trxId;

            WithdrawInfoDialog withdrawInfoDialog(this);
            withdrawInfoDialog.setTrxId(trxId);
            withdrawInfoDialog.pop();
        }

        return;
    }

    if(column == 6)
    {
        if(currentType == 1)
        {
            if(ui->crosschainTransactionTableWidget->item(row,0) && ui->crosschainTransactionTableWidget->item(row,4))
            {
                if(ui->crosschainTransactionTableWidget->item(row,4)->text() == tr("signed"))
                {
                    CommonDialog commonDialog(CommonDialog::OkOnly);
                    commonDialog.setText(tr("%1 has already signed!").arg(ui->accountComboBox->currentText()));
                    commonDialog.pop();

                    return;
                }

                QString trxId = ui->crosschainTransactionTableWidget->item(row,0)->data(Qt::UserRole).toString();

                QString generatedTrxId = HXChain::getInstance()->lookupGeneratedTrxByApplyTrxId(trxId);

                if(!generatedTrxId.isEmpty() && !ui->accountComboBox->currentText().isEmpty())
                {
                    HXChain::getInstance()->postRPC( "WithdrawConfirmPage-senator_sign_crosschain_transaction", toJsonFormat( "senator_sign_crosschain_transaction",
                                                     QJsonArray() << generatedTrxId << ui->accountComboBox->currentText()));

                }
            }
        }
        else if(currentType == 0)
        {
            if(ui->crosschainTransactionTableWidget->item(row,0) && ui->crosschainTransactionTableWidget->item(row,2))
            {
                CommonDialog commonDialog(CommonDialog::YesOrNo);
                commonDialog.setText(tr("Sure to cancel?"));
                if(commonDialog.pop())
                {
                    QString trxId = ui->crosschainTransactionTableWidget->item(row,0)->data(Qt::UserRole).toString();
                    QString withdrawAddress = ui->crosschainTransactionTableWidget->item(row,2)->text();
                    QString accountName = HXChain::getInstance()->addressToName(withdrawAddress);
                    if(!accountName.isEmpty())
                    {
                        HXChain::getInstance()->postRPC( "WithdrawConfirmPage-refund_request", toJsonFormat( "refund_request",
                                                         QJsonArray() << accountName << trxId << true));
                    }
                }

            }
        }


        return;
    }
}

void WithdrawConfirmPage::showCrosschainTransactions()
{
    if(currentType == 1)
    {
        QStringList keys = HXChain::getInstance()->applyTransactionMap.keys();
        int size = keys.size();
        ui->crosschainTransactionTableWidget->setRowCount(0);
        ui->crosschainTransactionTableWidget->setRowCount(size);

        for(int i = 0; i < size; i++)
        {
            ui->crosschainTransactionTableWidget->setRowHeight(i,40);

            ApplyTransaction at = HXChain::getInstance()->applyTransactionMap.value(keys.at(i));

            ui->crosschainTransactionTableWidget->setItem(i, 0, new QTableWidgetItem(toLocalTime(at.expirationTime)));
            ui->crosschainTransactionTableWidget->item(i,0)->setData(Qt::UserRole, at.trxId);

            ui->crosschainTransactionTableWidget->setItem(i, 1, new QTableWidgetItem(at.amount + " " + at.assetSymbol));

            ui->crosschainTransactionTableWidget->setItem(i, 2, new QTableWidgetItem(at.withdrawAddress));

            ui->crosschainTransactionTableWidget->setItem(i, 3, new QTableWidgetItem(at.crosschainAddress));

            ui->crosschainTransactionTableWidget->setItem(i, 4, new QTableWidgetItem(tr("checking")));

            ui->crosschainTransactionTableWidget->setItem(i, 5, new QTableWidgetItem(tr("check")));
            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->crosschainTransactionTableWidget->item(i,5)->text());
            ui->crosschainTransactionTableWidget->setCellWidget(i,5,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellClicked,this,i,5));

            ui->crosschainTransactionTableWidget->setItem(i, 6, new QTableWidgetItem(tr("sign")));
            ToolButtonWidget *toolButton2 = new ToolButtonWidget();
            toolButton2->setText(ui->crosschainTransactionTableWidget->item(i,6)->text());
            ui->crosschainTransactionTableWidget->setCellWidget(i,6,toolButton2);
            connect(toolButton2,&ToolButtonWidget::clicked,std::bind(&WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellClicked,this,i,6));

        }

        tableWidgetSetItemZebraColor(ui->crosschainTransactionTableWidget);
    }
    else if( currentType == 0)
    {
        QStringList keys = HXChain::getInstance()->pendingApplyTransactionMap.keys();
        int size = keys.size();
        ui->crosschainTransactionTableWidget->setRowCount(0);
        ui->crosschainTransactionTableWidget->setRowCount(size);

        for(int i = 0; i < size; i++)
        {
            ui->crosschainTransactionTableWidget->setRowHeight(i,40);

            ApplyTransaction at = HXChain::getInstance()->pendingApplyTransactionMap.value(keys.at(i));

            ui->crosschainTransactionTableWidget->setItem(i, 0, new QTableWidgetItem(toLocalTime(at.expirationTime)));
            ui->crosschainTransactionTableWidget->item(i,0)->setData(Qt::UserRole, at.trxId);

            ui->crosschainTransactionTableWidget->setItem(i, 1, new QTableWidgetItem(at.amount + " " + at.assetSymbol));

            ui->crosschainTransactionTableWidget->setItem(i, 2, new QTableWidgetItem(at.withdrawAddress));

            ui->crosschainTransactionTableWidget->setItem(i, 3, new QTableWidgetItem(at.crosschainAddress));

            ui->crosschainTransactionTableWidget->setItem(i, 4, new QTableWidgetItem(tr("waiting")));

            ui->crosschainTransactionTableWidget->setItem(i, 5, new QTableWidgetItem(tr("check")));
            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->crosschainTransactionTableWidget->item(i,5)->text());
            ui->crosschainTransactionTableWidget->setCellWidget(i,5,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellClicked,this,i,5));

            QString accountName = HXChain::getInstance()->addressToName(at.withdrawAddress);
            if(!accountName.isEmpty())
            {
                ui->crosschainTransactionTableWidget->setItem(i, 6, new QTableWidgetItem(tr("cancel")));
                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->crosschainTransactionTableWidget->item(i,6)->text());
                ui->crosschainTransactionTableWidget->setCellWidget(i,6,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellClicked,this,i,6));
            }

        }

        tableWidgetSetItemZebraColor(ui->crosschainTransactionTableWidget);
    }
    int page = (ui->crosschainTransactionTableWidget->rowCount()%ROWNUMBER==0 && ui->crosschainTransactionTableWidget->rowCount() != 0) ?
                ui->crosschainTransactionTableWidget->rowCount()/ROWNUMBER : ui->crosschainTransactionTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(ui->crosschainTransactionTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(pageWidget->GetCurrentPage());

    pageWidget->setVisible(0 != ui->crosschainTransactionTableWidget->rowCount());
    blankWidget->setVisible(ui->crosschainTransactionTableWidget->rowCount() == 0);


}

void WithdrawConfirmPage::refreshCrosschainTransactionsState()
{
    if(currentType != 1)    return;

    int rowCount = ui->crosschainTransactionTableWidget->rowCount();
    for(int i = 0; i < rowCount; i++)
    {
        QString trxId = ui->crosschainTransactionTableWidget->item(i,0)->data(Qt::UserRole).toString();
        QString generatedTrxId = HXChain::getInstance()->lookupGeneratedTrxByApplyTrxId(trxId);
        QStringList guardAddresses = HXChain::getInstance()->lookupSignedGuardsByGeneratedTrxId(generatedTrxId);

        QString currentAddress = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
        if(guardAddresses.contains(currentAddress))
        {
            ui->crosschainTransactionTableWidget->item(i,4)->setText(tr("signed"));
        }
        else
        {
            ui->crosschainTransactionTableWidget->item(i,4)->setText(tr("unsigned"));
        }
    }
}

void WithdrawConfirmPage::fetchEthBalance()
{
    AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());

    HXChain::getInstance()->postRPC( "WithdrawConfirmPage-get_multi_address_obj-" + ui->assetComboBox->currentText() + "-" + accountInfo.id,
                                     toJsonFormat( "get_multi_address_obj",
                                                   QJsonArray() << ui->assetComboBox->currentText() << accountInfo.id));

}

void WithdrawConfirmPage::showEthFinalTrxs()
{
    QStringList keys = HXChain::getInstance()->ethFinalTrxMap.keys();
    int size = keys.size();
    ui->ethFinalTrxTableWidget->setRowCount(0);
    ui->ethFinalTrxTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->ethFinalTrxTableWidget->setRowHeight(i,40);

        ETHFinalTrx eft = HXChain::getInstance()->ethFinalTrxMap.value(keys.at(i));

        ui->ethFinalTrxTableWidget->setItem(i, 0, new QTableWidgetItem(eft.symbol));
        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->ethFinalTrxTableWidget->item(i,0)->text());
        ui->ethFinalTrxTableWidget->setCellWidget(i, 0, assetIconItem);

        QString item1Str = eft.signer;
        QString senatorName = HXChain::getInstance()->getGuardNameByHotColdAddress(eft.signer);
        if(!senatorName.isEmpty())
        {
            item1Str.append("("+senatorName+")");
        }

        ui->ethFinalTrxTableWidget->setItem(i, 1, new QTableWidgetItem(item1Str));
        ui->ethFinalTrxTableWidget->item(i,1)->setData(Qt::UserRole, eft.trxId);

        ui->ethFinalTrxTableWidget->setItem(i, 2, new QTableWidgetItem(eft.nonce));

#ifndef SAFE_VERSION
        ui->ethFinalTrxTableWidget->setItem(i, 3, new QTableWidgetItem(tr("checking")));
        ToolButtonWidget *toolButton = new ToolButtonWidget();
        toolButton->setText(ui->ethFinalTrxTableWidget->item(i,3)->text());
        toolButton->setEnabled(false);
        ui->ethFinalTrxTableWidget->setCellWidget(i,3,toolButton);
        connect(toolButton,&ToolButtonWidget::clicked,std::bind(&WithdrawConfirmPage::on_ethFinalTrxTableWidget_cellClicked,this,i,3));

        HXChain::getInstance()->postRPC( "WithdrawConfirmPage+dump_crosschain_private_key+" + QString("%1+%2").arg(eft.signer).arg(i),
                                         toJsonFormat( "dump_crosschain_private_key",
                                                       QJsonArray() << eft.signer));
#else
        ui->ethFinalTrxTableWidget->setItem(i, 3, new QTableWidgetItem(tr("sign")));
        ToolButtonWidget *toolButton = new ToolButtonWidget();
        toolButton->setText(ui->ethFinalTrxTableWidget->item(i,3)->text());
        toolButton->setEnabled(true);
        ui->ethFinalTrxTableWidget->setCellWidget(i,3,toolButton);
        connect(toolButton,&ToolButtonWidget::clicked,std::bind(&WithdrawConfirmPage::on_ethFinalTrxTableWidget_cellClicked,this,i,3));
#endif
    }

    tableWidgetSetItemZebraColor(ui->ethFinalTrxTableWidget);
}

void WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellPressed(int row, int column)
{
    if( column == 2 || column == 3)
    {
        ShowContentDialog showContentDialog( ui->crosschainTransactionTableWidget->item(row, column)->text(),this);

        int x = ui->crosschainTransactionTableWidget->columnViewportPosition(column) + ui->crosschainTransactionTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->crosschainTransactionTableWidget->rowViewportPosition(row) - 10 + ui->crosschainTransactionTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->crosschainTransactionTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void WithdrawConfirmPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->hotAddressBalanceLabel->clear();
    ui->coldAddressBalanceLabel->clear();
    refresh();
}

void WithdrawConfirmPage::on_typeCurrentBtn_clicked()
{
    currentType = 1;
    ui->stackedWidget2->setCurrentIndex(0);
    ui->typeCurrentBtn->setChecked(true);
    ui->typeWaitingBtn->setChecked(false);
    ui->typeETHBtn->setChecked(false);
    ui->assetComboBox->hide();
    ui->assetLabel->hide();
    showCrosschainTransactions();
}

void WithdrawConfirmPage::on_typeWaitingBtn_clicked()
{
    currentType = 0;
    ui->stackedWidget2->setCurrentIndex(0);
    ui->typeCurrentBtn->setChecked(false);
    ui->typeWaitingBtn->setChecked(true);
    ui->typeETHBtn->setChecked(false);
    ui->assetComboBox->hide();
    ui->assetLabel->hide();
    showCrosschainTransactions();
}

void WithdrawConfirmPage::on_typeETHBtn_clicked()
{
    currentType = 2;
    ui->stackedWidget2->setCurrentIndex(1);
    ui->typeCurrentBtn->setChecked(false);
    ui->typeWaitingBtn->setChecked(false);
    ui->assetComboBox->show();
    ui->assetLabel->show();
    ui->typeETHBtn->setChecked(true);
}

void WithdrawConfirmPage::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->crosschainTransactionTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->crosschainTransactionTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->crosschainTransactionTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->crosschainTransactionTableWidget->setRowHidden(i,true);
        }
    }
}


void WithdrawConfirmPage::on_ethFinalTrxTableWidget_cellClicked(int row, int column)
{
    if(column == 3)
    {
        if(ui->ethFinalTrxTableWidget->item(row,1))
        {
            QString trxId = ui->ethFinalTrxTableWidget->item(row,1)->data(Qt::UserRole).toString();

            qDebug() << trxId;

            HXChain::getInstance()->postRPC( "id-senator_sign_eths_final_trx",
                                             toJsonFormat( "senator_sign_eths_final_trx",
                                                           QJsonArray() << trxId << ui->accountComboBox->currentText()));

        }

        return;
    }
}

void WithdrawConfirmPage::onAssetComboBoxCurrentIndexChanged(const QString &arg1)
{
    ui->hotAddressBalanceLabel->clear();
    ui->coldAddressBalanceLabel->clear();
    refresh();
}

void WithdrawConfirmPage::fetchCoinBalance(int id, QString chainId, QString address)
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",id);
    object.insert("method","Zchain.Address.GetBalance");
    QJsonObject paramObject;
    paramObject.insert("chainId",chainId);
    paramObject.insert("addr",address);
    object.insert("params",paramObject);
    httpManager.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());
}

void WithdrawConfirmPage::on_ethFinalTrxTableWidget_cellPressed(int row, int column)
{
    if( column == 1)
    {
        ShowContentDialog showContentDialog( ui->ethFinalTrxTableWidget->item(row, column)->text(),this);

        int x = ui->ethFinalTrxTableWidget->columnViewportPosition(column) + ui->ethFinalTrxTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->ethFinalTrxTableWidget->rowViewportPosition(row) - 10 + ui->ethFinalTrxTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->ethFinalTrxTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void WithdrawConfirmPage::on_autoSignCheckBox_stateChanged(int arg1)
{
    if(!inited) return;

    HXChain::getInstance()->autoWithdrawConfirm = !HXChain::getInstance()->autoWithdrawConfirm;
    HXChain::getInstance()->configFile->setValue("/settings/autoWithdrawConfirm", HXChain::getInstance()->autoWithdrawConfirm);

    ui->autoSignSetBtn->setVisible(ui->autoSignCheckBox->isChecked());
}

void WithdrawConfirmPage::on_autoSignSetBtn_clicked()
{
    AutoWithdrawSetDialog autoWithdrawSetDialog;
    autoWithdrawSetDialog.pop();
}
