#include "ApplyExchangeInfoDialog.h"
#include "ui_ApplyExchangeInfoDialog.h"

#include "wallet.h"
#include "dapp/ContractTokenPage.h"
#include "commondialog.h"

ApplyExchangeInfoDialog::ApplyExchangeInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplyExchangeInfoDialog)
{
    ui->setupUi(this);

    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    init();
}

ApplyExchangeInfoDialog::~ApplyExchangeInfoDialog()
{
    delete ui;
}

void ApplyExchangeInfoDialog::pop()
{
    move(0,0);
    exec();
}

void ApplyExchangeInfoDialog::init()
{

}

void ApplyExchangeInfoDialog::setParentPage(ContractTokenPage *p)
{
    page = p;
    QStringList contractIds = page->contractTokenInfoMap.keys();
    for(int i = 0; i < contractIds.size(); i++)
    {
        QString contractId = contractIds.at(i);
        ContractTokenInfo tokenInfo = page->contractTokenInfoMap.value(contractId);
        QString str = QString("%1 (%2)").arg(tokenInfo.symbol).arg(contractId);
        ui->contractComboBox->addItem(str, contractId);
    }

}

void ApplyExchangeInfoDialog::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    qDebug() << "11111111111 " << object;
    int id = object.value("id").toInt();

    if(id == 1024)
    {
        bool result = object.value("result").toBool();
        if(result)
        {
            CommonDialog dialog(CommonDialog::OkOnly);
            dialog.setText(tr("Your application has been submitted."));
            dialog.pop();
            close();
        }
        else
        {
            CommonDialog dialog(CommonDialog::OkOnly);
            dialog.setText(tr("Failed!"));
            dialog.pop();
        }
    }
}

void ApplyExchangeInfoDialog::on_okBtn_clicked()
{
    if(ui->contractComboBox->currentText().isEmpty())    return;
    if(ui->memoLineEdit->text().size() != 8)
    {
        CommonDialog dialog(CommonDialog::OkOnly);
        dialog.setText(tr("Please enter the pay code!"));
        dialog.pop();
        return;
    }

    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",1024);
    object.insert("method","Zchain.Coin.Add.Request");
    QJsonObject paramObject;
    paramObject.insert("chainId", "HX");

    QString contractId = ui->contractComboBox->currentData().toString();
    ContractTokenInfo tokenInfo = page->contractTokenInfoMap.value(contractId);
    paramObject.insert("coinName", tokenInfo.symbol);

    paramObject.insert("desc", ui->descriptionLineEdit->text());
    paramObject.insert("contractAddr", ui->contractComboBox->currentText());
    paramObject.insert("contact", ui->phoneLineEdit->text());
    paramObject.insert("payCode", ui->memoLineEdit->text());
    object.insert("params",paramObject);
    httpManager.post(MIDDLE_DEFAULT_URL,QJsonDocument(object).toJson());
}

void ApplyExchangeInfoDialog::on_cancelBtn_clicked()
{
    close();
}

void ApplyExchangeInfoDialog::on_closeBtn_clicked()
{
    close();
}
