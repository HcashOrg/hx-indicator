#include "KLineWidget.h"
#include "ui_KLineWidget.h"

KLineWidget::KLineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KLineWidget)
{
    ui->setupUi(this);
}

KLineWidget::~KLineWidget()
{
    delete ui;
}
