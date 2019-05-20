#include "MultiSigPage.h"
#include "ui_MultiSigPage.h"

#include <QClipboard>

#include "CreateMultisigWidget.h"
#include "ToolButtonWidget.h"
#include "control/AssetIconItem.h"
#include "MultiSigTransferWidget.h"
#include "commondialog.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "AddMultiSigAddressDialog.h"
#include "showcontentdialog.h"

MultiSigPage::MultiSigPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiSigPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->balanceTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->balanceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->balanceTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->balanceTableWidget->setFrameShape(QFrame::NoFrame);
    ui->balanceTableWidget->setMouseTracking(true);
    ui->balanceTableWidget->setShowGrid(false);//隐藏表格线

    ui->balanceTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->balanceTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->balanceTableWidget->horizontalHeader()->setVisible(true);
    ui->balanceTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->balanceTableWidget->setColumnWidth(0,140);
    ui->balanceTableWidget->setColumnWidth(1,300);
    ui->balanceTableWidget->setColumnWidth(2,190);
    ui->balanceTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->infoTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->infoTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->infoTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->infoTableWidget->setFrameShape(QFrame::NoFrame);
    ui->infoTableWidget->setMouseTracking(true);
    ui->infoTableWidget->setShowGrid(false);//隐藏表格线

    ui->infoTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->infoTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->infoTableWidget->horizontalHeader()->setVisible(true);
    ui->infoTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->infoTableWidget->setColumnWidth(0,400);
    ui->infoTableWidget->setColumnWidth(1,180);
    ui->infoTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->typeBalanceBtn->setCheckable(true);
    ui->typeInfoBtn->setCheckable(true);
    ui->typeSignBtn->setCheckable(true);
    ui->typeBalanceBtn->adjustSize();
    ui->typeBalanceBtn->resize(ui->typeBalanceBtn->width(), 18);
    ui->typeInfoBtn->adjustSize();
    ui->typeInfoBtn->resize(ui->typeInfoBtn->width(), 18);
    ui->typeInfoBtn->move(ui->typeBalanceBtn->x() + ui->typeBalanceBtn->width() + 30, ui->typeInfoBtn->y());
    ui->typeSignBtn->adjustSize();
    ui->typeSignBtn->resize(ui->typeSignBtn->width(), 18);
    ui->typeSignBtn->move(ui->typeInfoBtn->x() + ui->typeInfoBtn->width() + 30, ui->typeSignBtn->y());

    setStyleSheet(PUSHBUTTON_CHECK_STYLE);
    ui->trxStructTextBrowser->setStyleSheet(TEXTBROWSER_READONLY);
    ui->trxSignedextBrowser->setStyleSheet(TEXTBROWSER_READONLY);

    init();
}

MultiSigPage::~MultiSigPage()
{
    delete ui;
}

void MultiSigPage::fetchMultisigInfo()
{
    HXChain::getInstance()->postRPC( "MultiSigPage-get_multisig_address-" + ui->multiSigAddressComboBox->currentText(),
                                     toJsonFormat( "get_multisig_address", QJsonArray() << ui->multiSigAddressComboBox->currentText() ));

}

void MultiSigPage::showMultiSigInfo()
{
    MultiSigInfo info = multiSigInfoMap.value(ui->multiSigAddressComboBox->currentText());
    ui->requiredLabel->setText(tr("Required / Total   %1 / %2").arg(info.required).arg(info.addresses.size()));

    ui->infoTableWidget->setRowCount(0);
    ui->infoTableWidget->setRowCount(info.addresses.size());

    for(int i = 0; i < info.addresses.size(); i++)
    {
        ui->infoTableWidget->setRowHeight(i,40);

        ui->infoTableWidget->setItem( i, 0, new QTableWidgetItem( info.addresses.at(i)));

        QString accountName = HXChain::getInstance()->addressToName(info.addresses.at(i));
        if(accountName.isEmpty())
        {
            ui->infoTableWidget->setItem( i, 1, new QTableWidgetItem( tr("non local account")));
        }
        else
        {
            ui->infoTableWidget->setItem( i, 1, new QTableWidgetItem( accountName));
        }
    }

    tableWidgetSetItemZebraColor(ui->infoTableWidget);
}


void MultiSigPage::fetchBalances()
{
    HXChain::getInstance()->postRPC( "MultiSigPage-get_addr_balances-" + ui->multiSigAddressComboBox->currentText(),
                                     toJsonFormat( "get_addr_balances", QJsonArray() << ui->multiSigAddressComboBox->currentText() ));
}

