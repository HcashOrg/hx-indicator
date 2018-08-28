#include "ContractTokenPage.h"
#include "ui_ContractTokenPage.h"

#include "wallet.h"
#include "CreateTokenDialog.h"
#include "AddTokenDialog.h"
#include "ToolButtonWidget.h"
#include "showcontentdialog.h"
#include "TokenTransferWidget.h"
#include "TokenHistoryWidget.h"

ContractTokenPage::ContractTokenPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContractTokenPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->tokenTableWidget->installEventFilter(this);
    ui->tokenTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tokenTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tokenTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->tokenTableWidget->setFrameShape(QFrame::NoFrame);
    ui->tokenTableWidget->setMouseTracking(true);
    ui->tokenTableWidget->setShowGrid(false);//隐藏表格线

    ui->tokenTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->tokenTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->tokenTableWidget->horizontalHeader()->setVisible(true);
    ui->tokenTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->tokenTableWidget->setColumnWidth(0,140);
    ui->tokenTableWidget->setColumnWidth(1,260);
    ui->tokenTableWidget->setColumnWidth(2,160);
    ui->tokenTableWidget->setColumnWidth(3,80);
    ui->tokenTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->addTokenBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->createTokenBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->historyBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

    init();
}

ContractTokenPage::~ContractTokenPage()
{
    delete ui;
}

void ContractTokenPage::init()
{
    inited = false;
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

    inited = true;
    fetchTokensInfo();
}

void ContractTokenPage::refresh()
{
    fetchTokensBalance();
}

void ContractTokenPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void ContractTokenPage::fetchTokensInfo()
{
    HXChain::getInstance()->configFile->beginGroup("/contractTokens");
    QStringList contractIds = HXChain::getInstance()->configFile->childKeys();
    HXChain::getInstance()->configFile->endGroup();

    foreach (QString contractId, contractIds)
    {
        HXChain::getInstance()->postRPC( "ContractTokenPage-invoke_contract_offline-tokenSymbol-" + contractId, toJsonFormat( "invoke_contract_offline",
                                                                               QJsonArray() << ui->accountComboBox->currentText() << contractId
                                                                               << "tokenSymbol"  << ""));

        HXChain::getInstance()->postRPC( "ContractTokenPage-invoke_contract_offline-precision-" + contractId, toJsonFormat( "invoke_contract_offline",
                                                                               QJsonArray() << ui->accountComboBox->currentText() << contractId
                                                                               << "precision"  << ""));
        HXChain::getInstance()->postRPC( "ContractTokenPage-get_contract_info-" + contractId, toJsonFormat( "get_contract_info",
                                                                               QJsonArray() << contractId ));
    }

    HXChain::getInstance()->postRPC( "ContractTokenPage-fetchTokensInfo-finish", toJsonFormat( "fetchTokensInfo-finish",
                                                                           QJsonArray()));
}

void ContractTokenPage::fetchTokensBalance()
{
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();

    QStringList contractIds = contractTokenInfoMap.keys();

    foreach (QString contractId, contractIds)
    {
        foreach (QString account, accounts)
        {
            QString accountAddress = HXChain::getInstance()->accountInfoMap.value(account).address;

            HXChain::getInstance()->postRPC( "ContractTokenPage-invoke_contract_offline-balanceOf-" + contractId + "###" + account,
                                             toJsonFormat( "invoke_contract_offline",
                                                           QJsonArray() << account << contractId
                                                           << "balanceOf"  << accountAddress));

        }
    }

    HXChain::getInstance()->postRPC( "ContractTokenPage-invoke_contract_offline-fetchTokensBalance-finish",
                                     toJsonFormat( "fetchTokensBalance-finish", QJsonArray() ));
}

void ContractTokenPage::showAccountTokens()
{
    ui->tokenTableWidget->setRowCount(0);

    QStringList contractIds = contractTokenInfoMap.keys();
    int size = contractIds.size();
    ui->tokenTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        QString contractId = contractIds.at(i);
        ContractTokenInfo tokenInfo = contractTokenInfoMap.value(contractId);
        ui->tokenTableWidget->setItem( i, 0, new QTableWidgetItem(tokenInfo.symbol));
        ui->tokenTableWidget->setItem( i, 1, new QTableWidgetItem(contractId));

        QMap<QString,TokenBalance> balanceMap = accountContractTokenBalanceMap.value(ui->accountComboBox->currentText());
        QString amount = balanceMap.value(contractId).amount;
        ui->tokenTableWidget->setItem( i, 2, new QTableWidgetItem(QString::number(amount.toDouble() / tokenInfo.precision.toDouble(), 'g', 12)));

        ui->tokenTableWidget->setItem(i,3,new QTableWidgetItem(tr("transfer")));

        ToolButtonWidget *toolButton = new ToolButtonWidget();
        toolButton->setText(ui->tokenTableWidget->item(i,3)->text());
        ui->tokenTableWidget->setCellWidget(i,3,toolButton);
        connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ContractTokenPage::on_tokenTableWidget_cellClicked,this,i,3));
    }

    tableWidgetSetItemZebraColor(ui->tokenTableWidget);

}


