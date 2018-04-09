#include <QListView>

#ifdef WIN32
#include <windows.h>
#endif



#include "transferpage.h"
#include "ui_transferpage.h"
#include "wallet.h"

#include "contactdialog.h"
#include "remarkdialog.h"
#include "commondialog.h"
#include "transferconfirmdialog.h"


TransferPage::TransferPage(QString name,QWidget *parent) :
    QWidget(parent),
    accountName(name),
    inited(false),
    assetUpdating(false),
    contactUpdating(false),
    ui(new Ui::TransferPage)
{
	

    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->accountComboBox->setView(new QListView());
    ui->assetComboBox->setView(new QListView());
    ui->contactComboBox->setView(new QListView());


    if( accountName.isEmpty())  // 如果是点击账单跳转
    {
        if( UBChain::getInstance()->addressMap.size() > 0)
        {
            accountName = UBChain::getInstance()->addressMap.keys().at(0);
        }
        else  // 如果还没有账户
        {
            emit back();    // 跳转在functionbar  这里并没有用
            return;
        }
    }

    // 账户下拉框按字母顺序排序
    QStringList keys = UBChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems( keys);
    if( accountName.isEmpty() )
    {
        ui->accountComboBox->setCurrentIndex(0);
    }
    else
    {
        ui->accountComboBox->setCurrentText( accountName);
    }

    QString showName = UBChain::getInstance()->addressMapValue(accountName).ownerAddress;
    ui->addressLabel->setText(showName);


    ui->amountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    setAmountPrecision();

    QRegExp regx("[a-zA-Z0-9\-\.\ \n]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->sendtoLineEdit->setValidator( validator );
    ui->sendtoLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->feeLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->feeLineEdit->setReadOnly(true);
    updateTransactionFee();

    QRegExp rx("^([0]|[1-9][0-9]{0,5})(?:\\.\\d{0,4})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->feeLineEdit->setValidator(pReg);

//    ui->messageLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
//    ui->messageLineEdit->setTextMargins(8,0,0,0);

    ui->tipLabel3->hide();
    ui->tipLabel4->hide();
    ui->tipLabel6->hide();
    ui->callContractFeeLabel->hide();
    ui->callContractFeeLabel2->hide();
    ui->callContractFeeLabel3->hide();


    on_amountLineEdit_textChanged(ui->amountLineEdit->text());


    getContactsList();
    getAssets();

//    ui->sendBtn->setEnabled(false);

    inited = true;

	
}

TransferPage::~TransferPage()
{
	
    delete ui;

	
}



void TransferPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if( !inited)  return;

    accountName = arg1;
    UBChain::getInstance()->mainFrame->setCurrentAccount(accountName);
    ui->addressLabel->setText(UBChain::getInstance()->accountInfoMap.value(accountName).address);

    on_amountLineEdit_textChanged(ui->amountLineEdit->text());
}


void TransferPage::on_sendBtn_clicked()
{
    if(ui->amountLineEdit->text().size() == 0 || ui->sendtoLineEdit->text().size() == 0)
    {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText( tr("Please enter the amount and address."));
        tipDialog.pop();
        return;
    }

    if( ui->amountLineEdit->text().toDouble()  <= 0)
    {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText( tr("The amount can not be 0"));
        tipDialog.pop();
        return;
    }

//    if( ui->feeLineEdit->text().toDouble() <= 0)
//    {
//        CommonDialog tipDialog(CommonDialog::OkOnly);
//        tipDialog.setText( tr("The fee can not be 0"));
//        tipDialog.pop();
//        return;
//    }


    QString remark = ui->memoTextEdit->toPlainText();

//    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
//    QByteArray ba = utfCodec->fromUnicode(remark);
//    if( ba.size() > 40)
//    {
//        CommonDialog tipDialog(CommonDialog::OkOnly);
//        tipDialog.setText( tr("Message length more than 40 bytes!"));
//        tipDialog.pop();
//        return;
//    }


    AddressType type = checkAddress(ui->sendtoLineEdit->text(),AccountAddress | MultiSigAddress);
    if( type == AccountAddress)
    {
        TransferConfirmDialog transferConfirmDialog( ui->sendtoLineEdit->text(), ui->amountLineEdit->text(), ui->feeLineEdit->text(), remark, ui->assetComboBox->currentText());
        bool yOrN = transferConfirmDialog.pop();
        if( yOrN)
        {
            //        QString str = "wallet_set_transaction_fee " + ui->feeLineEdit->text() + '\n';
            //        Hcash::getInstance()->write(str);
            //        QString result = Hcash::getInstance()->read();



            UBChain::getInstance()->postRPC( "id-transfer_to_address-" + accountName,
                                             toJsonFormat( "transfer_to_address",
                                                           QJsonArray() << accountName << ui->sendtoLineEdit->text()
                                                           << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
                                                           << remark << true ));

        }

    }
//    else if(type == MultiSigAddress)
//    {
//        if(ui->assetComboBox->currentIndex() != 0)
//        {
//            CommonDialog commonDialog(CommonDialog::OkOnly);
//            commonDialog.setText(tr("You can only send %1s to multisig address currently.").arg(ASSET_NAME));
//            commonDialog.pop();
//        }
//        else
//        {
//            TransferConfirmDialog transferConfirmDialog( ui->sendtoLineEdit->text(), ui->amountLineEdit->text(), ui->feeLineEdit->text(), remark, ui->assetComboBox->currentText());
//            bool yOrN = transferConfirmDialog.pop();
//            if(yOrN)
//            {
//                AssetInfo info = UBChain::getInstance()->assetInfoMap.value(0);
//                UBChain::getInstance()->postRPC( "id_wallet_multisig_deposit+" + accountName,
//                                                 toJsonFormat( "wallet_multisig_deposit",
//                                                               QJsonArray() << ui->amountLineEdit->text() << info.symbol << accountName
//                                                               << ui->sendtoLineEdit->text() << remark ));
//            }

//        }


//    }
}



void TransferPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
    painter.drawRect(0,0,680,482);
}

