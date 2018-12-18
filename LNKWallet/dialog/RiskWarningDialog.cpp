#include "RiskWarningDialog.h"
#include "ui_RiskWarningDialog.h"

#include "wallet.h"

RiskWarningDialog::RiskWarningDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RiskWarningDialog)
{
    ui->setupUi(this);

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    ui->okBtn->setEnabled(false);
}

RiskWarningDialog::~RiskWarningDialog()
{
    delete ui;
}

void RiskWarningDialog::setInfoText(QString _text)
{
    ui->infoLabel->setText(_text);
}

void RiskWarningDialog::setReadingTime(int _seconds)
{
    seconds = _seconds;
}

void RiskWarningDialog::pop()
{
    ui->okBtn->setText(tr("I know (%1)").arg(seconds));
    timer->setInterval(1000);
    timer->start();
    move(0,0);
    exec();
}

void RiskWarningDialog::on_okBtn_clicked()
{
    close();
}

void RiskWarningDialog::onTimer()
{
    seconds--;
    if(seconds < 1)
    {
        ui->okBtn->setText(tr("I know"));
        timer->stop();
        ui->okBtn->setEnabled(true);
    }
    else
    {
        ui->okBtn->setText(tr("I know (%1)").arg(seconds));
    }
}
