#include "ChangeCrosschainAddressDialog.h"
#include "ui_ChangeCrosschainAddressDialog.h"

#include "wallet.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "selectwalletpathwidget.h"
#include "commondialog.h"
#include "SelectColdKeyFileDialog.h"

ChangeCrosschainAddressDialog::ChangeCrosschainAddressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeCrosschainAddressDialog)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));
    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CANCELBTN_STYLE);

    init();
}

ChangeCrosschainAddressDialog::~ChangeCrosschainAddressDialog()
{
    delete ui;
}

void ChangeCrosschainAddressDialog::pop()
{
    move(0,0);
    exec();
}

void ChangeCrosschainAddressDialog::init()
{
    ui->stackedWidget->setCurrentIndex(0);

    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->getMyFormalGuards();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

    QStringList assetIds = HXChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        if(assetId == "1.3.0")  continue;
        ui->assetComboBox->addItem( revertERCSymbol( HXChain::getInstance()->assetInfoMap.value(assetId).symbol), assetId);
    }

    QIntValidator *validator = new QIntValidator(1, 60 * 60 * 24 * 365,this);
    ui->timeLineEdit->setValidator(validator);

}

void ChangeCrosschainAddressDialog::jsonDataUpdated(QString id)
{
    if( id == "ChangeCrosschainAddressDialog-get_multisig_account_pair-" + getRealAssetSymbol( ui->assetComboBox->currentText()))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if( result.startsWith("\"result\":"))             // 成功
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();

            MultisigPair    pair;
            QString latestId;
            foreach (QJsonValue v, array)
            {
                QJsonObject object = v.toObject();
                int effectiveBlock = object.take("effective_block_num").toInt();
                if(effectiveBlock != 0)     continue;

                QString id = object.take("id").toString();
                if( latestId.isEmpty() || compareVersion(id, latestId) == 1)
                {
                    latestId = id;
                    pair.id = id;
                    pair.symbol = object.take("chain_type").toString();
                    pair.hotAddress = object.take("bind_account_hot").toString();
                    pair.coldAddress = object.take("bind_account_cold").toString();
                    pair.effectiveBlock = effectiveBlock;
                }
            }

            if(latestId.isEmpty())
            {
                ui->hotAddressLabel->setText(tr("There are no new multisig-address to change to!"));
                ui->coldAddressLabel->setText(tr("There are no new multisig-address to change to!"));
            }
            else
            {
                ui->hotAddressLabel->setText(pair.hotAddress);
                ui->coldAddressLabel->setText(pair.coldAddress);
            }
        }

        return;

    }

    if( id == "id-account_change_for_crosschain")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        close();

        if( result.startsWith("\"result\":{"))             // 成功
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of change-multisig proposal has been sent out. You can check the voting state in the proposal page."));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Failed!"));
            errorResultDialog.setDetailText(result);
        }

        return;
    }

    if( id == "ChangeCrosschainAddressDialog-get_eth_multi_account_trx-" + getRealAssetSymbol( ui->assetComboBox->currentText()))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if( result.startsWith("\"result\":"))             // 成功
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.value("result").toArray();

            foreach (QJsonValue v, array)
            {
                QJsonObject object = v.toObject();
                QString symbol = object.value("symbol").toString();
                if(symbol == getRealAssetSymbol( ui->assetComboBox->currentText()))
                {
                    ethTrx.symbol = symbol;
                    ethTrx.trxId = object.value("multi_account_create_trx_id").toString();

                    QJsonArray operationArray = object.value("object_transaction").toObject().value("operations").toArray();
                    QJsonObject object2 = operationArray.at(0).toArray().at(1).toObject();
                    ethTrx.guardSignHotAddress = object2.value("guard_sign_hot_address").toString();
                    ethTrx.guardSignColdAddress = object2.value("guard_sign_cold_address").toString();

                    ui->trxIdLabel->setText(ethTrx.trxId);
                    ui->senatorHotAddressLabel->setText(ethTrx.guardSignHotAddress);
                    ui->senatorColdAddressLabel->setText(ethTrx.guardSignColdAddress);

                    httpManager.fetchCoinBalance(1011,"ETH",ethTrx.guardSignHotAddress);
                    httpManager.fetchCoinBalance(1012,"ETH",ethTrx.guardSignColdAddress);
                    needSenatorSign = true;
                    break;
                }
            }

            if(needSenatorSign)
            {
                ui->stackedWidget->setCurrentIndex(1);
                ui->accountLabel->hide();
                ui->accountComboBox->hide();
                ui->signerLabel->show();
                ui->signerLabel2->show();
            }
            else
            {
                ui->stackedWidget->setCurrentIndex(0);
                ui->accountLabel->show();
                ui->accountComboBox->show();
                ui->signerLabel->hide();
                ui->signerLabel2->hide();

                queryMultiAccountPair();
            }
        }

        return;

    }

    if( id.startsWith("ChangeCrosschainAddressDialog-get_multi_address_obj-" + getRealAssetSymbol( ui->assetComboBox->currentText())))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

        if( result.startsWith("\"result\":"))             // 成功
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.value("result").toArray();
            foreach (QJsonValue v, array)
            {
                QJsonObject object = v.toObject();
                QString multiSigAccountPairId = object.value("multisig_account_pair_object_id").toString();
                if(multiSigAccountPairId == "2.7.0")
                {
                    QString newHotAddress = object.value("new_address_hot").toString();
                    QString newColdAddress = object.value("new_address_cold").toString();

                    if(newHotAddress == ethTrx.guardSignHotAddress && newColdAddress == ethTrx.guardSignColdAddress)
                    {
                        signerId = object.value("guard_account").toString();

                        ui->signerLabel->setText(HXChain::getInstance()->guardAccountIdToName(signerId));
                    }
                    continue;
                }
            }

        }

        return;
    }

    if( id == "Finish-get_multi_address_obj-" + getRealAssetSymbol( ui->assetComboBox->currentText()))
    {
        if(signerId.isEmpty())      // 如果本钱包内没有signer
        {
            ui->signerLabel->setText(tr("Signer not in this wallet."));
        }

        return;
    }

    if( id == "ChangeCrosschainAddressDialog-senator_sign_eths_multi_account_create_trx")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")             // 成功
        {
            close();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Trx has been signed! Wait for confirmation."));
            commonDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Failed!"));
            errorResultDialog.setDetailText(result);
        }
        return;
    }
}

