#include "ColdHotTransferPage.h"
#include "ui_ColdHotTransferPage.h"

#include "wallet.h"
#include "ColdHotTransferDialog.h"
#include "ToolButtonWidget.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"

ColdHotTransferPage::ColdHotTransferPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColdHotTransferPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->coldHotTransactionTableWidget->installEventFilter(this);
    ui->coldHotTransactionTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->coldHotTransactionTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->coldHotTransactionTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->coldHotTransactionTableWidget->setMouseTracking(true);
    ui->coldHotTransactionTableWidget->setShowGrid(false);//隐藏表格线

    ui->coldHotTransactionTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->coldHotTransactionTableWidget->horizontalHeader()->setVisible(true);
    ui->coldHotTransactionTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->coldHotTransactionTableWidget->setColumnWidth(0,120);
    ui->coldHotTransactionTableWidget->setColumnWidth(1,120);
    ui->coldHotTransactionTableWidget->setColumnWidth(2,100);
    ui->coldHotTransactionTableWidget->setColumnWidth(3,100);
    ui->coldHotTransactionTableWidget->setColumnWidth(4,80);
    ui->coldHotTransactionTableWidget->setColumnWidth(5,80);
    ui->coldHotTransactionTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    init();
}

ColdHotTransferPage::~ColdHotTransferPage()
{
    delete ui;
}

void ColdHotTransferPage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = UBChain::getInstance()->getMyFormalGuards();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    fetchColdHotTransaction();
}

void ColdHotTransferPage::jsonDataUpdated(QString id)
{
    if( id.startsWith("id-get_coldhot_transaction-") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        int type = id.mid(QString("id-get_coldhot_transaction-").size()).toInt();

        if(result.startsWith("\"result\":"))
        {
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

                    coldHotTransactionMap.insert(cht.trxId, cht);
                }
            }


            QStringList keys = coldHotTransactionMap.keys();
            int size = keys.size();
            ui->coldHotTransactionTableWidget->setRowCount(0);
            ui->coldHotTransactionTableWidget->setRowCount(size);

            for(int i = 0; i < size; i++)
            {
                ui->coldHotTransactionTableWidget->setRowHeight(i,40);

                ColdHotTransaction cht = coldHotTransactionMap.value(keys.at(i));

                QDateTime time = QDateTime::fromString(cht.expirationTime, "yyyy-MM-ddThh:mm:ss");
                time = time.addSecs(-600);       // 时间减10分钟
                QString currentDateTime = time.toString("yyyy-MM-dd hh:mm:ss");
                ui->coldHotTransactionTableWidget->setItem(i, 0, new QTableWidgetItem(currentDateTime));
                ui->coldHotTransactionTableWidget->item(i,0)->setData(Qt::UserRole, cht.trxId);

                ui->coldHotTransactionTableWidget->setItem(i, 1, new QTableWidgetItem(cht.amount + " " + cht.assetSymbol));

                ui->coldHotTransactionTableWidget->setItem(i, 2, new QTableWidgetItem(cht.withdrawAddress));

                ui->coldHotTransactionTableWidget->setItem(i, 3, new QTableWidgetItem(cht.depositAddress));

                ui->coldHotTransactionTableWidget->setItem(i, 5, new QTableWidgetItem(tr("sign")));
                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->coldHotTransactionTableWidget->item(i,5)->text());
    //            toolButton->setBackgroundColor(itemColor);
                ui->coldHotTransactionTableWidget->setCellWidget(i,5,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ColdHotTransferPage::on_coldHotTransactionTableWidget_cellClicked,this,i,5));


                for (int j : {0,1,2,3})
                {
                    if(i%2)
                    {
                        ui->coldHotTransactionTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                        ui->coldHotTransactionTableWidget->item(i,j)->setBackgroundColor(QColor(252,253,255));
                    }
                    else
                    {
                        ui->coldHotTransactionTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                        ui->coldHotTransactionTableWidget->item(i,j)->setBackgroundColor(QColor("white"));
                    }
                }
            }
        }

        return;
    }

    if(id == "id-guard_sign_coldhot_transaction")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            close();

            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of guard-sign-coldhot has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
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

void ColdHotTransferPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}

void ColdHotTransferPage::on_transferBtn_clicked()
{
    ColdHotTransferDialog coldHotTransferDialog;
    coldHotTransferDialog.pop();
}

void ColdHotTransferPage::fetchColdHotTransaction(int type)
{
    UBChain::getInstance()->postRPC( "id-get_coldhot_transaction-" + QString::number(type), toJsonFormat( "get_coldhot_transaction",
                                                                                     QJsonArray() << type));

}

void ColdHotTransferPage::on_coldHotTransactionTableWidget_cellClicked(int row, int column)
{
    if(column == 5)
    {
        if(ui->coldHotTransactionTableWidget->item(row,0))
        {
            QString trxId = ui->coldHotTransactionTableWidget->item(row,0)->data(Qt::UserRole).toString();

            QString chcTrxId;
            QStringList keys = coldHotCrosschainTransactionMap.keys();
            foreach (QString key, keys)
            {
                ColdHotCrosschainTransaction chct = coldHotCrosschainTransactionMap.value(key);
                if(chct.coldHotTrxId == trxId)
                {
                    chcTrxId = chct.trxId;
                    break;
                }
            }
qDebug() << "ccccccccccc " << trxId << chcTrxId;
            if(!chcTrxId.isEmpty() && !ui->accountComboBox->currentText().isEmpty())
            {
                UBChain::getInstance()->postRPC( "id-guard_sign_coldhot_transaction", toJsonFormat( "guard_sign_coldhot_transaction",
                                                 QJsonArray() << chcTrxId << ui->accountComboBox->currentText()));

            }
        }

        return;
    }
}
