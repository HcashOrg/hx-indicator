#include "MultiSigPage.h"
#include "ui_MultiSigPage.h"

#include "wallet.h"
#include "CreateMultisigWidget.h"

MultiSigPage::MultiSigPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiSigPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    init();
}

MultiSigPage::~MultiSigPage()
{
    delete ui;
}

void MultiSigPage::jsonDataUpdated(QString id)
{
    if( id == "MultiSigPage-get_pubkey_from_account-" + ui->accountComboBox->currentText() )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        result.prepend("{");
        result.append("}");
        QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
        QString pubKey = object.value("result").toString();
        ui->pubKeyLabel->setText(pubKey);
        return;
    }
}

void MultiSigPage::on_createMultiSigBtn_clicked()
{
    emit backBtnVisible(true);

    CreateMultisigWidget* createMultisigWidget = new CreateMultisigWidget( ui->accountComboBox->currentText(), ui->pubKeyLabel->text(), this);
    createMultisigWidget->setAttribute(Qt::WA_DeleteOnClose);
    createMultisigWidget->show();
    createMultisigWidget->raise();
}

void MultiSigPage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

}

void MultiSigPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void MultiSigPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    ui->addressLabel->setText(info.address);

    HXChain::getInstance()->postRPC( "MultiSigPage-get_pubkey_from_account-" + ui->accountComboBox->currentText(),
                                     toJsonFormat( "get_pubkey_from_account", QJsonArray() << ui->accountComboBox->currentText() ));
}