void ChangeCrosschainAddressDialog::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();

    int id = object.value("id").toInt();
    QJsonObject resultObject = object.value("result").toObject();

    QString balance = resultObject.value("balance").toString();
    QString address = resultObject.value("address").toString();

    if(id == 1011)
    {
        ui->senatorHotAddressLabel->setText( QString("%1 (%2 ETH)").arg(address).arg(balance));
        hotBalance = balance.toDouble();
    }
    else if(id == 1012)
    {
        ui->senatorColdAddressLabel->setText( QString("%1 (%2 ETH)").arg(address).arg(balance));
        coldBalance = balance.toDouble();
    }
}

void ChangeCrosschainAddressDialog::on_okBtn_clicked()
{
    if(needSenatorSign)
    {
        if(signerId.isEmpty())
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("You have no signer senator in this wallet!"));
            commonDialog.pop();
            return;
        }

        if(hotBalance <= 0 || coldBalance <= 0)
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Balance of hot & cold address should not be 0!"));
            commonDialog.pop();
            return;
        }

        SelectColdKeyFileDialog selectColdKeyFileDialog;
        selectColdKeyFileDialog.pop();
        if(!selectColdKeyFileDialog.filePath.isEmpty() && !selectColdKeyFileDialog.pwd.isEmpty())
        {
            HXChain::getInstance()->postRPC( "ChangeCrosschainAddressDialog-senator_sign_eths_multi_account_create_trx", toJsonFormat( "senator_sign_eths_multi_account_create_trx",
                                             QJsonArray() << ui->trxIdLabel->text() << ui->signerLabel->text()
                                             << selectColdKeyFileDialog.filePath << selectColdKeyFileDialog.pwd));
        }

