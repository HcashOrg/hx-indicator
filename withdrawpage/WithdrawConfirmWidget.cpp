#include "WithdrawConfirmWidget.h"
#include "ui_WithdrawConfirmWidget.h"

#include <QPainter>

#include "wallet.h"

class WithdrawConfirmWidget::WithdrawConfirmWidgetPrivate
{
public:
    WithdrawConfirmWidgetPrivate(const WithdrawConfirmInput &data)
        :account(data.account),ammount(data.ammount),symbol(data.symbol),crosschain_account(data.crosschain_account)
    {

    }
public:
    QString account;
    QString ammount;
    QString symbol;
    QString crosschain_account;
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
    if( id == "id-unlock-lockpage")
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
    else if("id-withdraw_cross_chain_transaction" == id)
    {
        QString  result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id <<"asdfsdfsdfasdf"<< result;
    }

}

void WithdrawConfirmWidget::ConfirmSlots()
{
    UBChain::getInstance()->postRPC( "id-withdraw_cross_chain_transaction", toJsonFormat( "withdraw_cross_chain_transaction",
                                     QJsonArray() << _p->account<<_p->ammount<<_p->symbol<<_p->crosschain_account<<""<<true ));
    close();

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

    UBChain::getInstance()->postRPC( "id-unlock-lockpage", toJsonFormat( "unlock", QJsonArray() << ui->lineEdit->text() ));
}

void WithdrawConfirmWidget::InitData()
{
    ui->label_address->setText(_p->crosschain_account);
    ui->label_totalNumber->setText(_p->ammount + "  " + _p->symbol);
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
    //palette.setColor(QPalette::Background, QColor(0xFF,0xFF,0xFF,200));
    //setPalette(palette);

    QFont font("MicrosoftYaHeiLight",10,50);
    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0xC6,0xCA,0xD4));
    ui->label_1->setPalette(pa);
    ui->label_1->setFont(font);
    ui->label_2->setPalette(pa);
    ui->label_2->setFont(font);
    ui->label_3->setPalette(pa);
    ui->label_3->setFont(font);
    ui->label_4->setPalette(pa);
    ui->label_4->setFont(font);
    ui->label_5->setPalette(pa);
    ui->label_5->setFont(font);

    QFont font1("MicrosoftYaHeiLight",14,50);
    QPalette pa1;
    pa1.setColor(QPalette::WindowText,Qt::black);
    ui->label_address->setPalette(pa1);
    ui->label_address->setFont(font1);
    ui->label_actualNumber->setPalette(pa1);
    ui->label_actualNumber->setFont(font1);
    ui->label_feeNumber->setPalette(pa1);
    ui->label_feeNumber->setFont(font1);
    ui->label_totalNumber->setFont(font1);
    QPalette pa2;
    pa2.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_totalNumber->setPalette(pa2);

    setStyleSheet("QLineEdit{border:none;background:transparent;color:red;font-size:12pt;margin-left:2px;}"
                  "Qline{color:#5474EB;background:#5474EB;}"
                  "QToolButton{border:none;color:white;border-radius:10px;font-size:12pt;}"
                  "QToolButton#toolButton_cancel::hover,QToolButton#toolButton_confirm::hover{background-color:#00D2FF;}"
                  "QToolButton#toolButton_confirm{background-color:#5474EB;}"
                  "QToolButton#toolButton_cancel{background-color:#E5E5E5;}"
                  "QLabel{background:transparent;color:black:font-family:MicrosoftYaHeiLight;}");
}

void WithdrawConfirmWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255,255,255,240));//最后一位是设置透明属性（在0-255取值）
    painter.drawRect(rect());


    painter.setBrush(QColor(255,255,255,255));
    painter.drawRect(220,60,320,425);
    QWidget::paintEvent(event);
}
