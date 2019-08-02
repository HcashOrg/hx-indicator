#include "NameTransferPage.h"
#include "ui_NameTransferPage.h"

#include "wallet.h"
#include "NameTransferDialog.h"
#include "showcontentdialog.h"
#include "ShowTrxCodeDialog.h"
#include "transfer/transferconfirmdialog.h"
#include "FeeChooseWidget.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"

NameTransferPage::NameTransferPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NameTransferPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->trxTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->trxTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->trxTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->trxTableWidget->setMouseTracking(true);
    ui->trxTableWidget->setShowGrid(false);//隐藏表格线

    ui->trxTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->trxTableWidget->horizontalHeader()->setVisible(true);
    ui->trxTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->trxTableWidget->setColumnWidth(0,150);
    ui->trxTableWidget->setColumnWidth(1,150);
    ui->trxTableWidget->setColumnWidth(2,70);
    ui->trxTableWidget->setColumnWidth(3,70);
    ui->trxTableWidget->setColumnWidth(4,70);
    ui->trxTableWidget->setColumnWidth(5,80);
    ui->trxTableWidget->setColumnWidth(6,50);
    ui->trxTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->typeTrxBtn->setCheckable(true);
    ui->typeSignBtn->setCheckable(true);
    ui->typeTrxBtn->adjustSize();
    ui->typeTrxBtn->resize(ui->typeTrxBtn->width(), 18);
    ui->typeSignBtn->adjustSize();
    ui->typeSignBtn->resize(ui->typeSignBtn->width(), 18);
    ui->typeSignBtn->move(ui->typeTrxBtn->x() + ui->typeTrxBtn->width() + 30, ui->typeSignBtn->y());

    setStyleSheet(PUSHBUTTON_CHECK_STYLE);

    showNameTransferTrxs();

    on_typeTrxBtn_clicked();

    ui->widget->hide();
    ui->signBtn->setEnabled(false);

    ui->feeLabel->setText(QString::number(HXChain::getInstance()->feeChargeInfo.transferFee.toDouble(),'f',ASSET_PRECISION) + " " + ASSET_NAME);
}

NameTransferPage::~NameTransferPage()
{
    delete ui;
}

void NameTransferPage::jsonDataUpdated(QString id)
{
    if( id == "NameTransferPage-decode_multisig_transaction-" + ui->trxCodeLineEdit->text())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\"") && !result.startsWith("\"result\":{\"ref_block_num\":0"))
        {
            result.prepend("{");
            result.append("}");
            QJsonObject ob = QJsonDocument::fromJson(result.toUtf8()).object();
            QJsonObject trxObject = ob.value("result").toObject();

            QString expiration = trxObject.value("expiration").toString();
            QJsonArray opsArray = trxObject.value("operations").toArray();
            QJsonObject opsObject = opsArray.at(0).toArray().at(1).toObject();
            QJsonObject makerOpObject = opsObject.value("maker_op").toArray().at(0).toObject().value("op").toArray().at(1).toObject();
            QString from = makerOpObject.value("from").toString();
            QString to = makerOpObject.value("to").toString();
            QString newName = makerOpObject.value("newname").toString();
            QJsonObject feeObject = makerOpObject.value("fee").toObject();
            unsigned long long feeAmount = jsonValueToULL(feeObject.value("amount"));

            QJsonObject takerOpObject = opsObject.value("taker_op").toArray().at(0).toObject().value("op").toArray().at(1).toObject();
            QJsonObject amountObject = takerOpObject.value("amount").toObject();
            unsigned long long amount = jsonValueToULL(amountObject.value("amount"));
            QString amountAssetId = amountObject.value("asset_id").toString();
            AssetInfo amountAssetInfo = HXChain::getInstance()->assetInfoMap.value(amountAssetId);

            ui->fromAddressLabel->setText(from);
            ui->sendtoLabel->setText(to);
            ui->amountLabel->setText(getBigNumberString(amount,amountAssetInfo.precision) + " " + amountAssetInfo.symbol);
            ui->expirationLabel->setText(toLocalTime(expiration));


            ui->widget->show();

            if(HXChain::getInstance()->isMyAddress(to))
            {
                ui->signBtn->setEnabled(true);
            }
            else
            {
                ui->signBtn->setEnabled(false);
            }

            HXChain::getInstance()->postRPC( "NameTransferPage-get_account_by_addr-" + ui->trxCodeLineEdit->text(), toJsonFormat( "get_account_by_addr", QJsonArray() << from));
        }
        else
        {
            ui->widget->hide();
            ui->signBtn->setEnabled(false);
        }

        return;
    }

    if( id == "NameTransferPage-get_account_by_addr-" + ui->trxCodeLineEdit->text())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        result.prepend("{");
        result.append("}");
        QJsonObject resultObject = QJsonDocument::fromJson(result.toUtf8()).object().value("result").toObject();
        QString name = resultObject.value("name").toString();
        ui->fromLabel->setText(name);

        return;
    }

    if( id == "NameTransferPage-confirm_name_transfer")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":{"))             // 成功
        {
            HXChain::getInstance()->postRPC( "NameTransferPage-info", toJsonFormat( "info", QJsonArray()));
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

    if( id == "NameTransferPage-info")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        result.prepend("{");
        result.append("}");
        QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
        QString maintenanceTime = object.value("result").toObject().value("next_maintenance_time").toString();
        maintenanceTime.remove(" in the future");

        TransactionResultDialog transactionResultDialog;
        transactionResultDialog.setInfoText(tr("The account name change will take effect %1 in the future!").arg(maintenanceTime));
        transactionResultDialog.setDetailText(result);
        transactionResultDialog.pop();

        return;
    }

    if( id.startsWith("NameTransferPage-get_transaction-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        int row = id.mid(QString("NameTransferPage-get_transaction-").size()).toInt();

        if(result == "\"result\":null")
        {
            if(ui->trxTableWidget->item(row,5) && ui->trxTableWidget->item(row,4))
            {
                QString trxCode = ui->trxTableWidget->item(row,4)->data(Qt::UserRole).toString();
                QString trxStr = HXChain::getInstance()->transactionDB.getNameTransferTrx(trxCode).at(0);
                QJsonObject object = QJsonDocument::fromJson(trxStr.toUtf8()).object();
                QString expirationTime = object.value("expiration").toString();
                QDateTime dateTime = QDateTime::fromString(expirationTime,"yyyy-MM-ddThh:mm:ss");
                dateTime.setTimeSpec(Qt::UTC);
                QDateTime localTime = dateTime.toLocalTime();
                if(dateTime <= QDateTime::currentDateTime())
                {
                    ui->trxTableWidget->item(row,5)->setText(tr("expired"));
                }
                else
                {
                    ui->trxTableWidget->item(row,5)->setText(tr("unconfirmed"));
                }
            }
        }
        else
        {
            if(ui->trxTableWidget->item(row,5))
            {
                ui->trxTableWidget->item(row,5)->setText(tr("confirmed"));
            }
        }
    }
}

