#include "FeeChargeWidget.h"
#include "ui_FeeChargeWidget.h"

#include "FeeChooseWidget.h"

#include <QPainter>

class FeeChargeWidget::FeeChargeWidgetPrivate
{
public:
    FeeChargeWidgetPrivate()
        :chooseWidget(nullptr)
    {
        tip = tr("You should pay for this operation: %1 %2");
    }
public:
    FeeChooseWidget *chooseWidget;
    QString tip;
};

FeeChargeWidget::FeeChargeWidget(double feeNumber,const QString &feeType,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeeChargeWidget),
    _p(new FeeChargeWidgetPrivate())
{
    ui->setupUi(this);
    _p->chooseWidget = new FeeChooseWidget(feeNumber,feeType);
    InitWidget();
}

FeeChargeWidget::~FeeChargeWidget()
{
    delete _p;
    delete ui;
}

void FeeChargeWidget::ConfirmSlots()
{
    emit confirmSignal();
    hide();
    close();
}

void FeeChargeWidget::CancelSlots()
{
    emit cancelSignal();
    hide();
    close();
}

void FeeChargeWidget::InitWidget()
{
    InitStyle();
    ui->stackedWidget->addWidget(_p->chooseWidget);
    ui->label_2->setText(_p->tip.arg(_p->chooseWidget->GetFeeNumber()).arg(_p->chooseWidget->GetFeeType()));

    connect(ui->toolButton_cancel,&QToolButton::clicked,this,&FeeChargeWidget::CancelSlots);
    connect(ui->toolButton_close,&QToolButton::clicked,this,&FeeChargeWidget::CancelSlots);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&FeeChargeWidget::ConfirmSlots);

//    connect(ui->toolButton_cancel,&QToolButton::clicked,this,&FeeChargeWidget::close);
//    connect(ui->toolButton_close,&QToolButton::clicked,this,&FeeChargeWidget::close);
//    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&FeeChargeWidget::close);
}

void FeeChargeWidget::InitStyle()
{
    setAttribute(Qt::WA_TranslucentBackground, true);

    setStyleSheet("QToolButton#toolButton_confirm{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
             "QToolButton#toolButton_cancel::hover,QToolButton#toolButton_confirm::hover{background-color:#00D2FF;}"
    "QToolButton#toolButton_cancel{background-color:#E5E5E5; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
             );

    ui->toolButton_close->setIconSize(QSize(12,12));
    ui->toolButton_close->setIcon(QIcon(":/ui/wallet_ui/close.png"));
    ui->toolButton_close->setStyleSheet("QToolButton{background-color:transparent;border:none;}"
                                "QToolButton:hover{background-color:rgb(208,228,255);}");


}

void FeeChargeWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(10,10,10,100));//最后一位是设置透明属性（在0-255取值）
    painter.drawRect(rect());


//    painter.setBrush(QColor(255,255,255,255));
//    painter.drawRect(220,60,320,425);

    painter.drawPixmap(300,140,420,350,QPixmap(":/ui/wallet_ui/whitebord.png").scaled(420,350));

    QWidget::paintEvent(event);
}
