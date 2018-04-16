#include "myexchangecontractpage.h"
#include "ui_myexchangecontractpage.h"

#include "wallet.h"
#include "commondialog.h"

#define CONTRACT_ADDRESS "CLNKKNgt2JSFtMYn8HkjZkJFWPxJgqGkv5p9p"

MyExchangeContractPage::MyExchangeContractPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyExchangeContractPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));


    init();
}

MyExchangeContractPage::~MyExchangeContractPage()
{
    delete ui;
}

void MyExchangeContractPage::init()
{
    QStringList accounts = UBChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    inited = true;
}

void MyExchangeContractPage::jsonDataUpdated(QString id)
{
    if( id == "id-register_contract")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Create exchange contract successfully!") );
            commonDialog.pop();

        }
        else if(result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Create exchange contract failed: " + errorMessage );
            commonDialog.pop();
        }
    }
}

void MyExchangeContractPage::on_createBtn_clicked()
{
    CommonDialog commonDialog(CommonDialog::OkAndCancel);
    commonDialog.setText(tr("Sure to create an exchange contract for account %1").arg(ui->accountComboBox->currentText()));
    if(commonDialog.pop())
    {
        QString filePath = QDir::currentPath() + "/contracts/blocklink_exchange2.lua.gpc";
        QFileInfo fileInfo(filePath);
        if(fileInfo.exists())
        {
            UBChain::getInstance()->postRPC( "id-register_contract", toJsonFormat( "register_contract",
                                                                                   QJsonArray() << ui->accountComboBox->currentText() << "0.00000001"
                                                                                   << "10000"  << filePath));
        }
        else
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Can not find file contracts/blocklink_exchange.glua.gpc!"));
            commonDialog.pop();
        }

    }
}

void MyExchangeContractPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}

void MyExchangeContractPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;

    UBChain::getInstance()->currentAccount = ui->accountComboBox->currentText();
}