void TransferPage::on_amountLineEdit_textChanged(const QString &arg1)
{
//    double amount = ui->amountLineEdit->text().toDouble();
//    double fee = ui->feeLineEdit->text().toDouble();
//    QString strBalanceTemp = UBChain::getInstance()->balanceMapValue(accountName).remove(",");
//    strBalanceTemp = strBalanceTemp.remove(" " + QString(ASSET_NAME));
//    double dBalance = strBalanceTemp.remove(",").toDouble();


//    if( ui->assetComboBox->currentIndex() == 0)
//    {
//        if( amount + fee > dBalance )
//        {
//            ui->tipLabel3->show();
//            ui->tipLabel3->setText( "UB" + tr(" not enough"));

//            ui->sendBtn->setEnabled(false);
//        }
//        else
//        {
//            ui->tipLabel3->hide();

//            ui->sendBtn->setEnabled(true);
//        }
//    }

}

void TransferPage::refresh()
{

}


void TransferPage::contactSelected(QString remark, QString contact)
{
    ui->sendtoLineEdit->setText(contact);
    on_sendtoLineEdit_textEdited(ui->sendtoLineEdit->text());
}


void TransferPage::getContactsList()
{
    contactUpdating = true;

    if( !UBChain::getInstance()->contactsFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "contact.dat not exist";
    }
    QString str = QByteArray::fromBase64( UBChain::getInstance()->contactsFile->readAll());
    UBChain::getInstance()->contactsFile->close();

    QStringList strList = str.split(";");
    strList.removeLast();
    int size = strList.size();

    for( int i = 0; i < size; i++)
    {
        QString str2 = strList.at(i);
        contactsList += str2.left( str2.indexOf("="));
        if( str2.size() - 1 == str2.indexOf("="))    // 如果没有备注，text为地址  data为空
        {
            ui->contactComboBox->addItem( str2.left( str2.indexOf("=")),"");
        }
        else                                         // 如果有备注，text为备注  data为地址
        {
            ui->contactComboBox->addItem( str2.mid( str2.indexOf("=") + 1), str2.left( str2.indexOf("=")));
        }
    }

    ui->contactComboBox->setCurrentIndex(-1);

    contactUpdating = false;

}

