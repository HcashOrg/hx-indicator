#include "ApplyChoiceDialog.h"
#include "ui_ApplyChoiceDialog.h"

#include "wallet.h"

ApplyChoiceDialog::ApplyChoiceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplyChoiceDialog)
{
    ui->setupUi(this);

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->payBtn->setStyleSheet(OKBTN_STYLE);
    ui->submitBtn->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

}

ApplyChoiceDialog::~ApplyChoiceDialog()
{
    delete ui;
}

int ApplyChoiceDialog::pop()
{
    move(0,0);
    exec();

    return choice;
}

void ApplyChoiceDialog::on_payBtn_clicked()
{
    choice = 1;
    close();
}

void ApplyChoiceDialog::on_submitBtn_clicked()
{
    choice = 2;
    close();
}

void ApplyChoiceDialog::on_closeBtn_clicked()
{
    choice = 0;
    close();
}
