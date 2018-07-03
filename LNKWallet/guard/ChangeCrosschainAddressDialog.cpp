#include "ChangeCrosschainAddressDialog.h"
#include "ui_ChangeCrosschainAddressDialog.h"

#include "wallet.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "selectwalletpathwidget.h"
#include "commondialog.h"

ChangeCrosschainAddressDialog::ChangeCrosschainAddressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeCrosschainAddressDialog)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CANCELBTN_STYLE);

    ui->assetComboBox->setStyleSheet(COMBOBOX_STYLE_BOTTOMBORDER);
    ui->accountComboBox->setStyleSheet(COMBOBOX_STYLE_BOTTOMBORDER);

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
    ui->accountComboBox->clear();
    QStringList accounts = UBChain::getInstance()->getMyFormalGuards();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    QStringList assetIds = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        if(assetId == "1.3.0")  continue;
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(assetId).symbol, assetId);
    }

    QIntValidator *validator = new QIntValidator(1, 60 * 60 * 24 * 365,this);
    ui->timeLineEdit->setValidator(validator);
}

void ChangeCrosschainAddressDialog::jsonDataUpdated(QString id)
{
    if( id == "ChangeCrosschainAddressDialog-get_multisig_account_pair-" + ui->assetComboBox->currentText())
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

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
        QString result = UBChain::getInstance()->jsonDataValue(id);
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
}

void ChangeCrosschainAddressDialog::on_okBtn_clicked()
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

    UBChain::getInstance()->postRPC( "id-account_change_for_crosschain", toJsonFormat( "account_change_for_crosschain",
                                     QJsonArray() << ui->accountComboBox->currentText() << ui->assetComboBox->currentText()
                                     << ui->hotAddressLabel->text() << ui->coldAddressLabel->text()
                                     << ui->timeLineEdit->text().toInt() << true ));

}

void ChangeCrosschainAddressDialog::on_closeBtn_clicked()
{
    close();
}

void ChangeCrosschainAddressDialog::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    ui->hotAddressLabel->clear();
    ui->coldAddressLabel->clear();

    UBChain::getInstance()->postRPC( "ChangeCrosschainAddressDialog-get_multisig_account_pair-" + ui->assetComboBox->currentText(),
                                     toJsonFormat( "get_multisig_account_pair", QJsonArray() << ui->assetComboBox->currentText()));

}
