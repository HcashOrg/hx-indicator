#include "FunctionAdvanceWidget.h"
#include "ui_FunctionAdvanceWidget.h"

#include "extra/style.h"
#include <QPainter>

FunctionAdvanceWidget::FunctionAdvanceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionAdvanceWidget)
{
    ui->setupUi(this);
    InitWidget();
}

FunctionAdvanceWidget::~FunctionAdvanceWidget()
{
    delete ui;
}
void FunctionAdvanceWidget::retranslator()
{
    ui->retranslateUi(this);
}
void FunctionAdvanceWidget::DefaultShow()
{
    MultiSigShowSlots();
}

void FunctionAdvanceWidget::PoundageShowSlots()
{
    ui->pushButton_poundage->setChecked(true);
    ui->pushButton_multiSig->setChecked(false);
    ui->pushButton_nameTransfer->setChecked(false);
    emit showPoundageSignal();
}

void FunctionAdvanceWidget::MultiSigShowSlots()
{
    ui->pushButton_poundage->setChecked(false);
    ui->pushButton_multiSig->setChecked(true);
    ui->pushButton_nameTransfer->setChecked(false);
    emit showMultiSigSignal();
}

void FunctionAdvanceWidget::NameTransferShowSlots()
{
    ui->pushButton_poundage->setChecked(false);
    ui->pushButton_multiSig->setChecked(false);
    ui->pushButton_nameTransfer->setChecked(true);
    emit showNameTransferSignal();
}

void FunctionAdvanceWidget::InitWidget()
{
    InitStyle();
    ui->pushButton_poundage->setCheckable(true);
    ui->pushButton_multiSig->setCheckable(true);
    ui->pushButton_nameTransfer->setCheckable(true);
    connect(ui->pushButton_poundage,&QPushButton::clicked,this,&FunctionAdvanceWidget::PoundageShowSlots);
    connect(ui->pushButton_multiSig,&QPushButton::clicked,this,&FunctionAdvanceWidget::MultiSigShowSlots);
    connect(ui->pushButton_nameTransfer,&QPushButton::clicked,this,&FunctionAdvanceWidget::NameTransferShowSlots);

}

void FunctionAdvanceWidget::InitStyle()
{
    setStyleSheet(FUNCTIONBAR_PUSHBUTTON_STYLE);
}

void FunctionAdvanceWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QLinearGradient linear(QPointF(0, 480), QPointF(130, 0));
    linear.setColorAt(0, QColor(71,33,84));
    linear.setColorAt(0.5, QColor(55,36,88));
    linear.setColorAt(1, QColor(51,37,90));
    linear.setSpread(QGradient::PadSpread);
    painter.setBrush(linear);
    painter.drawRect(QRect(-1,-1,131,481));

    painter.setPen(QColor(81,59,134));
    painter.drawLine(20,62,110,62);
}