void MultiSigPage::showBalances()
{
    AssetAmountMap map = multiSigBalancesMap.value(ui->multiSigAddressComboBox->currentText());
    QStringList assetIds = map.keys();
    int size = assetIds.size();
    ui->balanceTableWidget->setRowCount(0);
    ui->balanceTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->balanceTableWidget->setRowHeight(i,40);

        QString assetId = assetIds.at(i);
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetId);

        //资产名
        QString symbol = assetInfo.symbol;
        ui->balanceTableWidget->setItem(i,0,new QTableWidgetItem(symbol));
        ui->balanceTableWidget->item(i,0)->setData(Qt::UserRole, assetId);

        //数量
        ui->balanceTableWidget->setItem(i,1,new QTableWidgetItem(getBigNumberString(map.value(assetId).amount, assetInfo.precision)));

        ui->balanceTableWidget->setItem(i,2,new QTableWidgetItem(tr("transfer")));

        for (int j : {2})
        {
            if(ui->balanceTableWidget->item(i,j))
            {
                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->balanceTableWidget->item(i,j)->text());
                ui->balanceTableWidget->setCellWidget(i,j,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&MultiSigPage::on_balanceTableWidget_cellClicked,this,i,j));
            }
        }


        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->balanceTableWidget->item(i,0)->text());
        ui->balanceTableWidget->setCellWidget(i, 0, assetIconItem);

    }

    tableWidgetSetItemZebraColor(ui->balanceTableWidget);
}

void MultiSigPage::jsonDataUpdated(QString id)
{
    if( id.startsWith( "MultiSigPage-get_addr_balances-") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        QString address = id.mid(QString("MultiSigPage-get_addr_balances-").size());
        qDebug() << id << result;
        result.prepend("{");
        result.append("}");
        QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
        QJsonArray array = object.value("result").toArray();
        AssetAmountMap map;
        foreach (QJsonValue v, array)
        {
            QJsonObject assetObject = v.toObject();
            AssetAmount aa;
            aa.amount = jsonValueToULL(assetObject.value("amount"));
            aa.assetId = assetObject.value("asset_id").toString();

            map.insert(aa.assetId, aa);
        }

        multiSigBalancesMap.insert(address, map);

        showBalances();
        return;
    }


//    if( id == "MultiSigPage-get_pubkey_from_account-" + ui->accountComboBox->currentText() )
//    {
//        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;
//        result.prepend("{");
//        result.append("}");
//        QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
//        QString pubKey = object.value("result").toString();
//        ui->pubKeyLabel->setText(pubKey);
//        return;
//    }


    if( id == "MultiSigPage-get_multisig_address-" + ui->multiSigAddressComboBox->currentText() )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        result.prepend("{");
        result.append("}");
        QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
        QJsonObject resultObject = object.value("result").toObject();
        MultiSigInfo info;
        info.owner = resultObject.value("owner").toString();

        QJsonArray array = resultObject.value("multisignatures").toArray().at(0).toArray();
        info.required = array.at(0).toInt();

        QJsonArray array2 = array.at(1).toArray();
        foreach (QJsonValue v, array2)
        {
            info.addresses += v.toString();
        }
        multiSigInfoMap.insert(info.owner, info);

        showMultiSigInfo();
        return;
    }

    if( id.startsWith("SignMultiSigTrx+get_multisig_address+"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        result.prepend("{");
        result.append("}");
        QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
        QJsonObject resultObject = object.value("result").toObject();
        QJsonArray array = resultObject.value("multisignatures").toArray().at(0).toArray();

        QJsonArray array2 = array.at(1).toArray();
        QStringList signerAccounts;
        foreach (QJsonValue v, array2)
        {
            QString account = HXChain::getInstance()->addressToName(v.toString());
            if(!account.isEmpty())
            {
                signerAccounts += account;
            }
        }
        if(signerAccounts.size() > 0)
        {
            ui->signerAccountLabel->show();
            ui->accountComboBox->show();
            ui->addressLabel->show();
            ui->noAccountLabel->hide();

            ui->accountComboBox->clear();
            ui->accountComboBox->addItems(signerAccounts);
        }
        else
        {
            ui->signerAccountLabel->hide();
            ui->accountComboBox->hide();
            ui->addressLabel->hide();
            ui->noAccountLabel->show();
        }

        return;
    }

    if( id == "MultiSigPage+decode_multisig_transaction")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");
            QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
            QJsonObject resultObject = object.value("result").toObject();
            ui->trxStructTextBrowser->setText( QJsonDocument(resultObject).toJson());

            QJsonObject operationObject = resultObject.value("operations").toArray().at(0).toArray().at(1).toObject();
            QString fromAddress = operationObject.value("from_addr").toString();
            HXChain::getInstance()->postRPC( "SignMultiSigTrx+get_multisig_address+" + fromAddress,
                                             toJsonFormat( "get_multisig_address", QJsonArray() << fromAddress ));

        }
        else
        {
            ui->trxStructTextBrowser->setText("error: \n" + result);
        }
        return;
    }

    if( id == "MultiSigPage+sign_multisig_trx")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");
            QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
            QString signedCode = object.value("result").toString();
            ui->trxSignedextBrowser->setText( signedCode);
            ui->trxSignedextBrowser->show();
            ui->copyBtn2->show();
            ui->signBtn->hide();
            ui->sendSignatureBtn->show();
        }
        else
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Sign multi-sig trx failed!"));
            commonDialog.pop();
        }
        return;
    }

    if( id == "MultiSigPage+combine_transaction")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result.startsWith("\"result\":"))             // 成功
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Signature of multi-sig trx has been sent out!"));
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

