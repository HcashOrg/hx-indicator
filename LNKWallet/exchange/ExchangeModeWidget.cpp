#include "ExchangeModeWidget.h"
#include "ui_ExchangeModeWidget.h"

ExchangeModeWidget::ExchangeModeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangeModeWidget)
{
    ui->setupUi(this);

    init();
}

ExchangeModeWidget::~ExchangeModeWidget()
{
    delete ui;
}

void ExchangeModeWidget::init()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);

}

void ExchangeModeWidget::on_orderModeBtn_clicked()
{
    close();
}
