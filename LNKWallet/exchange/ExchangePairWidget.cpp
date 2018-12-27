#include "ExchangePairWidget.h"
#include "ui_ExchangePairWidget.h"

#include "wallet.h"

ExchangePairWidget::ExchangePairWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangePairWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);
    setStyleSheet("#ExchangePairWidget{background:transparent;}");

}

ExchangePairWidget::~ExchangePairWidget()
{
    delete ui;
}
