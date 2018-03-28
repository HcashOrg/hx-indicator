#include "neworimportwalletwidget.h"
#include "ui_neworimportwalletwidget.h"

#include "lnk.h"

#include <QFileDialog>
#include <QPainter>

NewOrImportWalletWidget::NewOrImportWalletWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewOrImportWalletWidget)
{
    ui->setupUi(this);

    setStyleSheet("NewOrImportWalletWidget{background-image:url(:/ui/wallet_ui/loginBg.png);}");

}

NewOrImportWalletWidget::~NewOrImportWalletWidget()
{
    delete ui;
}

void NewOrImportWalletWidget::on_newWalletBtn_clicked()
{
    UBChain::getInstance()->startExe();
    emit enter();
}

void NewOrImportWalletWidget::on_importWalletBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose your wallet file."),"","(*.json)");
#ifdef WIN32
    file.replace("\\","/");
#endif

    if(file.isEmpty())      return;
    ui->walletFilePathLabel->setText(file);

    qDebug() << "copy wallet.json to chaindata path : " << QFile::copy(file, UBChain::getInstance()->appDataPath + "/wallet.json");

    UBChain::getInstance()->startExe();
    emit enter();
}

void NewOrImportWalletWidget::on_closeBtn_clicked()
{
    emit closeWallet();
}

void NewOrImportWalletWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(e);
}