void NameTransferPage::on_typeTrxBtn_clicked()
{
    ui->typeTrxBtn->setChecked(true);
    ui->typeSignBtn->setChecked(false);

    ui->stackedWidget->setCurrentIndex(0);
}

void NameTransferPage::on_typeSignBtn_clicked()
{
    ui->typeTrxBtn->setChecked(false);
    ui->typeSignBtn->setChecked(true);

    ui->stackedWidget->setCurrentIndex(1);
}

void NameTransferPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));
    painter.drawRect(rect());

    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(243,241,250),Qt::SolidPattern));
    painter.drawRect(0,87,this->width(),24);
}

void NameTransferPage::on_createTrxBtn_clicked()
{
    NameTransferDialog nameTransferDialog;
    nameTransferDialog.pop();
    showNameTransferTrxs();
}

void NameTransferPage::showNameTransferTrxs()
{
    QStringList trxCodeList = HXChain::getInstance()->transactionDB.getNameTransferTrxsCode();
    ui->trxTableWidget->setRowCount(0);
    ui->trxTableWidget->setRowCount(trxCodeList.size());
    qDebug() << "cccccccccccc " << trxCodeList;

    for(int i = 0; i < trxCodeList.size(); i++)
    {
        ui->trxTableWidget->setRowHeight(i,40);

        QString trxCode = trxCodeList.at(i);
        QStringList trxStrList = HXChain::getInstance()->transactionDB.getNameTransferTrx(trxCode);

        QJsonObject object = QJsonDocument::fromJson(trxStrList.at(0).toUtf8()).object();

        QJsonArray opsArray = object.value("operations").toArray();
        QJsonObject opsObject = opsArray.at(0).toArray().at(1).toObject();
        QJsonObject makerOpObject = opsObject.value("maker_op").toArray().at(0).toObject().value("op").toArray().at(1).toObject();
        QString from = makerOpObject.value("from").toString();
        QString to = makerOpObject.value("to").toString();
        QString original = makerOpObject.value("original").toString();
        QString newName = makerOpObject.value("newname").toString();
        QJsonObject feeObject = makerOpObject.value("fee").toObject();
        unsigned long long feeAmount = jsonValueToULL(feeObject.value("amount"));

        QJsonObject takerOpObject = opsObject.value("taker_op").toArray().at(0).toObject().value("op").toArray().at(1).toObject();
        QJsonObject amountObject = takerOpObject.value("amount").toObject();
        unsigned long long amount = jsonValueToULL(amountObject.value("amount"));
        QString amountAssetId = amountObject.value("asset_id").toString();
        AssetInfo amountAssetInfo = HXChain::getInstance()->assetInfoMap.value(amountAssetId);

        ui->trxTableWidget->setItem(i,0, new QTableWidgetItem( original + " (" + from + ")"));
        ui->trxTableWidget->setItem(i,1, new QTableWidgetItem(to));
        ui->trxTableWidget->setItem(i,2, new QTableWidgetItem(newName));
        ui->trxTableWidget->setItem(i,3, new QTableWidgetItem( getBigNumberString(amount,amountAssetInfo.precision) + " " + revertERCSymbol(amountAssetInfo.symbol) ));
        ui->trxTableWidget->setItem(i,4, new QTableWidgetItem( trxCode.left(5) + "..."));
        ui->trxTableWidget->item(i,4)->setData(Qt::UserRole, trxCode);
        ui->trxTableWidget->setItem(i,5, new QTableWidgetItem(tr("querying")));
        ui->trxTableWidget->setItem(i,6, new QTableWidgetItem( tr("delete")));

        HXChain::getInstance()->postRPC( "NameTransferPage-get_transaction-" + QString::number(i), toJsonFormat( "get_transaction", QJsonArray() << trxStrList.at(1)));
    }
    tableWidgetSetItemZebraColor(ui->trxTableWidget);

}

