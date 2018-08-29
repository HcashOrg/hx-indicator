#include "AccountInfoWidget.h"
#include "ui_AccountInfoWidget.h"

#include "wallet.h"

AccountInfoWidget::AccountInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountInfoWidget)
{
    ui->setupUi(this);

    init();
}

AccountInfoWidget::~AccountInfoWidget()
{
    delete ui;
}

void AccountInfoWidget::init()
{
    setStyleSheet("QLabel{font: 12px \"微软雅黑 Light\";color: rgb(51, 51, 51);}");
}

void AccountInfoWidget::setAccount(QString accountName)
{
    ui->nameLabel->setText(accountName);
    ui->nameLabel->adjustSize();
    ui->nameLabel->setGeometry(ui->nameLabel->x(), ui->nameLabel->y(), ui->nameLabel->width(), 20);

    ui->guardLabel->setGeometry(ui->nameLabel->x() + ui->nameLabel->width() + 6, ui->nameLabel->y() + 4, 12, 12);

    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(accountName);
    ui->addressLabel->setText(info.address);

    if(HXChain::getInstance()->allGuardMap.contains(accountName))
    {
        if(HXChain::getInstance()->allGuardMap.value(accountName).isFormal)
        {
            ui->guardLabel->setPixmap(QPixmap(":/ui/wallet_ui/guard_formal.png"));
        }
        else
        {
            ui->guardLabel->setPixmap(QPixmap(":/ui/wallet_ui/guard_normal.png"));
        }
    }
    else
    {
        ui->guardLabel->hide();
    }
}