void TransferPage::setAmountPrecision()
{
    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(ASSET_PRECISION));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
}

QString TransferPage::getCurrentAccount()
{
    return accountName;
}

void TransferPage::setAddress(QString address)
{
    ui->sendtoLineEdit->setText(address);
}

void TransferPage::setContact(QString contactRemark)
{
    ui->contactComboBox->setCurrentText(contactRemark);
}

void TransferPage::getAssets()
{
    assetUpdating = true;

//    int index = ui->assetComboBox->currentIndex();
//    ui->assetComboBox->clear();
//    foreach (int key, UBChain::getInstance()->assetInfoMap.keys())
//    {
//        ui->assetComboBox->addItem( UBChain::getInstance()->assetInfoMap.value(key).symbol);
//    }

//    if( index < 0 )   index = 0;
//    ui->assetComboBox->setCurrentIndex(index);

    QStringList keys = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString key, keys)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(key).symbol);
    }


    assetUpdating = false;
}


void TransferPage::updateTransactionFee()
{
    ui->feeLineEdit->setText( getBigNumberString(UBChain::getInstance()->transactionFee,ASSET_PRECISION));
}


void TransferPage::jsonDataUpdated(QString id)
{
    if( id == "id-transfer_to_address-" + accountName)
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":{"))             // 成功
        {
//            QString recordId = result.mid( result.indexOf("\"entry_id\"") + 12, 40);

//            mutexForPendingFile.lock();

//            mutexForConfigFile.lock();
//            UBChain::getInstance()->configFile->setValue("/recordId/" + recordId , 0);
//            mutexForConfigFile.unlock();

//            if( !UBChain::getInstance()->pendingFile->open(QIODevice::ReadWrite))
//            {
//                qDebug() << "pending.dat open fail";
//                return;
//            }

//            QByteArray ba = QByteArray::fromBase64( UBChain::getInstance()->pendingFile->readAll());
//            ba += QString( recordId + "," + accountName + "," + ui->sendtoLineEdit->text() + "," + ui->amountLineEdit->text() + "," + ui->feeLineEdit->text() + ";").toUtf8();
//            ba = ba.toBase64();
//            UBChain::getInstance()->pendingFile->resize(0);
//            QTextStream ts(UBChain::getInstance()->pendingFile);
//            ts << ba;

//            UBChain::getInstance()->pendingFile->close();

//            mutexForPendingFile.unlock();

            CommonDialog tipDialog(CommonDialog::OkOnly);
            tipDialog.setText( tr("Transaction has been sent,please wait for confirmation"));
            tipDialog.pop();

//            if( !contactsList.contains( ui->sendtoLineEdit->text()))
//            {
//                CommonDialog commonDialog(CommonDialog::OkAndCancel);
//                commonDialog.setText(tr("Add this address to contacts?"));
//                if( commonDialog.pop())
//                {
//                    RemarkDialog remarkDialog( ui->sendtoLineEdit->text());
//                    remarkDialog.pop();
//                    getContactsList();
//                }
//            }
//            emit showAccountPage(accountName);
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog tipDialog(CommonDialog::OkOnly);
            tipDialog.setText( tr("Transaction sent failed: %1").arg(errorMessage));
            tipDialog.pop();

        }
        return;
    }



//    if( id == "id_wallet_multisig_deposit+" + accountName)
//    {
//        QString result = UBChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;
//        if( result.mid(0,18) == "\"result\":{\"index\":")             // 成功
//        {
//            QString recordId = result.mid( result.indexOf("\"entry_id\"") + 12, 40);

