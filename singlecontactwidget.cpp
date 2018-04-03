#include "singlecontactwidget.h"
#include "ui_singlecontactwidget.h"

#include "wallet.h"
#include "commondialog.h"
#include "control/contacteditdialog.h"

#include <QPainter>
#include <QDebug>

SingleContactWidget::SingleContactWidget(int num,QString address,QString remarkName,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleContactWidget)
{
    ui->setupUi(this);

    setStyleSheet("SingleContactWidget{background-color:white;border:1px solid white;border-radius:5px;}");


    ui->addressLabel->setText(address);
    ui->remarkLabel->setText(remarkName);
    number = num;

}

SingleContactWidget::~SingleContactWidget()
{
    delete ui;
}

void SingleContactWidget::on_transferBtn_clicked()
{
    mutexForAddressMap.lock();
    int size = UBChain::getInstance()->addressMap.size();
    mutexForAddressMap.unlock();

    if( size == 0)   // 还没有账户
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
//        commonDialog.setText(QString::fromLocal8Bit("没有可以转账的账户"));
        commonDialog.setText(tr("No account for transfering."));
        commonDialog.pop();
        return;
    }
    emit  gotoTransferPage(ui->addressLabel->text(),ui->remarkLabel->text());
}

void SingleContactWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(e);
}


void SingleContactWidget::on_chooseBtn_clicked()
{
    ContactEditDialog contactEditDialog(this);
    contactEditDialog.move( this->mapToGlobal(QPoint(ui->chooseBtn->x() + 6, ui->chooseBtn->y() + 6)));
    contactEditDialog.exec();

    if(contactEditDialog.choice == 1)
    {
        emit editContact( ui->addressLabel->text(), ui->remarkLabel->text());
    }
    else if(contactEditDialog.choice == 2)
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText(tr("Sure to delete this contact?"));

        bool yOrN = !commonDialog.pop();

        if( yOrN)
        {
            return;
        }

        if( !UBChain::getInstance()->contactsFile->open(QIODevice::ReadWrite))
        {
            qDebug() << "contact.dat not exist";
        }
        QByteArray rd = UBChain::getInstance()->contactsFile->readAll();
        QString str =  QByteArray::fromBase64( rd);
        QStringList strList = str.split(";");
        strList.removeLast();
        strList.removeAt(number);
        int size = strList.size();

        QString str2;
        for( int i = 0; i < size; i++)
        {
            str2 += strList.at(i) + ';';
        }
        QByteArray ba = str2.toUtf8();
        ba = ba.toBase64();
        UBChain::getInstance()->contactsFile->resize(0);
        QTextStream ts(UBChain::getInstance()->contactsFile);
        ts << ba;

        UBChain::getInstance()->contactsFile->close();

        emit deleteContact();

    }
}