void MultiSigPage::on_createMultiSigBtn_clicked()
{
    emit backBtnVisible(true);

    CreateMultisigWidget* createMultisigWidget = new CreateMultisigWidget( this);
    createMultisigWidget->setAttribute(Qt::WA_DeleteOnClose);
    createMultisigWidget->show();
    createMultisigWidget->raise();
}

void MultiSigPage::init()
{
    ui->multiSigAddressComboBox->clear();
    HXChain::getInstance()->configFile->beginGroup("/multisigAddresses");
    QStringList keys = HXChain::getInstance()->configFile->childKeys();
    HXChain::getInstance()->configFile->endGroup();
    ui->multiSigAddressComboBox->addItems(keys);
    ui->requiredLabel->setVisible(keys.size() > 0);

    ui->typeBalanceBtn->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget2->setCurrentIndex(0);

    ui->signerAccountLabel->hide();
    ui->accountComboBox->hide();
    ui->addressLabel->hide();
    ui->noAccountLabel->hide();
    ui->signBtn->hide();
    ui->trxSignedextBrowser->hide();
    ui->copyBtn2->hide();
    ui->sendSignatureBtn->hide();

    fetchMultisigInfo();
    fetchBalances();
}

void MultiSigPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));
    painter.drawRect(rect());

    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(243,241,250),Qt::SolidPattern));
    painter.drawRect(0,87,this->width(),24);
}

//void MultiSigPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
//{
//    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
//    ui->addressLabel->setText(info.address);

//    HXChain::getInstance()->postRPC( "MultiSigPage-get_pubkey_from_account-" + ui->accountComboBox->currentText(),
//                                     toJsonFormat( "get_pubkey_from_account", QJsonArray() << ui->accountComboBox->currentText() ));
//}

void MultiSigPage::on_balanceTableWidget_cellClicked(int row, int column)
{
    if(column == 2)
    {
        if(ui->balanceTableWidget->item(row,0))
        {
            emit backBtnVisible(true);

            MultiSigTransferWidget* multiSigTransferWidget = new MultiSigTransferWidget(this);
            multiSigTransferWidget->setAttribute(Qt::WA_DeleteOnClose);
            multiSigTransferWidget->show();
            multiSigTransferWidget->raise();
            multiSigTransferWidget->setFromAddress(ui->multiSigAddressComboBox->currentText());
            multiSigTransferWidget->setAsset( getRealAssetSymbol( ui->balanceTableWidget->item(row,0)->text()));
        }

    }
}

void MultiSigPage::on_typeBalanceBtn_clicked()
{
    ui->typeBalanceBtn->setChecked(true);
    ui->typeInfoBtn->setChecked(false);
    ui->typeSignBtn->setChecked(false);

    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget2->setCurrentIndex(0);
}

void MultiSigPage::on_typeInfoBtn_clicked()
{
    ui->typeBalanceBtn->setChecked(false);
    ui->typeInfoBtn->setChecked(true);
    ui->typeSignBtn->setChecked(false);

    ui->stackedWidget->setCurrentIndex(1);
    ui->stackedWidget2->setCurrentIndex(0);
}

