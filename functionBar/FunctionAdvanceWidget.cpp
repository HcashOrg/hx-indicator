#include "FunctionAdvanceWidget.h"
#include "ui_FunctionAdvanceWidget.h"

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

void FunctionAdvanceWidget::InitWidget()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(24,28,45));
    setPalette(palette);

    connect(ui->pushButton_feedPage,&QPushButton::clicked,this,&FunctionAdvanceWidget::showFeedPage);
    connect(ui->pushButton_multiSig,&QPushButton::clicked,this,&FunctionAdvanceWidget::showMultiSigPage);
}
