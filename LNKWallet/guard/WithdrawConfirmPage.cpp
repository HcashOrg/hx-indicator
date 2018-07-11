#include "WithdrawConfirmPage.h"
#include "ui_WithdrawConfirmPage.h"

#include "wallet.h"
#include "ToolButtonWidget.h"
#include "commondialog.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "showcontentdialog.h"
#include "WithdrawInfoDialog.h"

WithdrawConfirmPage::WithdrawConfirmPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WithdrawConfirmPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->crosschainTransactionTableWidget->installEventFilter(this);
    ui->crosschainTransactionTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->crosschainTransactionTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->crosschainTransactionTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->crosschainTransactionTableWidget->setMouseTracking(true);
    ui->crosschainTransactionTableWidget->setShowGrid(false);//隐藏表格线

    ui->crosschainTransactionTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->crosschainTransactionTableWidget->horizontalHeader()->setVisible(true);
    ui->crosschainTransactionTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->crosschainTransactionTableWidget->setColumnWidth(0,120);
    ui->crosschainTransactionTableWidget->setColumnWidth(1,100);
    ui->crosschainTransactionTableWidget->setColumnWidth(2,100);
    ui->crosschainTransactionTableWidget->setColumnWidth(3,100);
    ui->crosschainTransactionTableWidget->setColumnWidth(4,80);
    ui->crosschainTransactionTableWidget->setColumnWidth(5,80);
    ui->crosschainTransactionTableWidget->setColumnWidth(6,80);
    ui->crosschainTransactionTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->typeCurrentBtn->setCheckable(true);
    ui->typeWaitingBtn->setCheckable(true);

    setStyleSheet("QToolButton{background:transparent;border:none;color: rgb(51,51,51);font:12px \"微软雅黑\";padding:0px 0px 1px 0px;}"
                  "QToolButton::checked{background-color:rgb(84,116,235);border-radius:12px;color:white;}");

    init();
}

WithdrawConfirmPage::~WithdrawConfirmPage()
{
    delete ui;
}

void WithdrawConfirmPage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = UBChain::getInstance()->getMyFormalGuards();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    UBChain::getInstance()->mainFrame->installBlurEffect(ui->crosschainTransactionTableWidget);

    ui->typeCurrentBtn->setChecked(true);

    fetchCrosschainTransactions();
}

void WithdrawConfirmPage::refresh()
{
    fetchCrosschainTransactions();
}

void WithdrawConfirmPage::fetchCrosschainTransactions()
{
    UBChain::getInstance()->postRPC( "id-get_crosschain_transaction-" + QString::number(0), toJsonFormat( "get_crosschain_transaction",
                                                                                    QJsonArray() << 0));

    UBChain::getInstance()->postRPC( "id-get_crosschain_transaction-" + QString::number(1), toJsonFormat( "get_crosschain_transaction",
                                                                                    QJsonArray() << 1));

    UBChain::getInstance()->postRPC( "id-get_crosschain_transaction-" + QString::number(2), toJsonFormat( "get_crosschain_transaction",
                                                                                    QJsonArray() << 2));
}

