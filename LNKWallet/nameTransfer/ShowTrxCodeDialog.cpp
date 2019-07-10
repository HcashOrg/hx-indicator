#include "ShowTrxCodeDialog.h"
#include "ui_ShowTrxCodeDialog.h"

#include <QClipboard>
#include <QDebug>

ShowTrxCodeDialog::ShowTrxCodeDialog(QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowTrxCodeDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);


    setStyleSheet("#ShowTrxCodeDialog{background-color:rgb(50,60,70);border:1px groove rgb(70,80,90);}");
    ui->plainTextEdit->setStyleSheet("QPlainTextEdit{background-color:rgb(50,60,70);color:rgb(192,196,212);border:none;}");
    ui->plainTextEdit->setPlainText(text);
    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->adjustSize();

    ui->copyBtn->setStyleSheet("QToolButton#copyBtn{background-image:url(:/ui/wallet_ui/copy.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                               "QToolButton#copyBtn:hover{background-image:url(:/ui/wallet_ui/copy_hover.png);}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));
    ui->copyBtn->move( 7, 4);
    ui->plainTextEdit->move( 25, 1);

    setGeometry(0,0, ui->plainTextEdit->width() + 30, ui->plainTextEdit->height() + 3);
}

ShowTrxCodeDialog::~ShowTrxCodeDialog()
{
    delete ui;
}

void ShowTrxCodeDialog::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText( ui->plainTextEdit->toPlainText() );
    close();
}
