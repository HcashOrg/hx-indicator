#include "ColdHotTransferPage.h"
#include "ui_ColdHotTransferPage.h"

#include "wallet.h"
#include "ColdHotTransferDialog.h"
#include "ToolButtonWidget.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/checkpwddialog.h"
#include "commondialog.h"
#include "showcontentdialog.h"
#include "ColdHotInfoDialog.h"
#include "control/AssetIconItem.h"

static const int ROWNUMBER = 6;
ColdHotTransferPage::ColdHotTransferPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColdHotTransferPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->coldHotTransactionTableWidget->installEventFilter(this);
    ui->coldHotTransactionTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->coldHotTransactionTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->coldHotTransactionTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->coldHotTransactionTableWidget->setMouseTracking(true);
    ui->coldHotTransactionTableWidget->setShowGrid(false);//隐藏表格线

    ui->coldHotTransactionTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->coldHotTransactionTableWidget->horizontalHeader()->setVisible(true);
    ui->coldHotTransactionTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->coldHotTransactionTableWidget->setColumnWidth(0,120);
    ui->coldHotTransactionTableWidget->setColumnWidth(1,100);
    ui->coldHotTransactionTableWidget->setColumnWidth(2,100);
    ui->coldHotTransactionTableWidget->setColumnWidth(3,100);
    ui->coldHotTransactionTableWidget->setColumnWidth(4,80);
    ui->coldHotTransactionTableWidget->setColumnWidth(5,80);
    ui->coldHotTransactionTableWidget->setColumnWidth(6,80);
    ui->coldHotTransactionTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

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

    ui->transferBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

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
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&ColdHotTransferPage::pageChangeSlot);

    ui->stackedWidget2->setCurrentIndex(0);

    blankWidget = new BlankDefaultWidget(ui->coldHotTransactionTableWidget);
    blankWidget->setTextTip(tr("There are no cold-hot trxs currently!"));
    init();
}

ColdHotTransferPage::~ColdHotTransferPage()
{
    delete ui;
}

void ColdHotTransferPage::init()
{
    inited = false;

    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

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

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->coldHotTransactionTableWidget);
    HXChain::getInstance()->mainFrame->installBlurEffect(ui->ethFinalTrxTableWidget);

    ui->typeCurrentBtn->setChecked(true);

    fetchColdHotTransaction();

    inited = true;
}

void ColdHotTransferPage::refresh()
{
    fetchColdHotTransaction();
    fetchEthBalance();
}

