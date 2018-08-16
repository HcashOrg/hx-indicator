#include "commondialog.h"
#include "ui_commondialog.h"
#include <QDebug>

#include "wallet.h"

CommonDialog::CommonDialog(commonDialogType _type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommonDialog)
{
    
    ui->setupUi(this);


    setParent(HXChain::getInstance()->mainFrame);
    move(HXChain::getInstance()->mainFrame->pos());

    HXChain::getInstance()->appendCurrentDialogVector(this);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);

    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setText(tr("Ok"));
    ui->cancelBtn->setText(tr("Cancel"));

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);


    yesOrNO = false;

    if( _type == OkAndCancel)
    {
    }
    else if( _type == OkOnly)
    {
        ui->cancelBtn->hide();
        ui->okBtn->move((ui->containerWidget->width() - ui->okBtn->width()) / 2, 150);
    }
    else if( _type == YesOrNo)
    {
        ui->okBtn->setText(tr("Yes"));
        ui->cancelBtn->setText(tr("No"));
    }

    
}

CommonDialog::~CommonDialog()
{
    
    qDebug() << "CommonDialog  delete ";
    delete ui;
    HXChain::getInstance()->removeCurrentDialogVector(this);
    
}

void CommonDialog::on_okBtn_clicked()
{
    yesOrNO = true;
    close();
}

void CommonDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

void CommonDialog::adaptSize()
{
    ui->textLabel->setFixedWidth(220);
    ui->textLabel->setMinimumHeight(60);
    ui->textLabel->adjustSize();

    int height = ui->textLabel->height();
    ui->okBtn->move(ui->okBtn->x(), 30 + height + 30);
    ui->cancelBtn->move(ui->cancelBtn->x(), 30 + height + 30);
    ui->containerWidget->setGeometry( (960 - ui->containerWidget->width()) / 2, (543 - height - 100) / 2,
                                      ui->containerWidget->width(), 30 + height + 30 + 50);
}

bool CommonDialog::pop()
{
    
    exec();
    return yesOrNO;
}

void CommonDialog::setText(QString text)
{
    ui->textLabel->setText(text);

    adaptSize();
}