void WithdrawConfirmPage::jsonDataUpdated(QString id)
{
    if(id.startsWith("id-get_crosschain_transaction-"))
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        int type = id.mid(QString("id-get_crosschain_transaction-").size()).toInt();
        if(type == 1)
        {
            applyTransactionMap.clear();
            generatedTransactionMap.clear();
        }
        else if(type == 2)
        {
            signTransactionMap.clear();
        }
        else if(type == 0)
        {
            pendingApplyTransactionMap.clear();
        }

        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonArray array = jsonObject.take("result").toArray();

        foreach (QJsonValue v, array)
        {
            QJsonArray array2 = v.toArray();
            QJsonObject object = array2.at(1).toObject();
            QJsonArray operationArray = object.take("operations").toArray();
            QJsonArray array3 = operationArray.at(0).toArray();
            int operationType = array3.at(0).toInt();
            QJsonObject operationObject = array3.at(1).toObject();

            if(operationType == TRANSACTION_TYPE_WITHDRAW_CROSSCHAIN)
            {
                GeneratedTransaction gt;
                gt.trxId = array2.at(0).toString();
                QJsonArray ccwArray = operationObject.take("ccw_trx_ids").toArray();
                foreach (QJsonValue v, ccwArray)
                {
                    gt.ccwTrxIds += v.toString();
                }

                generatedTransactionMap.insert(gt.trxId, gt);
            }
            else if(operationType == TRANSACTION_TYPE_WITHDRAW)
            {
                ApplyTransaction at;
                at.trxId            = array2.at(0).toString();
                at.expirationTime   = object.take("expiration").toString();
                at.withdrawAddress  = operationObject.take("withdraw_account").toString();
                at.amount           = operationObject.take("amount").toString();
                at.assetSymbol      = operationObject.take("asset_symbol").toString();
                at.assetId          = operationObject.take("asset_id").toString();
                at.crosschainAddress  = operationObject.take("crosschain_account").toString();
                at.memo             = operationObject.take("memo").toString();

                if(type == 0)
                {
                    pendingApplyTransactionMap.insert(at.trxId, at);
                }
                else
                {
                    applyTransactionMap.insert(at.trxId, at);
                }
            }
            else if(operationType == TRANSACTION_TYPE_WITHDRAW_SIGN)
            {
                SignTransaction st;
                st.trxId            = array2.at(0).toString();
                st.generatedTrxId   = operationObject.take("ccw_trx_id").toString();
                st.guardAddress     = operationObject.take("guard_address").toString();

                signTransactionMap.insert(st.trxId, st);
            }
        }

        if(type != 2)
        {
            showCrosschainTransactions();
        }
        else
        {
            refreshCrosschainTransactionsState();
        }

        return;
    }

    if(id == "id-guard_sign_crosschain_transaction")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {            
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Transaction of guard-sign-crosschain has been sent out!"));
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

void WithdrawConfirmPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
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
        if(currentType != 1)    return;
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

            QString generatedTrxId = lookupGeneratedTrxByApplyTrxId(trxId);

            if(!generatedTrxId.isEmpty() && !ui->accountComboBox->currentText().isEmpty())
            {
                UBChain::getInstance()->postRPC( "id-guard_sign_crosschain_transaction", toJsonFormat( "guard_sign_crosschain_transaction",
                                                 QJsonArray() << generatedTrxId << ui->accountComboBox->currentText()));

            }
        }

        return;
    }
}

void WithdrawConfirmPage::showCrosschainTransactions()
{
    if(currentType == 1)
    {
        QStringList keys = applyTransactionMap.keys();
        int size = keys.size();
        ui->crosschainTransactionTableWidget->setRowCount(0);
        ui->crosschainTransactionTableWidget->setRowCount(size);

        for(int i = 0; i < size; i++)
        {
            ui->crosschainTransactionTableWidget->setRowHeight(i,40);

            ApplyTransaction at = applyTransactionMap.value(keys.at(i));

            QDateTime time = QDateTime::fromString(at.expirationTime, "yyyy-MM-ddThh:mm:ss");
            time = time.addSecs(-600);       // 时间减10分钟
            QString currentDateTime = time.toString("yyyy-MM-dd\nhh:mm:ss");
            ui->crosschainTransactionTableWidget->setItem(i, 0, new QTableWidgetItem(currentDateTime));
            ui->crosschainTransactionTableWidget->item(i,0)->setData(Qt::UserRole, at.trxId);

            ui->crosschainTransactionTableWidget->setItem(i, 1, new QTableWidgetItem(at.amount + " " + at.assetSymbol));

            ui->crosschainTransactionTableWidget->setItem(i, 2, new QTableWidgetItem(at.withdrawAddress));

            ui->crosschainTransactionTableWidget->setItem(i, 3, new QTableWidgetItem(at.crosschainAddress));

            ui->crosschainTransactionTableWidget->setItem(i, 4, new QTableWidgetItem(tr("checking")));

            ui->crosschainTransactionTableWidget->setItem(i, 5, new QTableWidgetItem(tr("check")));
            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->crosschainTransactionTableWidget->item(i,5)->text());
    //            toolButton->setBackgroundColor(itemColor);
            ui->crosschainTransactionTableWidget->setCellWidget(i,5,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellClicked,this,i,5));

            ui->crosschainTransactionTableWidget->setItem(i, 6, new QTableWidgetItem(tr("sign")));
            ToolButtonWidget *toolButton2 = new ToolButtonWidget();
            toolButton2->setText(ui->crosschainTransactionTableWidget->item(i,6)->text());
    //            toolButton->setBackgroundColor(itemColor);
            ui->crosschainTransactionTableWidget->setCellWidget(i,6,toolButton2);
            connect(toolButton2,&ToolButtonWidget::clicked,std::bind(&WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellClicked,this,i,6));


            for (int j : {0,1,2,3,4})
            {
                if(i%2)
                {
                    ui->crosschainTransactionTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                    ui->crosschainTransactionTableWidget->item(i,j)->setBackgroundColor(QColor(252,253,255));
                }
                else
                {
                    ui->crosschainTransactionTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                    ui->crosschainTransactionTableWidget->item(i,j)->setBackgroundColor(QColor("white"));
                }
            }
        }
    }
    else if( currentType == 0)
    {
        QStringList keys = pendingApplyTransactionMap.keys();
        int size = keys.size();
        ui->crosschainTransactionTableWidget->setRowCount(0);
        ui->crosschainTransactionTableWidget->setRowCount(size);

        for(int i = 0; i < size; i++)
        {
            ui->crosschainTransactionTableWidget->setRowHeight(i,40);

            ApplyTransaction at = pendingApplyTransactionMap.value(keys.at(i));

            QDateTime time = QDateTime::fromString(at.expirationTime, "yyyy-MM-ddThh:mm:ss");
            time = time.addSecs(-600);       // 时间减10分钟
            QString currentDateTime = time.toString("yyyy-MM-dd\nhh:mm:ss");
            ui->crosschainTransactionTableWidget->setItem(i, 0, new QTableWidgetItem(currentDateTime));
            ui->crosschainTransactionTableWidget->item(i,0)->setData(Qt::UserRole, at.trxId);

            ui->crosschainTransactionTableWidget->setItem(i, 1, new QTableWidgetItem(at.amount + " " + at.assetSymbol));

            ui->crosschainTransactionTableWidget->setItem(i, 2, new QTableWidgetItem(at.withdrawAddress));

            ui->crosschainTransactionTableWidget->setItem(i, 3, new QTableWidgetItem(at.crosschainAddress));

            ui->crosschainTransactionTableWidget->setItem(i, 4, new QTableWidgetItem(tr("waiting")));

            ui->crosschainTransactionTableWidget->setItem(i, 5, new QTableWidgetItem(tr("check")));
            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->crosschainTransactionTableWidget->item(i,5)->text());
    //            toolButton->setBackgroundColor(itemColor);
            ui->crosschainTransactionTableWidget->setCellWidget(i,5,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellClicked,this,i,5));



            for (int j : {0,1,2,3,4})
            {
                if(i%2)
                {
                    ui->crosschainTransactionTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                    ui->crosschainTransactionTableWidget->item(i,j)->setBackgroundColor(QColor(252,253,255));
                }
                else
                {
                    ui->crosschainTransactionTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                    ui->crosschainTransactionTableWidget->item(i,j)->setBackgroundColor(QColor("white"));
                }
            }
        }
    }


}

