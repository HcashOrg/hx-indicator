#include "TokenHistoryWidget.h"
#include "ui_TokenHistoryWidget.h"

#include "wallet.h"
#include "showcontentdialog.h"

TokenHistoryWidget::TokenHistoryWidget(ContractTokenPage *parent) :
    QWidget(parent),
    page(parent),
    ui(new Ui::TokenHistoryWidget)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);

    ui->tokenRecordTableWidget->installEventFilter(this);
    ui->tokenRecordTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tokenRecordTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tokenRecordTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->tokenRecordTableWidget->setFrameShape(QFrame::NoFrame);
    ui->tokenRecordTableWidget->setMouseTracking(true);
    ui->tokenRecordTableWidget->setShowGrid(false);//隐藏表格线

    ui->tokenRecordTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->tokenRecordTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->tokenRecordTableWidget->horizontalHeader()->setVisible(true);
    ui->tokenRecordTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->tokenRecordTableWidget->setColumnWidth(0,120);
    ui->tokenRecordTableWidget->setColumnWidth(1,140);
    ui->tokenRecordTableWidget->setColumnWidth(2,120);
    ui->tokenRecordTableWidget->setColumnWidth(3,140);
    ui->tokenRecordTableWidget->setColumnWidth(4,120);
    ui->tokenRecordTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    init();
}

TokenHistoryWidget::~TokenHistoryWidget()
{
    delete ui;
}

void TokenHistoryWidget::init()
{
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

    QStringList contractIds = page->contractTokenInfoMap.keys();
    foreach (QString contractId, contractIds)
    {
        QString symbol = page->contractTokenInfoMap.value(contractId).symbol;
        ui->tokenComboBox->addItem(symbol, contractId);
    }

    fetchContractEvents();
}

void TokenHistoryWidget::setAccount(QString _accountName)
{
    ui->accountComboBox->setCurrentText(_accountName);
}

void TokenHistoryWidget::setToken(QString _tokenSymbol)
{
    ui->tokenComboBox->setCurrentText(_tokenSymbol);
}

void TokenHistoryWidget::jsonDataUpdated(QString id)
{
    if( id.startsWith("TokenHistoryWidget-get_contract_events-") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        QString contractId = id.mid(QString("TokenHistoryWidget-get_contract_events-").size());
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject object = parse_doucment.object();
            QJsonArray resultArray   = object.value("result").toArray();

            QVector<ContractEvent> vector;
            foreach (QJsonValue v, resultArray)
            {
                QJsonObject eventObject = v.toObject();
                ContractEvent event;
                event.eventId = eventObject.value("id").toString();
                event.contractAddress = eventObject.value("contract_address").toString();
                event.eventName = eventObject.value("event_name").toString();
                event.eventArg = eventObject.value("event_arg").toString();
                event.trxId = eventObject.value("trx_id").toString();
                event.block = eventObject.value("block_num").toInt();
                event.opNum = eventObject.value("op_num").toInt();
                vector.append(event);

            }
            contractEvents.insert(contractId, vector);

            if(contractId == ui->tokenComboBox->currentData(Qt::UserRole).toString())
            {
                showContractEvents();
            }
        }
        return;
    }
}

void TokenHistoryWidget::fetchContractEvents()
{
    QStringList contractIds = page->contractTokenInfoMap.keys();
    foreach (QString contractId, contractIds)
    {
        HXChain::getInstance()->postRPC( "TokenHistoryWidget-get_contract_events-" + contractId, toJsonFormat( "get_contract_events",
                                                                                                               QJsonArray() << contractId));
    }
}