void MultiSigPage::on_typeSignBtn_clicked()
{
    ui->typeBalanceBtn->setChecked(false);
    ui->typeInfoBtn->setChecked(false);
    ui->typeSignBtn->setChecked(true);

    ui->stackedWidget2->setCurrentIndex(1);
}

void MultiSigPage::on_multiSigAddressComboBox_currentIndexChanged(const QString &arg1)
{
    fetchBalances();
    fetchMultisigInfo();
}

void MultiSigPage::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->multiSigAddressComboBox->currentText());
}

void MultiSigPage::on_trxCodeTextBrowser_textChanged()
{
    ui->signerAccountLabel->hide();
    ui->accountComboBox->hide();
    ui->addressLabel->hide();
    ui->noAccountLabel->hide();
    ui->trxStructTextBrowser->clear();
    ui->signBtn->hide();
    ui->trxSignedextBrowser->hide();
    ui->copyBtn2->hide();
    ui->sendSignatureBtn->hide();

    if(!ui->trxCodeTextBrowser->toPlainText().isEmpty())
    {
        HXChain::getInstance()->postRPC( "MultiSigPage+decode_multisig_transaction",
                                         toJsonFormat( "decode_multisig_transaction",
                                                       QJsonArray() << ui->trxCodeTextBrowser->toPlainText() ));
    }

}

void MultiSigPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    ui->addressLabel->setText(info.address);
    ui->signBtn->show();
    ui->trxSignedextBrowser->hide();
    ui->copyBtn2->hide();
    ui->sendSignatureBtn->hide();
}

void MultiSigPage::on_signBtn_clicked()
{
    HXChain::getInstance()->postRPC( "MultiSigPage+sign_multisig_trx",
                                     toJsonFormat( "sign_multisig_trx",
                                                   QJsonArray() << ui->addressLabel->text()
                                                   << ui->trxCodeTextBrowser->toPlainText()));
}

void MultiSigPage::on_sendSignatureBtn_clicked()
{
    HXChain::getInstance()->postRPC( "MultiSigPage+combine_transaction",
                                     toJsonFormat( "combine_transaction",
                                                   QJsonArray() <<  (QJsonArray() << ui->trxSignedextBrowser->toPlainText())
                                                   << true));

}

void MultiSigPage::on_copyBtn2_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->trxSignedextBrowser->toPlainText());
}

void MultiSigPage::on_addMultiSigBtn_clicked()
{
    AddMultiSigAddressDialog addMultiSigAddressDialog;
    addMultiSigAddressDialog.pop();

    if(!addMultiSigAddressDialog.multiSigAddress.isEmpty())
    {
        HXChain::getInstance()->configFile->setValue("/multisigAddresses/" + addMultiSigAddressDialog.multiSigAddress, 1);
        init();

#ifndef LIGHT_MODE
        HXChain::getInstance()->witnessConfig->addTrackAddress(addMultiSigAddressDialog.multiSigAddress);
        HXChain::getInstance()->witnessConfig->save();
#endif

        HXChain::getInstance()->resyncNextTime = true;
        HXChain::getInstance()->configFile->setValue("/settings/resyncNextTime", true);
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText( tr("Everytime a new multi-sig address is added, the wallet will rescan the blockchain data when launched next time."
                                  " After that the transactions of this multi-sig address will be shown.") );
        commonDialog.pop();
    }
}

void MultiSigPage::on_deleteBtn_clicked()
{
    if(ui->multiSigAddressComboBox->currentText().isEmpty())    return;

    CommonDialog commonDialog(CommonDialog::OkAndCancel);
    commonDialog.setText(tr("Sure to delete this multi-sig address locally? This operation won't affect the data on the chain."));
    if(commonDialog.pop())
    {
        HXChain::getInstance()->configFile->remove("/multisigAddresses/" + ui->multiSigAddressComboBox->currentText());
        init();
    }
}

void MultiSigPage::on_infoTableWidget_cellClicked(int row, int column)
{
    if(column == 0)
    {
        if( !ui->infoTableWidget->item(row, column))    return;

        ShowContentDialog showContentDialog( ui->infoTableWidget->item(row, column)->text(),this);

        int x = ui->infoTableWidget->columnViewportPosition(column) + ui->infoTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->infoTableWidget->rowViewportPosition(row) - 10 + ui->infoTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->infoTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();
    }
}
