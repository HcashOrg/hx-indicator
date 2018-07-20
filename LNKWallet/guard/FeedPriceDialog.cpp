#include "FeedPriceDialog.h"
#include "ui_FeedPriceDialog.h"

#include "wallet.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"

#include "QtMath"

FeedPriceDialog::FeedPriceDialog( QString _assetSymbol, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FeedPriceDialog)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CANCELBTN_STYLE);

    ui->accountComboBox->setStyleSheet(COMBOBOX_STYLE_BOTTOMBORDER);

    ui->assetLabel->setText(_assetSymbol);

    init();
}

FeedPriceDialog::~FeedPriceDialog()
{
    delete ui;
}

void FeedPriceDialog::pop()
{
    move(0,0);
    exec();
}


void FeedPriceDialog::init()
{
    ui->accountComboBox->clear();

    QStringList keys = HXChain::getInstance()->accountInfoMap.keys();
    QStringList publishers;
    AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(ui->assetLabel->text()));
    foreach (QString key, keys)
    {
        AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(key);
        if(assetInfo.publishers.contains(accountInfo.address))
        {
            publishers += key;
        }
    }

    if(publishers.size() == 0)
    {
        ui->accountComboBox->hide();
        ui->noAcountLabel->show();
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->accountComboBox->addItems(publishers);
        ui->noAcountLabel->hide();
        ui->okBtn->setEnabled(true);
    }

    if(publishers.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

    QString str = QString("%1 : %2").arg( getBigNumberString(assetInfo.quoteAmount.amount, ASSET_PRECISION))
            .arg( getBigNumberString(assetInfo.baseAmount.amount, assetInfo.precision));
    ui->currentRateLabel->setText(str);


    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,5})?$|(^\\t?$)"));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->quoteLineEdit->setValidator(pReg1);
}

void FeedPriceDialog::jsonDataUpdated(QString id)
{
    if( id == "id-publish_normal_asset_feed")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result.startsWith("\"result\":{"))             // 成功
        {
            close();

            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of feed-price has been sent out."));
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

void FeedPriceDialog::on_okBtn_clicked()
{
    if(ui->accountComboBox->currentText().isEmpty() || ui->quoteLineEdit->text().toDouble() <= 0)
    {
        return;
    }


    AssetInfo baseAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(ui->assetLabel->text()));
    QJsonObject baseObject;
    baseObject.insert("amount", QString::number( qPow(10, baseAssetInfo.precision), 'f', 0));
    baseObject.insert("asset_id", baseAssetInfo.id);
    QJsonObject quoteObject;
    quoteObject.insert("amount", QString::number(ui->quoteLineEdit->text().toDouble() * qPow(10,ASSET_PRECISION), 'f', 0));
    quoteObject.insert("asset_id", "1.3.0");
    QJsonObject settlementPriceObject;
    settlementPriceObject.insert("base", baseObject);
    settlementPriceObject.insert("quote", quoteObject);
    QJsonObject object;
    object.insert("settlement_price", settlementPriceObject);

//    object.insert("core_exchange_rate", settlementPriceObject);

    HXChain::getInstance()->postRPC( "id-publish_normal_asset_feed", toJsonFormat( "publish_normal_asset_feed",
                            QJsonArray() << ui->accountComboBox->currentText() << ui->assetLabel->text() << object << true));

    qDebug() << toJsonFormat( "publish_normal_asset_feed",
                              QJsonArray() << ui->accountComboBox->currentText() << ui->assetLabel->text() << object << true);
}

void FeedPriceDialog::on_closeBtn_clicked()
{
    close();
}

void FeedPriceDialog::on_quoteLineEdit_textEdited(const QString &arg1)
{

}