void NameTransferPage::on_trxTableWidget_cellClicked(int row, int column)
{
    if(column == 0)
    {
        if( !ui->trxTableWidget->item(row, column))    return;

        ShowContentDialog showContentDialog( ui->trxTableWidget->item(row, column)->text(),this);

        int x = ui->trxTableWidget->columnViewportPosition(column) + ui->trxTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->trxTableWidget->rowViewportPosition(row) - 10 + ui->trxTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->trxTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if(column == 1)
    {
        if( !ui->trxTableWidget->item(row, column))    return;

        ShowContentDialog showContentDialog( ui->trxTableWidget->item(row, column)->text(),this);

        int x = ui->trxTableWidget->columnViewportPosition(column) + ui->trxTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->trxTableWidget->rowViewportPosition(row) - 10 + ui->trxTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->trxTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if(column == 4)
    {
        if( !ui->trxTableWidget->item(row, column))    return;

        ShowTrxCodeDialog showTrxCodeDialog( ui->trxTableWidget->item(row, column)->data(Qt::UserRole).toString(),this);

        int x = ui->trxTableWidget->columnViewportPosition(column) + ui->trxTableWidget->columnWidth(column) / 2
                - showTrxCodeDialog.width() / 2;
        int y = ui->trxTableWidget->rowViewportPosition(row) - 10 + ui->trxTableWidget->horizontalHeader()->height();

        showTrxCodeDialog.move( ui->trxTableWidget->mapToGlobal( QPoint(x, y)));
        showTrxCodeDialog.exec();

        return;
    }

    if(column == 6)
    {
        if( !ui->trxTableWidget->item(row, 4))    return;

        QString trxCode = ui->trxTableWidget->item(row, 4)->data(Qt::UserRole).toString();
        HXChain::getInstance()->transactionDB.removeNameTransferTrx(trxCode);
        showNameTransferTrxs();

        return;
    }
}



void NameTransferPage::on_trxCodeLineEdit_textChanged(const QString &arg1)
{
    HXChain::getInstance()->postRPC( "NameTransferPage-decode_multisig_transaction-" + arg1, toJsonFormat( "decode_multisig_transaction", QJsonArray() << arg1));
}



void NameTransferPage::on_signBtn_clicked()
{
    QStringList strList = ui->amountLabel->text().split(" ");
    QString amountStr = strList.size() > 1 ? strList.at(0) : "";
    QString symbol = strList.size() > 1 ? strList.at(1) : "";

    TransferConfirmDialog transferConfirmDialog( ui->fromAddressLabel->text(), amountStr,
                                                 getRealAssetSymbol( symbol),
                                                 QString::number(HXChain::getInstance()->feeChargeInfo.transferFee.toDouble(),'f',ASSET_PRECISION), ASSET_NAME, "");
    bool yOrN = transferConfirmDialog.pop();
    if(yOrN)
    {
        HXChain::getInstance()->postRPC( "NameTransferPage-confirm_name_transfer", toJsonFormat( "confirm_name_transfer",
                                                                                                 QJsonArray() << HXChain::getInstance()->addressToName(ui->sendtoLabel->text())
                                                                                                 << ui->trxCodeLineEdit->text() << true));

    }
}