//            mutexForPendingFile.lock();

//            mutexForConfigFile.lock();
//            UBChain::getInstance()->configFile->setValue("/recordId/" + recordId , 0);
//            mutexForConfigFile.unlock();

//            if( !UBChain::getInstance()->pendingFile->open(QIODevice::ReadWrite))
//            {
//                qDebug() << "pending.dat open fail";
//                return;
//            }

//            QByteArray ba = QByteArray::fromBase64( UBChain::getInstance()->pendingFile->readAll());
//            ba += QString( recordId + "," + accountName + "," + ui->sendtoLineEdit->text() + "," + ui->amountLineEdit->text() + "," + ui->feeLineEdit->text() + ";").toUtf8();
//            ba = ba.toBase64();
//            UBChain::getInstance()->pendingFile->resize(0);
//            QTextStream ts(UBChain::getInstance()->pendingFile);
//            ts << ba;

//            UBChain::getInstance()->pendingFile->close();

//            mutexForPendingFile.unlock();

//            CommonDialog tipDialog(CommonDialog::OkOnly);
//            tipDialog.setText( tr("Transaction has been sent,please wait for confirmation"));
//            tipDialog.pop();

//            if( !contactsList.contains( ui->sendtoLineEdit->text()))
//            {
//                CommonDialog commonDialog(CommonDialog::OkAndCancel);
//                commonDialog.setText(tr("Add this address to contacts?"));
//                if( commonDialog.pop())
//                {
//                    RemarkDialog remarkDialog( ui->sendtoLineEdit->text());
//                    remarkDialog.pop();
//                    getContactsList();
//                }
//            }
//            emit showAccountPage(accountName);
//        }
//        else
//        {
//            int pos = result.indexOf("\"message\":\"") + 11;
//            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);
//            qDebug() << "errorMessage : " << errorMessage;

//            if( errorMessage == "Assert Exception")
//            {
//                if( result.contains("\"format\":\"my->is_receive_account( from_account_name ): Invalid account name\","))
//                {
//                    CommonDialog tipDialog(CommonDialog::OkOnly);
//                    tipDialog.setText( tr("This name has been registered, please rename this account!"));
//                    tipDialog.pop();
//                }
//                else
//                {
//                    CommonDialog tipDialog(CommonDialog::OkOnly);
//                    tipDialog.setText( tr("Wrong address!"));
//                    tipDialog.pop();
//                }


//            }
//            else if( errorMessage == "imessage size bigger than soft_max_lenth")
//            {
//                CommonDialog tipDialog(CommonDialog::OkOnly);
//                tipDialog.setText( tr("Message too long!"));
//                tipDialog.pop();

//            }
//            else if( errorMessage == "invalid transaction expiration")
//            {
//                CommonDialog tipDialog(CommonDialog::OkOnly);
//                tipDialog.setText( tr("Failed: You need to wait for synchronization to complete"));
//                tipDialog.pop();
//            }
//            else if( errorMessage == "insufficient funds")
//            {
//                CommonDialog tipDialog(CommonDialog::OkOnly);
//                tipDialog.setText( tr("Not enough balance!"));
//                tipDialog.pop();
//            }
//            else if( errorMessage == "Out of Range")
//            {
//                CommonDialog tipDialog(CommonDialog::OkOnly);
//                tipDialog.setText( tr("Wrong address!"));
//                tipDialog.pop();
//            }
//            else if( errorMessage == "Parse Error")
//            {
//                CommonDialog tipDialog(CommonDialog::OkOnly);
//                tipDialog.setText( tr("Wrong address!"));
//                tipDialog.pop();
//            }
//            else
//            {
//                CommonDialog tipDialog(CommonDialog::OkOnly);
//                tipDialog.setText( tr("Transaction sent failed"));
//                tipDialog.pop();
//            }

//        }
//        return;

//    }
}


