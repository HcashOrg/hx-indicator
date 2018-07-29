#include "foreclosedialog.h"
#include "ui_foreclosedialog.h"

#include "wallet.h"
#include "commondialog.h"
#include "FeeChooseWidget.h"

ForecloseDialog::ForecloseDialog(const QString &accountName, QString _assetSymbol, QString _maxAmount, QWidget *parent) :
    QDialog(parent),
    yesOrNo(false),
    assetSymbol(_assetSymbol),
    maxAmount(_maxAmount),
    ui(new Ui::ForecloseDialog)
{
    ui->setupUi(this);

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

    ui->stackedWidget->addWidget(new FeeChooseWidget(HXChain::getInstance()->feeChargeInfo.minerForeCloseFee.toDouble(),
                                                     HXChain::getInstance()->feeType,accountName));
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->currentWidget()->resize(ui->stackedWidget->size());


    AssetInfo info = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(_assetSymbol));
    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(info.precision));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->setPlaceholderText(tr("Max: %1 %2").arg(_maxAmount).arg(_assetSymbol));
}

ForecloseDialog::~ForecloseDialog()
{
    delete ui;
}

bool ForecloseDialog::pop()
{
    move(0,0);
    exec();

    amountStr = ui->amountLineEdit->text();
    return yesOrNo;
}

void ForecloseDialog::on_okBtn_clicked()
{
    yesOrNo = true;
    close();
}

void ForecloseDialog::on_cancelBtn_clicked()
{
    yesOrNo = false;
    close();
}

void ForecloseDialog::on_closeBtn_clicked()
{
    yesOrNo = false;
    close();
}


void ForecloseDialog::on_amountLineEdit_textEdited(const QString &arg1)
{
    if(ui->amountLineEdit->text().toDouble() > maxAmount.toDouble())
    {
        ui->amountLineEdit->setText(maxAmount);
    }
}