void ColdHotTransferPage::jsonDataUpdated(QString id)
{
    if( id.startsWith("id-get_coldhot_transaction-") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        int type = id.mid(QString("id-get_coldhot_transaction-").size()).toInt();

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();

            if(type == 1)
            {
                coldHotTransactionMap.clear();
                coldHotCrosschainTransactionMap.clear();
            }
            else if(type == 2)
            {
                coldHotSignTransactionMap.clear();
            }
            else if(type == 0)
            {
                pendingColdHotTransactionMap.clear();
            }
            else if(type == 6)
            {
                ethFinalTrxMap.clear();
                foreach (QJsonValue v, array)
                {
                    QJsonArray array2 = v.toArray();
                    QJsonObject object = array2.at(1).toObject();
                    QJsonArray operationArray = object.take("operations").toArray();
                    QJsonArray array3 = operationArray.at(0).toArray();
                    int operationType = array3.at(0).toInt();
                    QJsonObject operationObject = array3.at(1).toObject();

                    if( operationType == TRANSACTION_TYPE_COLDHOT_FINAL)
                    {
                        QJsonObject ethObject = operationObject.value("coldhot_trx_original_chain").toObject();
                        ETHFinalTrx eft;
                        eft.trxId = array2.at(0).toString();
                        eft.signer = ethObject.value("signer").toString();
                        eft.nonce = ethObject.value("nonce").toString();
                        eft.symbol = operationObject.value("asset_symbol").toString();

                        ethFinalTrxMap.insert(eft.trxId, eft);
                    }
                }

                showEthFinalTrxs();
                return;
            }

            foreach (QJsonValue v, array)
            {
                QJsonArray array2 = v.toArray();
                QJsonObject object = array2.at(1).toObject();
                QJsonArray operationArray = object.take("operations").toArray();
                QJsonArray array3 = operationArray.at(0).toArray();
                int operationType = array3.at(0).toInt();
                QJsonObject operationObject = array3.at(1).toObject();

                if(operationType == TRANSACTION_TYPE_COLDHOT_CROSSCHAIN)
                {
                    ColdHotCrosschainTransaction chct;
                    chct.trxId = array2.at(0).toString();
                    chct.coldHotTrxId = operationObject.take("coldhot_trx_id").toString();

                    coldHotCrosschainTransactionMap.insert(chct.trxId, chct);
                }
                else if(operationType == TRANSACTION_TYPE_COLDHOT)
                {
                    ColdHotTransaction cht;
                    cht.trxId           = array2.at(0).toString();
                    cht.expirationTime  = object.take("expiration").toString();
                    cht.withdrawAddress = operationObject.take("multi_account_withdraw").toString();
                    cht.depositAddress  = operationObject.take("multi_account_deposit").toString();
                    cht.amount          = operationObject.take("amount").toString();
                    cht.assetSymbol     = operationObject.take("asset_symbol").toString();
                    cht.assetId         = operationObject.take("asset_id").toString();
                    cht.guard           = operationObject.take("guard").toString();
                    cht.guardId         = operationObject.take("guard_id").toString();
                    cht.memo            = operationObject.take("memo").toString();

                    if(type == 0)
                    {
                        pendingColdHotTransactionMap.insert(cht.trxId, cht);
                    }
                    else
                    {
                        coldHotTransactionMap.insert(cht.trxId, cht);
                    }
                }
                else if(operationType == TRANSACTION_TYPE_COLDHOT_SIGN)
                {
                    ColdHotSignTransaction chst;
                    chst.trxId          = array2.at(0).toString();
                    chst.coldHotCrosschainTrxId = operationObject.take("coldhot_trx_id").toString();
                    chst.guardAddress   = operationObject.take("guard_address").toString();

                    coldHotSignTransactionMap.insert(chst.trxId, chst);
                }
            }

            if(type == 0 || type == 1)
            {
                showColdHotTransactions();
            }
            else if(type == 2)
            {
                refreshColdHotTtransactionsState();
            }
        }

        return;
    }

    if(id == "id-senator_sign_coldhot_transaction")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":null"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of senator-sign-coldhot has been sent out!"));
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

    if(id.startsWith("ColdHotTransferPage-get_multi_address_obj-" + ui->assetComboBox->currentText() + "-"))
    {
        QString accountId = id.mid(QString("ColdHotTransferPage-get_multi_address_obj-" + ui->assetComboBox->currentText() + "-").size());
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
        }
        return;
    }

    if(id == "id-senator_sign_eths_coldhot_final_trx")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":null"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of senator-sign-eth-coldhot-final has been sent out!"));
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

    if(id.startsWith("ColdHotTransferPage+dump_crosschain_private_key+"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        QStringList strList = id.mid(QString("ColdHotTransferPage+dump_crosschain_private_key+").size()).split("+");
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
}

void ColdHotTransferPage::httpReplied(QByteArray _data, int _status)
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

void ColdHotTransferPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));
    painter.drawRect(rect());

    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(243,241,250),Qt::SolidPattern));
    painter.drawRect(0,87,this->width(),24);
}

void ColdHotTransferPage::on_transferBtn_clicked()
{
    ColdHotTransferDialog coldHotTransferDialog;
    coldHotTransferDialog.pop();
}

void ColdHotTransferPage::fetchColdHotTransaction()
{
    HXChain::getInstance()->postRPC( "id-get_coldhot_transaction-" + QString::number(0), toJsonFormat( "get_coldhot_transaction",
                                                                                     QJsonArray() << 0));

    HXChain::getInstance()->postRPC( "id-get_coldhot_transaction-" + QString::number(1), toJsonFormat( "get_coldhot_transaction",
                                                                                     QJsonArray() << 1));

    HXChain::getInstance()->postRPC( "id-get_coldhot_transaction-" + QString::number(2), toJsonFormat( "get_coldhot_transaction",
                                                                                     QJsonArray() << 2));

    HXChain::getInstance()->postRPC( "id-get_coldhot_transaction-" + QString::number(6), toJsonFormat( "get_coldhot_transaction",
                                                                                     QJsonArray() << 6));

}

