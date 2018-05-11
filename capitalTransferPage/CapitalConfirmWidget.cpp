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

    QFont font("Microsoft YaHei UI Light",10,50);
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


    QFont font1("Microsoft YaHei UI Light",14,50);
    QPalette pa1;
    pa1.setColor(QPalette::WindowText,Qt::black);
    ui->label_actualNumber->setPalette(pa1);
    ui->label_actualNumber->setFont(font1);
    ui->label_feeNumber->setPalette(pa1);
    ui->label_feeNumber->setFont(font1);
    ui->label_totalNumber->setFont(font1);

    QPalette paw;
    paw.setColor(QPalette::WindowText,Qt::white);
    QFont fontw("Microsoft YaHei UI Light",14,63);
    ui->label_title->setFont(fontw);
    ui->label_title->setPalette(paw);

    QPalette pa2;
    pa2.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_totalNumber->setPalette(pa2);

    QFont fontad("Microsoft YaHei UI Light",10,50);
    QPalette paad;
    paad.setColor(QPalette::WindowText,Qt::black);

    ui->label_address->setPalette(paad);
    ui->label_address->setFont(fontad);


    setStyleSheet("QLineEdit{border:none;background:transparent;color:red;font-size:10pt;margin-left:2px;}"
                  "Qline{color:#5474EB;background:#5474EB;}"
                  "QToolButton{border:none;color:white;border-radius:10px;font-size:12pt;}"
                  "QToolButton#toolButton_cancel::hover,QToolButton#toolButton_confirm::hover{background-color:#00D2FF;}"
                  "QToolButton#toolButton_confirm{background-color:#5474EB;}"
                  "QToolButton#toolButton_cancel{background-color:#E5E5E5;}"
                  "QLabel{background:transparent;color:black:font-family:Microsoft YaHei UI Light;}");
}

void CapitalConfirmWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(10,10,10,100));//最后一位是设置透明属性（在0-255取值）
    painter.drawRect(QRect(0,0,960,580));


//    painter.setBrush(QColor(255,255,255,255));
//    painter.drawRect(220,60,320,425);

    painter.drawPixmap(410,90,325,450,QPixmap(":/ui/wallet_ui/trade.png").scaled(325,450));

    QWidget::paintEvent(event);
}
