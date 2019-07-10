#include "showcontentdialog.h"
#include "ui_showcontentdialog.h"

#include <QClipboard>
#include <QDebug>

ShowContentDialog::ShowContentDialog(QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowContentDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);


    setStyleSheet("#ShowContentDialog{background-color:rgb(50,60,70);border:1px groove rgb(70,80,90);}");
    ui->textLabel->setStyleSheet("QLabel{background-color:rgb(50,60,70);color:rgb(192,196,212);}");
    ui->textLabel->setText(text);
    ui->textLabel->adjustSize();

    ui->copyBtn->setStyleSheet("QToolButton#copyBtn{background-image:url(:/ui/wallet_ui/copy.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                               "QToolButton#copyBtn:hover{background-image:url(:/ui/wallet_ui/copy_hover.png);}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));
    ui->copyBtn->move( 7, 4);
    ui->textLabel->move( 25, 1);

    setGeometry(0,0, ui->textLabel->width() + 30,20);
}

ShowContentDialog::~ShowContentDialog()
{
    delete ui;
}

void ShowContentDialog::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText( ui->textLabel->text() );
    close();
}