void ColdHotTransferPage::showColdHotTransactions()
{
    if(currentType == 1)
    {
        QStringList keys = coldHotTransactionMap.keys();
        int size = keys.size();
        ui->coldHotTransactionTableWidget->setRowCount(0);
        ui->coldHotTransactionTableWidget->setRowCount(size);

        for(int i = 0; i < size; i++)
        {
            ui->coldHotTransactionTableWidget->setRowHeight(i,40);

            ColdHotTransaction cht = coldHotTransactionMap.value(keys.at(i));

            ui->coldHotTransactionTableWidget->setItem(i, 0, new QTableWidgetItem(toLocalTime(cht.expirationTime)));
            ui->coldHotTransactionTableWidget->item(i,0)->setData(Qt::UserRole, cht.trxId);

            ui->coldHotTransactionTableWidget->setItem(i, 1, new QTableWidgetItem(cht.amount + " " + cht.assetSymbol));

            ui->coldHotTransactionTableWidget->setItem(i, 2, new QTableWidgetItem(cht.withdrawAddress));

            ui->coldHotTransactionTableWidget->setItem(i, 3, new QTableWidgetItem(cht.depositAddress));

            ui->coldHotTransactionTableWidget->setItem(i, 4, new QTableWidgetItem(tr("checking")));

            ui->coldHotTransactionTableWidget->setItem(i, 5, new QTableWidgetItem(tr("check")));
            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->coldHotTransactionTableWidget->item(i,5)->text());
            ui->coldHotTransactionTableWidget->setCellWidget(i,5,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ColdHotTransferPage::on_coldHotTransactionTableWidget_cellClicked,this,i,5));


            ui->coldHotTransactionTableWidget->setItem(i, 6, new QTableWidgetItem(tr("sign")));
            ToolButtonWidget *toolButton2 = new ToolButtonWidget();
            toolButton2->setText(ui->coldHotTransactionTableWidget->item(i,6)->text());
            ui->coldHotTransactionTableWidget->setCellWidget(i,6,toolButton2);
            connect(toolButton2,&ToolButtonWidget::clicked,std::bind(&ColdHotTransferPage::on_coldHotTransactionTableWidget_cellClicked,this,i,6));

        }

        tableWidgetSetItemZebraColor(ui->coldHotTransactionTableWidget);
    }
    else if(currentType == 0)
    {
        QStringList keys = pendingColdHotTransactionMap.keys();
        int size = keys.size();
        ui->coldHotTransactionTableWidget->setRowCount(0);
        ui->coldHotTransactionTableWidget->setRowCount(size);

        for(int i = 0; i < size; i++)
        {
            ui->coldHotTransactionTableWidget->setRowHeight(i,40);

            ColdHotTransaction cht = pendingColdHotTransactionMap.value(keys.at(i));

            ui->coldHotTransactionTableWidget->setItem(i, 0, new QTableWidgetItem(toLocalTime(cht.expirationTime)));
            ui->coldHotTransactionTableWidget->item(i,0)->setData(Qt::UserRole, cht.trxId);

            ui->coldHotTransactionTableWidget->setItem(i, 1, new QTableWidgetItem(cht.amount + " " + cht.assetSymbol));

            ui->coldHotTransactionTableWidget->setItem(i, 2, new QTableWidgetItem(cht.withdrawAddress));

            ui->coldHotTransactionTableWidget->setItem(i, 3, new QTableWidgetItem(cht.depositAddress));

            ui->coldHotTransactionTableWidget->setItem(i, 4, new QTableWidgetItem(tr("waiting")));

            ui->coldHotTransactionTableWidget->setItem(i, 5, new QTableWidgetItem(tr("check")));
            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->coldHotTransactionTableWidget->item(i,5)->text());
            ui->coldHotTransactionTableWidget->setCellWidget(i,5,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ColdHotTransferPage::on_coldHotTransactionTableWidget_cellClicked,this,i,5));
        }

        tableWidgetSetItemZebraColor(ui->coldHotTransactionTableWidget);
    }
    int page = (ui->coldHotTransactionTableWidget->rowCount()%ROWNUMBER==0 && ui->coldHotTransactionTableWidget->rowCount() != 0) ?
                ui->coldHotTransactionTableWidget->rowCount()/ROWNUMBER : ui->coldHotTransactionTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(ui->coldHotTransactionTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(0);

    pageWidget->setVisible(0 != ui->coldHotTransactionTableWidget->rowCount());
    blankWidget->setVisible(ui->coldHotTransactionTableWidget->rowCount() == 0);

}

