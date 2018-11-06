#include "FeeChargeWidget.h"
#include "ui_FeeChargeWidget.h"

#include "FeeChooseWidget.h"
#include "wallet.h"

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

FeeChargeWidget::FeeChargeWidget(double feeNumber,const QString &feeType,const QString &accountName,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeeChargeWidget),
    _p(new FeeChargeWidgetPrivate())
{
    ui->setupUi(this);
    _p->chooseWidget = new FeeChooseWidget(feeNumber,feeType,accountName,this);
    connect(_p->chooseWidget,&FeeChooseWidget::feeSufficient,ui->toolButton_confirm,&QToolButton::setEnabled);
    ui->label_2->setText(_p->tip.arg(feeNumber).arg(ASSET_NAME));
    ui->label_char->setVisible(false);
    QTimer::singleShot(100,[this](){this->ui->toolButton_confirm->setEnabled(_p->chooseWidget->isSufficient());});
    InitWidget();
}

FeeChargeWidget::~FeeChargeWidget()
{
    delete _p;
    delete ui;
}

void FeeChargeWidget::SetInfo(const QString &info,bool vi)
{
    ui->label_char->setVisible(vi);
    if(vi)
    {
        ui->label_char->setText(info);
    }
}

void FeeChargeWidget::updatePoundageID()
{
    _p->chooseWidget->updatePoundageID();
}

void FeeChargeWidget::SetTitle(const QString &title)
{
    ui->label_title->setText(title);
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
    _p->chooseWidget->resize(ui->stackedWidget->size());

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

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->toolButton_confirm->setStyleSheet(OKBTN_STYLE);
    ui->toolButton_cancel->setStyleSheet(CANCELBTN_STYLE);
    ui->toolButton_close->setStyleSheet(CLOSEBTN_STYLE);
}

void FeeChargeWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QColor(10,10,10,100));//最后一位是设置透明属性（在0-255取值）
//    painter.drawRect(rect());

    QWidget::paintEvent(event);
}