void TransferPage::on_assetComboBox_currentIndexChanged(int index)
{
    if( assetUpdating)  return;


    if( index == 0)
    {
        ui->memoLabel->show();
        ui->memoTextEdit->show();
        ui->callContractFeeLabel->hide();
        ui->callContractFeeLabel2->hide();
        ui->callContractFeeLabel3->hide();
    }
    else
    {
        ui->memoLabel->hide();
        ui->memoTextEdit->hide();
        ui->callContractFeeLabel->show();
        ui->callContractFeeLabel2->show();
        ui->callContractFeeLabel3->show();
    }

    ui->amountAssetLabel->setText(ui->assetComboBox->currentText());
    ui->amountLineEdit->clear();

    setAmountPrecision();

    on_amountLineEdit_textChanged(ui->amountLineEdit->text());

}

void TransferPage::on_sendtoLineEdit_textEdited(const QString &arg1)
{
    if( ui->sendtoLineEdit->text().contains(" ") || ui->sendtoLineEdit->text().contains("\n"))   // 不判断就remove的话 右键菜单撤销看起来等于不能用
    {
        ui->sendtoLineEdit->setText( ui->sendtoLineEdit->text().simplified().remove(" "));
    }

//    ui->sendtoLineEdit->setText( ui->sendtoLineEdit->text().remove("\n"));
    if( ui->sendtoLineEdit->text().isEmpty()  )
    {
        ui->tipLabel4->hide();
        ui->callContractFeeLabel->clear();
        return;
    }

    AddressType type = checkAddress(ui->sendtoLineEdit->text(),AccountAddress | ContractAddress | MultiSigAddress);
    if( type == AccountAddress)
    {
        ui->tipLabel4->setText(tr("Valid account address."));
        ui->tipLabel4->setStyleSheet("color: rgb(43,230,131);");
        ui->tipLabel4->show();
//        calculateCallContractFee();
    }
    else if( type == ContractAddress)
    {
        ui->tipLabel4->setText(tr("Sending coins to contract address is not supported currently."));
        ui->tipLabel4->setStyleSheet("color: rgb(255,34,76);");
        ui->tipLabel4->show();
        ui->callContractFeeLabel->clear();
    }
    else if( type == MultiSigAddress)
    {
        if(ui->assetComboBox->currentIndex() > 0)
        {
            ui->tipLabel4->setText(tr("You can only send %1s to multisig address currently.").arg(ASSET_NAME));
            ui->tipLabel4->setStyleSheet("color: rgb(255,34,76);");
            ui->tipLabel4->show();
            ui->callContractFeeLabel->clear();
        }
        else
        {
            ui->tipLabel4->setText(tr("Valid multisig address."));
            ui->tipLabel4->setStyleSheet("color: rgb(43,230,131);");
            ui->tipLabel4->show();
            ui->callContractFeeLabel->clear();
        }
    }
    else
    {
        ui->tipLabel4->setText(tr("Invalid address."));
        ui->tipLabel4->setStyleSheet("color: rgb(255,34,76);");
        ui->tipLabel4->show();
        ui->callContractFeeLabel->clear();
    }


}


void TransferPage::on_memoTextEdit_textChanged()
{
    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(ui->memoTextEdit->toPlainText());
    if( ba.size() > 40)
    {
        ui->tipLabel6->show();
    }
    else
    {
        ui->tipLabel6->hide();
    }
}

void TransferPage::on_contactComboBox_currentIndexChanged(int index)
{
    if(contactUpdating)     return;

    QString data = ui->contactComboBox->currentData().toString();
    QString address;
    QString remark;
    if(data.isEmpty())
    {
        address = ui->contactComboBox->currentText();
        remark = "";
    }
    else
    {
        address = ui->contactComboBox->currentData().toString();
        remark = ui->contactComboBox->currentText();
    }

    ui->sendtoLineEdit->setText(address);
    on_sendtoLineEdit_textEdited(ui->sendtoLineEdit->text());
}