void ColdHotTransferPage::refreshColdHotTtransactionsState()
{
    if(currentType != 1)    return;

    int rowCount = ui->coldHotTransactionTableWidget->rowCount();

    for(int i = 0; i < rowCount; i++)
    {
        QString trxId = ui->coldHotTransactionTableWidget->item(i,0)->data(Qt::UserRole).toString();
        QString crosschainTrxId = lookupCrosschainTrxByColdHotTrxId(trxId);
        QStringList guardAddresses = lookupSignedGuardByCrosschainTrx(crosschainTrxId);
        QString currentAddress = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;

        if(guardAddresses.contains(currentAddress))
        {
            ui->coldHotTransactionTableWidget->item(i,4)->setText(tr("signed"));
        }
        else
        {
            ui->coldHotTransactionTableWidget->item(i,4)->setText(tr("unsigned"));
        }
    }
}

void ColdHotTransferPage::fetchEthBalance()
{
    AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());

    HXChain::getInstance()->postRPC( "ColdHotTransferPage-get_multi_address_obj-" + ui->assetComboBox->currentText() + "-" + accountInfo.id,
                                     toJsonFormat( "get_multi_address_obj",
                                                   QJsonArray() << ui->assetComboBox->currentText() << accountInfo.id));
}

void ColdHotTransferPage::showEthFinalTrxs()
{
    QStringList keys = ethFinalTrxMap.keys();
    int size = keys.size();
    ui->ethFinalTrxTableWidget->setRowCount(0);
    ui->ethFinalTrxTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->ethFinalTrxTableWidget->setRowHeight(i,40);

        ETHFinalTrx eft = ethFinalTrxMap.value(keys.at(i));

        ui->ethFinalTrxTableWidget->setItem(i, 0, new QTableWidgetItem(eft.symbol));
        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->ethFinalTrxTableWidget->item(i,0)->text());
        ui->ethFinalTrxTableWidget->setCellWidget(i, 0, assetIconItem);

        ui->ethFinalTrxTableWidget->setItem(i, 1, new QTableWidgetItem(eft.signer));
        ui->ethFinalTrxTableWidget->item(i,1)->setData(Qt::UserRole, eft.trxId);

        ui->ethFinalTrxTableWidget->setItem(i, 2, new QTableWidgetItem(eft.nonce));

        ui->ethFinalTrxTableWidget->setItem(i, 3, new QTableWidgetItem(tr("checking")));
        ToolButtonWidget *toolButton = new ToolButtonWidget();
        toolButton->setText(ui->ethFinalTrxTableWidget->item(i,3)->text());
        toolButton->setEnabled(false);
        ui->ethFinalTrxTableWidget->setCellWidget(i,3,toolButton);
        connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ColdHotTransferPage::on_ethFinalTrxTableWidget_cellClicked,this,i,3));

        HXChain::getInstance()->postRPC( "ColdHotTransferPage+dump_crosschain_private_key+" + QString("%1+%2").arg(eft.signer).arg(i),
                                         toJsonFormat( "dump_crosschain_private_key",
                                                       QJsonArray() << eft.signer));

    }

    tableWidgetSetItemZebraColor(ui->ethFinalTrxTableWidget);
}

QString ColdHotTransferPage::lookupCrosschainTrxByColdHotTrxId(QString coldHotTrxId)
{
    QString result;
    QStringList keys = coldHotCrosschainTransactionMap.keys();
    foreach (QString key, keys)
    {
        if(coldHotCrosschainTransactionMap.value(key).coldHotTrxId == coldHotTrxId)
        {
            result += key;
            break;
        }
    }

    return result;
}

QStringList ColdHotTransferPage::lookupSignedGuardByCrosschainTrx(QString crosschainTrxId)
{
    QStringList result;
    QStringList keys = coldHotSignTransactionMap.keys();

    foreach (QString key, keys)
    {
        if(coldHotSignTransactionMap.value(key).coldHotCrosschainTrxId == crosschainTrxId)
        {
            result += coldHotSignTransactionMap.value(key).guardAddress;
        }
    }

    return result;
}