void ContractTokenPage::on_createTokenBtn_clicked()
{
    CreateTokenDialog createTokenDialog;
    createTokenDialog.pop();

    if(createTokenDialog.newTokenAdded)
    {
        QTimer::singleShot(5000, this, &ContractTokenPage::fetchTokensInfo);
    }
}

void ContractTokenPage::jsonDataUpdated(QString id)
{
    if( id.startsWith("ContractTokenPage-invoke_contract_offline-tokenSymbol-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        QString contractId = id.mid( QString("ContractTokenPage-invoke_contract_offline-tokenSymbol-").size());
//        qDebug() << id << result;

        if(result.startsWith(QString("\"result\":")))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject object = parse_doucment.object();
            QString symbol = object.value("result").toString();

            contractTokenInfoMap[contractId].contractId = contractId;
            contractTokenInfoMap[contractId].symbol = symbol;
        }
        return;
    }

    if( id.startsWith("ContractTokenPage-invoke_contract_offline-precision-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        QString contractId = id.mid( QString("ContractTokenPage-invoke_contract_offline-precision-").size());
//        qDebug() << id << result;

        if(result.startsWith(QString("\"result\":")))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject object = parse_doucment.object();
            QString precision = object.value("result").toString();

            contractTokenInfoMap[contractId].precision = precision;
        }
        return;
    }

    if( id.startsWith("ContractTokenPage-get_contract_info-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        QString contractId = id.mid( QString("ContractTokenPage-get_contract_info-").size());

        if(result.startsWith(QString("\"result\":")))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject object = parse_doucment.object().value("result").toObject();
            contractTokenInfoMap[contractId].registeredBlock = object.value("registered_block").toInt();
            contractTokenInfoMap[contractId].ownerAddress = object.value("owner_address").toString();
        }
        return;
    }

    if( id.startsWith("ContractTokenPage-invoke_contract_offline-balanceOf-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        QString str = id.mid( QString("ContractTokenPage-invoke_contract_offline-balanceOf-").size());
        QString contractId = str.split("###").at(0);
        QString accountName = str.split("###").at(1);

        if(result.startsWith(QString("\"result\":")))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject object = parse_doucment.object();
            QString amount = object.value("result").toString();

            accountContractTokenBalanceMap[accountName][contractId].contractId = contractId;
            accountContractTokenBalanceMap[accountName][contractId].amount = amount;
        }
        return;
    }


    if( id == "ContractTokenPage-fetchTokensInfo-finish")
    {
        fetchTokensBalance();

        return;
    }

    if( id == "ContractTokenPage-invoke_contract_offline-fetchTokensBalance-finish")
    {
        showAccountTokens();

        return;
    }

}

void ContractTokenPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;
    HXChain::getInstance()->currentAccount = ui->accountComboBox->currentText();

    showAccountTokens();
}

void ContractTokenPage::on_addTokenBtn_clicked()
{
    AddTokenDialog addTokenDialog;
    addTokenDialog.pop();

    if(addTokenDialog.newTokenAdded)
    {
        QTimer::singleShot(5000, this, &ContractTokenPage::fetchTokensInfo);
    }
}

void ContractTokenPage::on_tokenTableWidget_cellClicked(int row, int column)
{
    if( column == 1)
    {
        if( !ui->tokenTableWidget->item(row, column))    return;

        ShowContentDialog showContentDialog( ui->tokenTableWidget->item(row, column)->text(),this);

        int x = ui->tokenTableWidget->columnViewportPosition(column) + ui->tokenTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->tokenTableWidget->rowViewportPosition(row) - 10 + ui->tokenTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->tokenTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if(column == 3)
    {
        if( !ui->tokenTableWidget->item(row, 0))    return;

        emit backBtnVisible(true);
        TokenTransferWidget* tokenTransferWidget = new TokenTransferWidget( ui->accountComboBox->currentText(),
                                                                            ui->tokenTableWidget->item(row,0)->text(), this);
        tokenTransferWidget->setAttribute(Qt::WA_DeleteOnClose);
        tokenTransferWidget->show();
        tokenTransferWidget->raise();
        return;
    }
}

void ContractTokenPage::on_historyBtn_clicked()
{
    emit backBtnVisible(true);
    TokenHistoryWidget* tokenHistoryWidget = new TokenHistoryWidget(this);
    tokenHistoryWidget->setAttribute(Qt::WA_DeleteOnClose);
    tokenHistoryWidget->show();
    tokenHistoryWidget->raise();
    tokenHistoryWidget->setAccount(ui->accountComboBox->currentText());
}