//        CommonDialog commonDialog(CommonDialog::OkAndCancel);
//        commonDialog.setText(tr("Sure to sign this trx?"));
//        if(commonDialog.pop())
//        {
//            HXChain::getInstance()->postRPC( "ChangeCrosschainAddressDialog-senator_sign_eths_multi_account_create_trx", toJsonFormat( "senator_sign_eths_multi_account_create_trx",
//                                             QJsonArray() << ui->trxIdLabel->text() << ui->signerLabel->text()
//                                             << "" << ""));
//        }
    }
    else
    {
        if(ui->hotAddressLabel->text().isEmpty() || ui->hotAddressLabel->text() == tr("There are no new multisig-address to change to!"))
        {
            return;
        }

        if(ui->timeLineEdit->text().toInt() < 3600)
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("The expiration time should not less than 3600s(1 hour) !"));
            commonDialog.pop();

            return;
        }

        HXChain::getInstance()->postRPC( "id-account_change_for_crosschain", toJsonFormat( "account_change_for_crosschain",
                                         QJsonArray() << ui->accountComboBox->currentText() << getRealAssetSymbol( ui->assetComboBox->currentText())
                                         << ui->hotAddressLabel->text() << ui->coldAddressLabel->text()
                                         << ui->timeLineEdit->text().toInt() << true ));
    }

}

void ChangeCrosschainAddressDialog::on_closeBtn_clicked()
{
    close();
}

void ChangeCrosschainAddressDialog::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    ui->hotAddressLabel->clear();
    ui->coldAddressLabel->clear();
    ui->trxIdLabel->clear();
    ui->senatorHotAddressLabel->clear();
    ui->senatorColdAddressLabel->clear();
    needSenatorSign = false;
    signerId = "";
    ui->signerLabel->clear();
    hotBalance = 0;
    coldBalance = 0;

    if( getRealAssetSymbol( ui->assetComboBox->currentText()) == "ETH" || getRealAssetSymbol( ui->assetComboBox->currentText()).startsWith("ERC"))
    {
        HXChain::getInstance()->postRPC( "ChangeCrosschainAddressDialog-get_eth_multi_account_trx-" + getRealAssetSymbol( ui->assetComboBox->currentText()),
                                         toJsonFormat( "get_eth_multi_account_trx", QJsonArray() << 0));

        getSenatorMultiAddress();
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(0);
        ui->accountLabel->show();
        ui->accountComboBox->show();
        ui->signerLabel->hide();
        ui->signerLabel2->hide();

        queryMultiAccountPair();
    }

}


void ChangeCrosschainAddressDialog::queryMultiAccountPair()
{
    HXChain::getInstance()->postRPC( "ChangeCrosschainAddressDialog-get_multisig_account_pair-" + getRealAssetSymbol( ui->assetComboBox->currentText()),
                                     toJsonFormat( "get_multisig_account_pair", QJsonArray() << getRealAssetSymbol( ui->assetComboBox->currentText())));

}

void ChangeCrosschainAddressDialog::getSenatorMultiAddress()
{
    QStringList guards = HXChain::getInstance()->getMyFormalGuards();

    foreach (QString guard, guards)
    {
        AccountInfo info = HXChain::getInstance()->accountInfoMap.value(guard);
        HXChain::getInstance()->postRPC( "ChangeCrosschainAddressDialog-get_multi_address_obj-" + getRealAssetSymbol( ui->assetComboBox->currentText()) + "-" + info.id,
                                         toJsonFormat( "get_multi_address_obj", QJsonArray() << getRealAssetSymbol( ui->assetComboBox->currentText())
                                                                                << info.id));
    }

    HXChain::getInstance()->postRPC( "Finish-get_multi_address_obj-" + getRealAssetSymbol( ui->assetComboBox->currentText()),
                                     toJsonFormat( "get_multi_address_obj", QJsonArray() ));
}
