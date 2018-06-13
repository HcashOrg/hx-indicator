#include "ErrorResultDialog.h"
#include "ui_ErrorResultDialog.h"

#include "wallet.h"
#include <QClipboard>

ErrorResultDialog::ErrorResultDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorResultDialog)
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

ErrorResultDialog::~ErrorResultDialog()
{
    delete ui;
}

void ErrorResultDialog::setInfoText(QString _text)
{
    ui->infoLabel->setText(_text);
}

void ErrorResultDialog::setDetailText(QString _text)
{
    ui->textBrowser->setPlainText(_text);
}

void ErrorResultDialog::pop()
{
    move(0,0);
    exec();
}

void ErrorResultDialog::on_okBtn_clicked()
{
    close();
}

void ErrorResultDialog::on_closeBtn_clicked()
{
    close();
}

void ErrorResultDialog::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->textBrowser->toPlainText());
}
