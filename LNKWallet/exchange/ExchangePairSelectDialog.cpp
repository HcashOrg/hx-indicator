#include "ExchangePairSelectDialog.h"
#include "ui_ExchangePairSelectDialog.h"

#include "wallet.h"

ExchangePairSelectDialog::ExchangePairSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExchangePairSelectDialog)
{
    ui->setupUi(this);

//    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::Popup);

    setStyleSheet("#ExchangePairSelectDialog{border-image: url(:/ui/wallet_ui/dialogBorder.png);background:transparent;}");


}

ExchangePairSelectDialog::~ExchangePairSelectDialog()
{
    delete ui;
}

void ExchangePairSelectDialog::on_addBtn_clicked()
{

}
