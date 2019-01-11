#include "CheckExchangePairWidget.h"
#include "ui_CheckExchangePairWidget.h"

CheckExchangePairWidget::CheckExchangePairWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckExchangePairWidget)
{
    ui->setupUi(this);
}

CheckExchangePairWidget::~CheckExchangePairWidget()
{
    delete ui;
}

void CheckExchangePairWidget::setPair(const ExchangePair &_pair)
{
    pair = _pair;
    ui->pairLabel->setText( revertERCSymbol( pair.first) + "/" + revertERCSymbol( pair.second));
    ui->iconLabel->setStyleSheet(QString("border-image: url(:/ui/wallet_ui/coin_%1.png);").arg(pair.first));
}

void CheckExchangePairWidget::setChecked(bool isChecked)
{
    ui->checkBox->setChecked(isChecked);
}

void CheckExchangePairWidget::on_checkBox_stateChanged(int arg1)
{
    Q_EMIT stateChanged(pair, ui->checkBox->isChecked());
}
