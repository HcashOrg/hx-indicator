#include "WithdrawConfirmWidget.h"
#include "ui_WithdrawConfirmWidget.h"

#include <QPainter>

#include "wallet.h"
#include "commondialog.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/TransactionResultDialog.h"

class WithdrawConfirmWidget::WithdrawConfirmWidgetPrivate
{
public:
    WithdrawConfirmWidgetPrivate(const WithdrawConfirmInput &data)
        :account(data.account),ammount(data.ammount),symbol(data.symbol),crosschain_account(data.crosschain_account)
        ,pre(5)
    {
        foreach(AssetInfo asset,UBChain::getInstance()->assetInfoMap){
            if(asset.symbol == symbol)
            {
                pre = asset.precision;
                break;
            }
        }
    }
public:
    QString account;
    QString ammount;
    QString symbol;
    QString crosschain_account;
    int pre;
};

WithdrawConfirmWidget::WithdrawConfirmWidget(const WithdrawConfirmInput &data,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WithdrawConfirmWidget),
    _p(new WithdrawConfirmWidgetPrivate(data))
{
    ui->setupUi(this);
    InitWidget();
    InitData();
}

WithdrawConfirmWidget::~WithdrawConfirmWidget()
{
    delete _p;
    delete ui;
}
void WithdrawConfirmWidget::jsonDataUpdated(QString id)
{
    if( id == "withdraw-unlock-lockpage")
    {
        QString  result = UBChain::getInstance()->jsonDataValue(id);
        if( result == "\"result\":null")
        {
            ui->lineEdit->setStyleSheet("color:green");
            ui->toolButton_confirm->setEnabled(true);
        }
        else
        {
            ui->lineEdit->setStyleSheet("color:red");
            ui->toolButton_confirm->setEnabled(false);
        }
    }
    else if("withdraw-withdraw_cross_chain_transaction" == id)
    {
        QString  result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if( result.startsWith("\"result\":{"))             // 成功
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of withdraw-crosschain-balance has been sent,please wait for confirmation"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Fail to withdraw crosschain balance!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        emit closeSelf();
        close();
    }

}

void WithdrawConfirmWidget::ConfirmSlots()
{
    UBChain::getInstance()->postRPC( "withdraw-withdraw_cross_chain_transaction", toJsonFormat( "withdraw_cross_chain_transaction",
                                     QJsonArray() << _p->account<<_p->ammount<<_p->symbol<<_p->crosschain_account<<""<<true ));
    qDebug()<<_p->account << _p->ammount << _p->symbol << _p->crosschain_account;
    //close();

}

void WithdrawConfirmWidget::CancelSlots()
{
    close();
}

void WithdrawConfirmWidget::passwordChangeSlots(const QString &address)
{
    if( ui->lineEdit->text().size() < 8)
    {
        ui->lineEdit->setStyleSheet("color:red");
        return;
    }

    UBChain::getInstance()->postRPC( "withdraw-unlock-lockpage", toJsonFormat( "unlock", QJsonArray() << ui->lineEdit->text() ));
}

void WithdrawConfirmWidget::InitData()
{
    ui->label_address->setText(_p->crosschain_account);
    ui->label_totalNumber->setText(_p->ammount + "  " + _p->symbol);
    ui->label_feeNumber->setText(UBChain::getInstance()->feeChargeInfo.withDrawFee + " " + _p->symbol);
    int pre = 5;
    foreach(AssetInfo asset,UBChain::getInstance()->assetInfoMap){
        if(asset.symbol == _p->symbol)
        {
            pre = asset.precision;
            break;
        }
    }
    ui->label_actualNumber->setText(QString::number(_p->ammount.toDouble()-UBChain::getInstance()->feeChargeInfo.withDrawFee.toDouble(),'f',pre)+" "+_p->symbol);
}

void WithdrawConfirmWidget::InitWidget()
{
    InitStyle();

    ui->toolButton_confirm->setEnabled(false);
    connect( UBChain::getInstance(), &UBChain::jsonDataUpdated, this, &WithdrawConfirmWidget::jsonDataUpdated);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&WithdrawConfirmWidget::ConfirmSlots);
    connect(ui->toolButton_cancel,&QToolButton::clicked,this,&WithdrawConfirmWidget::CancelSlots);
    connect(ui->lineEdit,&QLineEdit::textEdited,this,&WithdrawConfirmWidget::passwordChangeSlots);
}

void WithdrawConfirmWidget::InitStyle()
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    //setAutoFillBackground(true);
    //QPalette palette;
    //palette.setColor(QPalette::Background, QColor(10,10,10,100));
    //setPalette(palette);

    ui->toolButton_confirm->setStyleSheet(OKBTN_STYLE);
    ui->toolButton_cancel->setStyleSheet(CANCELBTN_STYLE);
}

void WithdrawConfirmWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(10,10,10,100));//最后一位是设置透明属性（在0-255取值）
    painter.drawRect(QRect(0,0,960,580));


//    painter.setBrush(QColor(255,255,255,255));
//    painter.drawRect(220,60,320,425);

    painter.drawPixmap(320,60,325,450,QPixmap(":/ui/wallet_ui/trade.png").scaled(325,450));

    QWidget::paintEvent(event);
}