void ColdHotTransferPage::on_coldHotTransactionTableWidget_cellClicked(int row, int column)
{
    if(column == 5)
    {
        if(ui->coldHotTransactionTableWidget->item(row,0))
        {
            QString trxId = ui->coldHotTransactionTableWidget->item(row,0)->data(Qt::UserRole).toString();

            qDebug() << trxId;

            ColdHotInfoDialog coldHotInfoDialog(this);
            coldHotInfoDialog.setTrxId(trxId);
            coldHotInfoDialog.pop();
        }

        return;
    }

    if(column == 6)
    {
        if(currentType != 1)    return;

        if(ui->coldHotTransactionTableWidget->item(row,4))
        {
            if(ui->coldHotTransactionTableWidget->item(row,4)->text() == tr("signed"))
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText(tr("%1 has already signed this cold-hot transaction!").arg(ui->accountComboBox->currentText()));
                commonDialog.pop();
                return;
            }
            else if(ui->coldHotTransactionTableWidget->item(row,4)->text() == tr("checking"))
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText(tr("Wait for checking the signature of %1!").arg(ui->accountComboBox->currentText()));
                commonDialog.pop();
                return;
            }

            CheckPwdDialog checkPwdDialog;
            if(!checkPwdDialog.pop())   return;

            if(ui->coldHotTransactionTableWidget->item(row,0))
            {
                QString trxId = ui->coldHotTransactionTableWidget->item(row,0)->data(Qt::UserRole).toString();

                QString chcTrxId = lookupCrosschainTrxByColdHotTrxId(trxId);

                if(!chcTrxId.isEmpty() && !ui->accountComboBox->currentText().isEmpty())
                {
                    HXChain::getInstance()->postRPC( "id-senator_sign_coldhot_transaction", toJsonFormat( "senator_sign_coldhot_transaction",
                                                                                                        QJsonArray() << chcTrxId << ui->accountComboBox->currentText()));

                }
            }
        }

        return;
    }
}

void ColdHotTransferPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->hotAddressBalanceLabel->clear();
    ui->coldAddressBalanceLabel->clear();
    refresh();
}

void ColdHotTransferPage::on_coldHotTransactionTableWidget_cellPressed(int row, int column)
{
    if( column == 2 || column == 3)
    {
        ShowContentDialog showContentDialog( ui->coldHotTransactionTableWidget->item(row, column)->text(),this);

        int x = ui->coldHotTransactionTableWidget->columnViewportPosition(column) + ui->coldHotTransactionTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->coldHotTransactionTableWidget->rowViewportPosition(row) - 10 + ui->coldHotTransactionTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->coldHotTransactionTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void ColdHotTransferPage::on_typeCurrentBtn_clicked()
{
    currentType = 1;
    ui->stackedWidget2->setCurrentIndex(0);
    ui->typeCurrentBtn->setChecked(true);
    ui->typeWaitingBtn->setChecked(false);
    ui->typeETHBtn->setChecked(false);
    ui->assetComboBox->hide();
    ui->assetLabel->hide();
    showColdHotTransactions();
}

void ColdHotTransferPage::on_typeWaitingBtn_clicked()
{
    currentType = 0;
    ui->stackedWidget2->setCurrentIndex(0);
    ui->typeCurrentBtn->setChecked(false);
    ui->typeWaitingBtn->setChecked(true);
    ui->typeETHBtn->setChecked(false);
    ui->assetComboBox->hide();
    ui->assetLabel->hide();
    showColdHotTransactions();
}

void ColdHotTransferPage::on_typeETHBtn_clicked()
{
    currentType = 2;
    ui->stackedWidget2->setCurrentIndex(1);
    ui->typeCurrentBtn->setChecked(false);
    ui->typeWaitingBtn->setChecked(false);
    ui->assetComboBox->show();
    ui->assetLabel->show();
    ui->typeETHBtn->setChecked(true);
}

void ColdHotTransferPage::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->coldHotTransactionTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->coldHotTransactionTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->coldHotTransactionTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->coldHotTransactionTableWidget->setRowHidden(i,true);
        }
    }
}



void ColdHotTransferPage::on_ethFinalTrxTableWidget_cellClicked(int row, int column)
{
    if(column == 3)
    {
        if(ui->ethFinalTrxTableWidget->item(row,1))
        {
            QString trxId = ui->ethFinalTrxTableWidget->item(row,1)->data(Qt::UserRole).toString();

            qDebug() << trxId;

            HXChain::getInstance()->postRPC( "id-senator_sign_eths_coldhot_final_trx",
                                             toJsonFormat( "senator_sign_eths_coldhot_final_trx",
                                                           QJsonArray() << trxId << ui->accountComboBox->currentText()));

        }

        return;
    }

}

void ColdHotTransferPage::on_assetComboBox_currentIndexChanged(const QString &arg1)
{

}

void ColdHotTransferPage::on_ethFinalTrxTableWidget_cellPressed(int row, int column)
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

void ColdHotTransferPage::fetchCoinBalance(int id, QString chainId, QString address)
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
