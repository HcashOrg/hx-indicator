#include "TransactionResultDialog.h"
#include "ui_TransactionResultDialog.h"

#include "wallet.h"
#include <QClipboard>

TransactionResultDialog::TransactionResultDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransactionResultDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->copyBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/copy.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                               "QToolButton:hover{background-image:url(:/ui/wallet_ui/copy_hover.png);}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));
}

TransactionResultDialog::~TransactionResultDialog()
{
    delete ui;
}

void TransactionResultDialog::setInfoText(QString _text)
{
    ui->infoLabel->setText(_text);
}

void TransactionResultDialog::setDetailText(QString _text)
{
    ui->textBrowser->setPlainText(_text);

    if(_text.startsWith("\"result\":"))
    {
        UBChain::getInstance()->parseTransaction(_text);
    }
}

void TransactionResultDialog::pop()
{
    move(0,0);
    exec();
}

void TransactionResultDialog::on_okBtn_clicked()
{
    close();
}

void TransactionResultDialog::on_closeBtn_clicked()
{
    close();
}

void TransactionResultDialog::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->textBrowser->toPlainText());
}
