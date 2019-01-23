#include "TokenTransferWidget.h"
#include "ui_TokenTransferWidget.h"

#include "wallet.h"
#include "transfer/ContactChooseWidget.h"
#include "transfer/BlurWidget.h"
#include "FeeChooseWidget.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"

TokenTransferWidget::TokenTransferWidget(QString _accountName, QString _tokenSymbol, ContractTokenPage *parent) :
    QWidget(parent),
    page(parent),
    ui(new Ui::TokenTransferWidget)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);

    ui->sendBtn->setStyleSheet(OKBTN_STYLE);

    ui->memoTextEdit->setStyleSheet("QTextEdit{background: transparent;color: rgb(83,61,138);font: 11px \"Microsoft YaHei UI Light\";border:none;border-bottom:1px solid rgb(83,61,138);}"
                                    "QTextEdit:focus{border-bottom-color:rgb(83,61,138);}"
                                    "QTextEdit:disabled{color: rgb(151,151,151);}");

    setStyleSheet("QToolButton#toolButton_chooseContact{background-image:url(:/ui/wallet_ui/trans.png);border:none;"
                   "background-color:transparent;background-repeat: no-repeat;background-position: center;}"
                   "QToolButton#toolButton_chooseContact:hover{background-color:black;");

    init();

    ui->accountComboBox->setCurrentText(_accountName);
    ui->tokenComboBox->setCurrentText(_tokenSymbol);
}

TokenTransferWidget::~TokenTransferWidget()
{
    delete ui;
}

void TokenTransferWidget::init()
{
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

    QStringList contractIds = page->contractTokenInfoMap.keys();
    foreach (QString contractId, contractIds)
    {
        QString symbol = page->contractTokenInfoMap.value(contractId).symbol;
        ui->tokenComboBox->addItem(symbol, contractId);
    }

    QRegExp regx("[a-zA-Z0-9\ \n]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->sendtoLineEdit->setValidator( validator );
    ui->sendtoLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->tipLabel3->hide();
    ui->tipLabel4->hide();
    ui->tipLabel6->hide();

    transferFeeWidget = new FeeChooseWidget( 0,HXChain::getInstance()->feeType,
                                                 ui->accountComboBox->currentText());
    ui->stackedWidget->addWidget(transferFeeWidget);
    ui->stackedWidget->setCurrentWidget(transferFeeWidget);
    ui->stackedWidget->currentWidget()->resize(ui->stackedWidget->size());

    inited = true;

    setTokenBalance();
    ui->sendBtn->setEnabled(false);

    connect(ui->amountLineEdit,&QLineEdit::textChanged,[this](const QString &number){
        QDoubleValidator* via = dynamic_cast<QDoubleValidator*>(const_cast<QValidator*>(ui->amountLineEdit->validator()));
        if(!via)
        {
            return;
        }
        if(ui->amountLineEdit->text().toDouble() > via->top())
        {
            ui->amountLineEdit->setText(ui->amountLineEdit->text().remove(ui->amountLineEdit->text().length()-1,1));
        }
    });

}

void TokenTransferWidget::jsonDataUpdated(QString id)
{
    if( id.startsWith( "TokenTransferWidget-invoke_contract_testing-transfer-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            HXChain::TotalContractFee totalFee = HXChain::getInstance()->parseTotalContractFee(result);
            stepCount = totalFee.step;

            unsigned long long totalAmount = totalFee.baseAmount + ceil(totalFee.step * HXChain::getInstance()->contractFee / 100.0);
            transferFeeWidget->updateFeeNumberSlots(getBigNumberString(totalAmount, ASSET_PRECISION).toDouble());
            ui->sendBtn->setEnabled(true);
        }
        else
        {
            transferFeeWidget->updateFeeNumberSlots(0);
            ui->sendBtn->setEnabled(false);
        }

        return;
    }

    if( id.startsWith("TokenTransferWidget-invoke_contract-transfer-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":"))
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of invoking contract function \"transfer\" has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();

            close();
        }
        else if(result.startsWith("\"error\":"))
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Invoking contract function \"transfer\" failed!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        return;
    }
}

void TokenTransferWidget::on_sendBtn_clicked()
{
    if(!transferFeeWidget)     return;
    transferFeeWidget->updatePoundageID();

    ContractTokenInfo tokenInfo = page->contractTokenInfoMap.value(ui->tokenComboBox->currentData(Qt::UserRole).toString());
    QString params = QString("%1,%2").arg(ui->sendtoLineEdit->text()).arg(decimalToIntegerStr(ui->amountLineEdit->text(), tokenInfo.precision.size() - 1));
    if(ui->memoTextEdit->toPlainText().size() > 0)
    {
        params += "," + ui->memoTextEdit->toPlainText();
    }

    HXChain::getInstance()->postRPC( "TokenTransferWidget-invoke_contract-transfer-" + params, toJsonFormat( "invoke_contract",
                                     QJsonArray() << ui->accountComboBox->currentText()
                                     << HXChain::getInstance()->currentContractFee() << stepCount
                                     << tokenInfo.contractId
                                     << "transfer"  << params));
}

void TokenTransferWidget::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited) return;
    page->setAccount(arg1);
    ui->amountLineEdit->clear();
    setTokenBalance();
    transferFeeWidget->updateAccountNameSlots(ui->accountComboBox->currentText(), true);
}

