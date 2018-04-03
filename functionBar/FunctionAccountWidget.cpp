#include "FunctionAccountWidget.h"
#include "ui_FunctionAccountWidget.h"

FunctionAccountWidget::FunctionAccountWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionAccountWidget)
{
    ui->setupUi(this);
    InitWidget();
}

FunctionAccountWidget::~FunctionAccountWidget()
{
    delete ui;
}

void FunctionAccountWidget::InitWidget()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(24,28,45));
    setPalette(palette);
}