void TokenHistoryWidget::showContractEvents()
{
    QString contractId = ui->tokenComboBox->currentData(Qt::UserRole).toString();
    ContractTokenInfo tokenInfo = page->contractTokenInfoMap.value(contractId);
    QString accountAddress = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
    QVector<ContractEvent>  vector = filterAccountContractEvents(accountAddress,contractId);
    int size = vector.size();
    ui->tokenRecordTableWidget->setRowCount(0);
    ui->tokenRecordTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ContractEvent event = vector.at(i);
        ui->tokenRecordTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(event.block)));
        ui->tokenRecordTableWidget->setItem(i, 3, new QTableWidgetItem(event.trxId));

        if(event.eventName == "Inited")
        {
            ui->tokenRecordTableWidget->setItem(i, 1, new QTableWidgetItem("-"));
            ui->tokenRecordTableWidget->setItem(i, 2, new QTableWidgetItem("+" + event.eventArg));
            ui->tokenRecordTableWidget->item(i,2)->setTextColor(QColor(0,170,0));
            ui->tokenRecordTableWidget->setItem(i, 4, new QTableWidgetItem(tr("initial allocation")));
        }
        else if(event.eventName == "Transfer")
        {
            QJsonDocument parse_doucment = QJsonDocument::fromJson(event.eventArg.toLatin1());
            QJsonObject argObject = parse_doucment.object();
            QString toAddress = argObject.value("to").toString();
            QString fromAddress = argObject.value("from").toString();
            unsigned long long amount = jsonValueToULL( argObject.value("amount"));

            if(toAddress == accountAddress)
            {
                if(fromAddress == accountAddress)
                {
                    // 如果是自己转自己
                    ui->tokenRecordTableWidget->setItem(i, 1, new QTableWidgetItem(accountAddress));
                    ui->tokenRecordTableWidget->setItem(i, 2, new QTableWidgetItem(getBigNumberString(amount, tokenInfo.precision.size() - 1) ));
                    ui->tokenRecordTableWidget->item(i,2)->setTextColor(QColor(202,135,0));
                    ui->tokenRecordTableWidget->setItem(i, 4, new QTableWidgetItem(tr("transfer to self")));
                }
                else
                {
                    // 转入
                    ui->tokenRecordTableWidget->setItem(i, 1, new QTableWidgetItem(fromAddress));
                    ui->tokenRecordTableWidget->setItem(i, 2, new QTableWidgetItem( "+" + getBigNumberString(amount, tokenInfo.precision.size() - 1) ));
                    ui->tokenRecordTableWidget->item(i,2)->setTextColor(QColor(0,170,0));
                    ui->tokenRecordTableWidget->setItem(i, 4, new QTableWidgetItem(tr("transfer-in")));
                }
            }
            else
            {
                // 转出
                ui->tokenRecordTableWidget->setItem(i, 1, new QTableWidgetItem(toAddress));
                ui->tokenRecordTableWidget->setItem(i, 2, new QTableWidgetItem( "-" + getBigNumberString(amount, tokenInfo.precision.size() - 1) ));
                ui->tokenRecordTableWidget->item(i,2)->setTextColor(QColor(255,0,0));
                ui->tokenRecordTableWidget->setItem(i, 4, new QTableWidgetItem(tr("transfer-out")));
            }
        }
    }

    tableWidgetSetItemZebraColor(ui->tokenRecordTableWidget);
}

QVector<ContractEvent> TokenHistoryWidget::filterAccountContractEvents(QString accountAddress, QString contractId)
{
    QVector<ContractEvent> vector = contractEvents.value(contractId);
    QVector<ContractEvent> result;
    foreach (ContractEvent event, vector)
    {
        if(event.eventName == "Inited")
        {
            ContractTokenInfo tokenInfo = page->contractTokenInfoMap.value(contractId);
            if(tokenInfo.ownerAddress == accountAddress)
            {
                result.append(event);
            }
        }
        else if(event.eventName == "Transfer")
        {
            QJsonDocument parse_doucment = QJsonDocument::fromJson(event.eventArg.toLatin1());
            QJsonObject argObject = parse_doucment.object();
            QString toAddress = argObject.value("to").toString();
            QString fromAddress = argObject.value("from").toString();
            if(toAddress == accountAddress || fromAddress == accountAddress)
            {
                result.append(event);
            }
        }
    }

    return result;
}

void TokenHistoryWidget::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    showContractEvents();
}

void TokenHistoryWidget::on_tokenComboBox_currentIndexChanged(const QString &arg1)
{
    showContractEvents();
}

void TokenHistoryWidget::on_tokenRecordTableWidget_cellClicked(int row, int column)
{
    if( column == 1 || column == 3 || column == 4)
    {
        if( !ui->tokenRecordTableWidget->item(row, column))    return;

        ShowContentDialog showContentDialog( ui->tokenRecordTableWidget->item(row, column)->text(),this);

        int x = ui->tokenRecordTableWidget->columnViewportPosition(column) + ui->tokenRecordTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->tokenRecordTableWidget->rowViewportPosition(row) - 10 + ui->tokenRecordTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->tokenRecordTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}
