#include "choosecontractoperationdialog.h"
#include "ui_choosecontractoperationdialog.h"

#include "wallet.h"
#include "../commondialog.h"

#include <QDebug>


ChooseContractOperationDialog::ChooseContractOperationDialog(QWidget *parent) :
    QDialog(parent),
    choice(0),
    ui(new Ui::ChooseContractOperationDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);


//    setStyleSheet("ChooseContractOperationDialog{border:1px solid rgb(48,52,72);border-radius:5px;background:transparent;}");
//    setStyleSheet("ChooseContractOperationDialog{border:none;}");

}

ChooseContractOperationDialog::~ChooseContractOperationDialog()
{
    delete ui;
}


void ChooseContractOperationDialog::on_depositBtn_clicked()
{
    choice = 1;
    close();
}

void ChooseContractOperationDialog::on_mintBtn_clicked()
{
    choice = 2;
    close();
}

void ChooseContractOperationDialog::on_destroyBtn_clicked()
{
    choice = 3;
    close();
}

void ChooseContractOperationDialog::on_withdrawBtn_clicked()
{
    choice = 4;
    close();
}

void ChooseContractOperationDialog::paintEvent(QPaintEvent *e)
{
//    QStyleOption opt;

//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(e);
}
