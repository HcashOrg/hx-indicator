#include "CapitalConfirmWidget.h"
#include "ui_CapitalConfirmWidget.h"

#include "wallet.h"
class CapitalConfirmWidget::DataPrivate
{
public:
    DataPrivate(const CapitalConfirmWidget::CapitalConfirmInput &data)
        :address(data.address),fee(data.fee),actual(data.actual),total(data.total)
    {

    }
public:
    QString address;
    QString fee;
    QString actual;
    QString total;

};

CapitalConfirmWidget::CapitalConfirmWidget(const CapitalConfirmInput &input,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CapitalConfirmWidget),
    _p(new DataPrivate(input))
{
    ui->setupUi(this);
    InitWidget();
}

CapitalConfirmWidget::~CapitalConfirmWidget()
{
    delete _p;
    delete ui;
}

void CapitalConfirmWidget::jsonDataUpdated(QString id)
{
    if( id == "capital-unlock-lockpage")
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

}

void CapitalConfirmWidget::ConfirmSlots()
{
    emit ConfirmSignal();
    close();

}

void CapitalConfirmWidget::CancelSlots()
{
    emit CancelSignal();
    close();
}

void CapitalConfirmWidget::passwordChangeSlots(const QString &address)
{
    if( ui->lineEdit->text().size() < 8)
    {
        ui->lineEdit->setStyleSheet("color:red");
        return;
    }

    UBChain::getInstance()->postRPC( "capital-unlock-lockpage", toJsonFormat( "unlock", QJsonArray() << ui->lineEdit->text() ));
}

void CapitalConfirmWidget::InitData()
{
    ui->label_address->setText(_p->address);
    ui->label_totalNumber->setText(_p->total);
    ui->label_feeNumber->setText(_p->fee);
    ui->label_actualNumber->setText(_p->actual);

}
void CapitalConfirmWidget::InitWidget()
{
    InitStyle();

    InitData();
    ui->toolButton_confirm->setEnabled(false);
    connect( UBChain::getInstance(), &UBChain::jsonDataUpdated, this, &CapitalConfirmWidget::jsonDataUpdated);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&CapitalConfirmWidget::ConfirmSlots);
    connect(ui->toolButton_cancel,&QToolButton::clicked,this,&CapitalConfirmWidget::CancelSlots);
    connect(ui->lineEdit,&QLineEdit::textEdited,this,&CapitalConfirmWidget::passwordChangeSlots);
}

void CapitalConfirmWidget::InitStyle()
{

    setAttribute(Qt::WA_TranslucentBackground, true);
    //setAutoFillBackground(true);
    //QPalette palette;
    //palette.setColor(QPalette::Background, QColor(10,10,10,100));
    //setPalette(palette);


    ui->toolButton_confirm->setStyleSheet(OKBTN_STYLE);
    ui->toolButton_cancel->setStyleSheet(CANCELBTN_STYLE);
}

void CapitalConfirmWidget::paintEvent(QPaintEvent *event)
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
