#include "transferrecordwidget.h"
#include "ui_transferrecordwidget.h"

#include "wallet.h"

TransferRecordWidget::TransferRecordWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransferRecordWidget)
{
    ui->setupUi(this);
}

TransferRecordWidget::~TransferRecordWidget()
{
    delete ui;
}

void TransferRecordWidget::showTransferRecord(QString _accountAddress)
{
    accountAddress = _accountAddress;
    TransactionTypeIds typeIds = UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(_accountAddress,0);

    int size = typeIds.size();
    ui->transferRecordTableWidget->setRowCount(0);
    ui->transferRecordTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        QString transactionId = typeIds.at(i);
        TransactionStruct ts = UBChain::getInstance()->transactionDB.getTransactionStruct(transactionId);
        if(ts.type == -1)
        {
            qDebug() << "can not find transaction in transactionstruct db: " << transactionId;
            continue;
        }

        ui->transferRecordTableWidget->setItem(i,0, new QTableWidgetItem(QString::number(ts.blockNum)));

        QJsonObject object = QJsonDocument::fromJson(ts.operationStr.toLatin1()).object();
        QString fromAddress = object.take("from_addr").toString();
        QString toAddress   = object.take("to_addr").toString();
        if(fromAddress == _accountAddress)
        {

        }
    }
}
