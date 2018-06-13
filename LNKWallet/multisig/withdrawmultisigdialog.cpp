#include "withdrawmultisigdialog.h"
#include "ui_withdrawmultisigdialog.h"

#include <QClipboard>
#include <QListView>


#include "wallet.h"
#include "../commondialog.h"
#include "multisiginfodialog.h"
#include "dialog/checkpwddialog.h"

WithdrawMultiSigDialog::WithdrawMultiSigDialog(QString _multiSigAddress, QWidget *parent) :
    QDialog(parent),
    contactUpdating(false),
    ui(new Ui::WithdrawMultiSigDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->copyBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn2->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->contactComboBox->setView(new QListView());
    ui->multiSigAddressComboBox->setView(new QListView());

    ui->copyBtn->setToolTip(tr("copy to clipboard"));

    QRegExp regx("[a-zA-Z0-9\-\.\ \n]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->sendtoLineEdit->setValidator( validator );
    ui->sendtoLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    QRegExp rx1("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,5})?$|(^\\t?$)");
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);


    init();

    ui->multiSigAddressComboBox->setCurrentText(_multiSigAddress);
}

WithdrawMultiSigDialog::~WithdrawMultiSigDialog()
{
    delete ui;
}

void WithdrawMultiSigDialog::pop()
{
    move(0,0);
    exec();
}

void WithdrawMultiSigDialog::getContactsList()
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

void WithdrawMultiSigDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith("id_wallet_multisig_withdraw_start+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
qDebug() << id << result;
        if( result.startsWith("\"result\":") )             // 成功
        {
            ui->stackedWidget->setCurrentIndex(1);

            QString builderStr = result.mid(9);
            ui->transactionBrowser->setText(builderStr);

            QString path = id.mid(QString("id_wallet_multisig_withdraw_start+").size());
            path = path.left( path.lastIndexOf("/") );

#ifdef WIN32
            path.replace( "/", "\\");
            QProcess::startDetached("explorer \"" + path + "\"");
#else
            QProcess::startDetached("open \"" + path + "\"");
#endif
        }

        return;
    }
}

void WithdrawMultiSigDialog::on_okBtn_clicked()
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

    if( ui->feeLabel->text().toDouble() <= 0)
    {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText( tr("The fee can not be 0"));
        tipDialog.pop();
        return;
    }

    if( ui->feeLabel->text().toDouble() + ui->amountLineEdit->text().toDouble() > ui->balanceLabel->text().toDouble())
    {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText( tr("Not enough balance!"));
        tipDialog.pop();
        return;
    }


    QString remark = ui->memoLineEdit->text();
//    remark.remove(' ');
    if( remark.size() == 0)    // 转地址如果没有备注 会自动添加 TO ...   所以添加空格
    {
        remark = " ";
    }

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(remark);
    if( ba.size() > 40)
    {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText( tr("Message length more than 40 bytes!"));
        tipDialog.pop();
        return;
    }

    AddressType type = checkAddress(ui->sendtoLineEdit->text(),AccountAddress | ContractAddress | MultiSigAddress | ScriptAddress);
    if( type == AccountAddress)
    {
        CheckPwdDialog checkPwdDialog;
        if(!checkPwdDialog.pop())   return;

        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy.MM.dd_hh.mm.ss");
        QString fileName = "M_to_U_" + current_date + ".json";
        QString filePath = ui->pathLineEdit->text() + "/" + fileName;

        UBChain::getInstance()->postRPC( "id_wallet_multisig_withdraw_start+" + filePath,
                                         toJsonFormat( "wallet_multisig_withdraw_start",
                                                       QJsonArray() << ui->amountLineEdit->text() << ASSET_NAME << ui->multiSigAddressComboBox->currentText()
                                                       << ui->sendtoLineEdit->text() << remark << filePath));
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Invalid account address!"));
        commonDialog.pop();
    }

}

void WithdrawMultiSigDialog::on_cancelBtn_clicked()
{
    close();
}

void WithdrawMultiSigDialog::init()
{
    QStringList keys = UBChain::getInstance()->multiSigInfoMap.keys();
    ui->multiSigAddressComboBox->addItems(keys);

    ui->feeLabel->setText( getBigNumberString(UBChain::getInstance()->transactionFee,ASSET_PRECISION));

    QString path = UBChain::getInstance()->walletConfigPath;
    path.replace("\\","/");
    ui->pathLineEdit->setText( path);


    ui->stackedWidget->setCurrentIndex(0);

    getContactsList();

}

void WithdrawMultiSigDialog::on_multiSigAddressComboBox_currentIndexChanged(const QString &arg1)
{
    if(ui->multiSigAddressComboBox->currentText().isEmpty())    return;
    unsigned long long balance = UBChain::getInstance()->multiSigInfoMap.value(ui->multiSigAddressComboBox->currentText()).balanceMap.value(0);
    ui->balanceLabel->setText(getBigNumberString(balance,ASSET_PRECISION));
}



void WithdrawMultiSigDialog::on_selectPathBtn_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "Select the path to store the blockchain");
    if( !path.isEmpty())
    {
        ui->pathLineEdit->setText( path);
    }
}

void WithdrawMultiSigDialog::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText( ui->transactionBrowser->toPlainText() );
}

void WithdrawMultiSigDialog::on_closeBtn_clicked()
{
    close();
}

void WithdrawMultiSigDialog::on_detailBtn_clicked()
{
    MultiSigInfoDialog multiSigInfoDialog(ui->multiSigAddressComboBox->currentText());
    multiSigInfoDialog.pop();
}

void WithdrawMultiSigDialog::on_sendtoLineEdit_textEdited(const QString &arg1)
{
    QString address = ui->sendtoLineEdit->text().simplified();
    ui->sendtoLineEdit->setText(address);
}

void WithdrawMultiSigDialog::on_closeBtn2_clicked()
{
    close();
}

void WithdrawMultiSigDialog::on_contactComboBox_currentIndexChanged(int index)
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