void TokenTransferWidget::on_tokenComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited) return;
    ui->amountLineEdit->clear();
    setTokenBalance();
}

void TokenTransferWidget::setTokenBalance()
{
//    ui->amountLineEdit->clear();

    QString contractId = ui->tokenComboBox->currentData(Qt::UserRole).toString();
    ContractTokenInfo tokenInfo = page->contractTokenInfoMap.value(contractId);
    TokenBalance balance = page->accountContractTokenBalanceMap.value(ui->accountComboBox->currentText()).value(contractId);
    ui->amountLineEdit->setPlaceholderText(tr("Max: %1 %2").arg(getBigNumberString(balance.amount.toULongLong(), tokenInfo.precision.size() - 1))
                                           .arg(tokenInfo.symbol));

//    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(tokenInfo.precision.size() - 1));
//    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
//    ui->amountLineEdit->setValidator(pReg1);

    QDoubleValidator *validator = new QDoubleValidator(0,getBigNumberString(balance.amount.toULongLong(), tokenInfo.precision.size() - 1).toDouble(),
                                                       tokenInfo.precision.size() - 1, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->amountLineEdit->setValidator( validator );
}

void TokenTransferWidget::calculateTransferFee()
{

    if( !checkAddress(ui->sendtoLineEdit->text()) || ui->amountLineEdit->text().toDouble() == 0
            || !checkAddress(ui->tokenComboBox->currentData(Qt::UserRole).toString(), ContractAddress)
            || ui->accountComboBox->currentText().isEmpty())
    {
        transferFeeWidget->updateFeeNumberSlots(0);
        ui->sendBtn->setEnabled(false);
        return;
    }


    ContractTokenInfo tokenInfo = page->contractTokenInfoMap.value(ui->tokenComboBox->currentData(Qt::UserRole).toString());
    QString params = QString("%1,%2").arg(ui->sendtoLineEdit->text()).arg(decimalToIntegerStr(ui->amountLineEdit->text(), tokenInfo.precision.size() - 1));
    if(ui->memoTextEdit->toPlainText().size() > 0)
    {
        params += "," + ui->memoTextEdit->toPlainText();
    }

    HXChain::getInstance()->postRPC( "TokenTransferWidget-invoke_contract_testing-transfer-" + params, toJsonFormat( "invoke_contract_testing",
                                     QJsonArray() << ui->accountComboBox->currentText()
                                     << tokenInfo.contractId
                                     << "transfer"  << params));
}


void TokenTransferWidget::on_toolButton_chooseContact_clicked()
{
    ContactChooseWidget *wi = new ContactChooseWidget(this);
    if(wi->isInitSuccess())
    {
        BlurWidget *blur = new BlurWidget(this);
        connect(wi,&ContactChooseWidget::closeSignal,blur,&BlurWidget::close);
        connect(wi,&ContactChooseWidget::selectContactSignal,this,&TokenTransferWidget::selectContactSlots);

        blur->show();
        wi->move(QPoint(160,140));
        wi->show();
        wi->raise();
    }
}

void TokenTransferWidget::selectContactSlots(const QString &name, const QString &address)
{
    ui->sendtoLineEdit->setText(address);
    on_sendtoLineEdit_textEdited(ui->sendtoLineEdit->text());
}

void TokenTransferWidget::on_memoTextEdit_textChanged()
{
    calculateTransferFee();
}

void TokenTransferWidget::on_sendtoLineEdit_textEdited(const QString &arg1)
{
    if( ui->sendtoLineEdit->text().contains(" ") || ui->sendtoLineEdit->text().contains("\n"))   // 不判断就remove的话 右键菜单撤销看起来等于不能用
    {
        ui->sendtoLineEdit->setText( ui->sendtoLineEdit->text().simplified().remove(" "));
    }

    if( ui->sendtoLineEdit->text().isEmpty())
    {
        ui->tipLabel4->hide();
    }
    else
    {
        AddressType type = checkAddress(ui->sendtoLineEdit->text(),AccountAddress);
        if( type == AccountAddress)
        {
            ui->tipLabel4->setText(tr("Valid account address."));
            ui->tipLabel4->setStyleSheet("color: rgb(0,170,0);");
            ui->tipLabel4->show();
        }
        else
        {
            ui->tipLabel4->setText(tr("Invalid address."));
            ui->tipLabel4->setStyleSheet("color: rgb(255,0,0);");
            ui->tipLabel4->show();
        }
    }

    calculateTransferFee();
}

void TokenTransferWidget::on_amountLineEdit_textChanged(const QString &arg1)
{
    calculateTransferFee();
}