void WithdrawConfirmPage::refreshCrosschainTransactionsState()
{
    if(currentType != 1)    return;

    int rowCount = ui->crosschainTransactionTableWidget->rowCount();
    for(int i = 0; i < rowCount; i++)
    {
        QString trxId = ui->crosschainTransactionTableWidget->item(i,0)->data(Qt::UserRole).toString();
        QString generatedTrxId = lookupGeneratedTrxByApplyTrxId(trxId);
        QStringList guardAddresses = lookupSignedGuardsByGeneratedTrxId(generatedTrxId);

        QString currentAddress = UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
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

QString WithdrawConfirmPage::lookupGeneratedTrxByApplyTrxId(QString applyTrxId)
{
    QString generatedTrxId;
    QStringList keys = generatedTransactionMap.keys();
    foreach (QString key, keys)
    {
        GeneratedTransaction gt = generatedTransactionMap.value(key);
        if(gt.ccwTrxIds.contains(applyTrxId))
        {
            generatedTrxId = gt.trxId;
            break;
        }
    }

    return generatedTrxId;
}

QStringList WithdrawConfirmPage::lookupSignedGuardsByGeneratedTrxId(QString generatedTrxId)
{
    QStringList result;
    QStringList keys = signTransactionMap.keys();

    foreach (QString key, keys)
    {
        SignTransaction st = signTransactionMap.value(key);
        if(st.generatedTrxId == generatedTrxId)
        {
            result += st.guardAddress;
        }
    }

    return result;
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
    fetchCrosschainTransactions();
}

void WithdrawConfirmPage::on_typeCurrentBtn_clicked()
{
    currentType = 1;
    ui->typeCurrentBtn->setChecked(true);
    ui->typeWaitingBtn->setChecked(false);
    showCrosschainTransactions();
}

void WithdrawConfirmPage::on_typeWaitingBtn_clicked()
{
    currentType = 0;
    ui->typeCurrentBtn->setChecked(false);
    ui->typeWaitingBtn->setChecked(true);
    showCrosschainTransactions();
}
