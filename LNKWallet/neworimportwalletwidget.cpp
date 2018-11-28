#include "neworimportwalletwidget.h"
#include "ui_neworimportwalletwidget.h"

#include "wallet.h"

NewOrImportWalletWidget::NewOrImportWalletWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewOrImportWalletWidget)
{
    ui->setupUi(this);
    InitWidget();

}

NewOrImportWalletWidget::~NewOrImportWalletWidget()
{
    delete ui;
}

void NewOrImportWalletWidget::on_newWalletBtn_clicked()
{
    HXChain::getInstance()->startExe();
    emit enter();
}

void NewOrImportWalletWidget::on_importWalletBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select your wallet file."),"","(*.json)");
#ifdef WIN32
    file.replace("\\","/");
#endif

    if(file.isEmpty())      return;

    QDir dir(HXChain::getInstance()->appDataPath);
    if(!dir.exists())
    {
        qDebug() << dir.mkdir(HXChain::getInstance()->appDataPath);
    }

    qDebug() << "copy wallet.json to chaindata path : " << QFile::copy(file, HXChain::getInstance()->appDataPath + "/wallet.json");

    HXChain::getInstance()->importedWalletNeedToAddTrackAddresses = true;
    HXChain::getInstance()->configFile->setValue("/settings/importedWalletNeedToAddTrackAddresses",true);


    HXChain::getInstance()->startExe();
    emit enter();
}

void NewOrImportWalletWidget::on_closeBtn_clicked()
{
    emit closeWallet();
}

void NewOrImportWalletWidget::InitWidget()
{
    InitStyle();

    ui->label_version->setText(QString("v") + WALLET_VERSION);
}

void NewOrImportWalletWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/cover.png").scaled(this->size())));
    setPalette(palette);

    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(243,241,250));
    ui->label_version->setPalette(pa);

    ui->newWalletBtn->setStyleSheet("QToolButton{font: 12px \"Microsoft YaHei UI Light\";background-color:rgb(255,255,255); border:none;border-radius:3px;color: rgb(59, 22, 136);}" \
                                    );
    ui->importWalletBtn->setStyleSheet("QToolButton{font: 12px \"Microsoft YaHei UI Light\";background-color:rgb(255,255,255); border:none;border-radius:3px;color: rgb(59, 22, 136);}" \
                                       );

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/cover_close.png);background-repeat: no-repeat;background-position: center;border: none;}");
    //welcome
}

void NewOrImportWalletWidget::paintEvent(QPaintEvent *e)
{
//    QPainter painter(this);
//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QBrush(QColor(243,241,250)));
//    painter.drawRect(0,0,228,24);
//    painter.drawPixmap(7,5,32,12,QPixmap(":/ui/wallet_ui/hx_label_logo.png").scaled(32,12,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
//    painter.drawPixmap(94,38,36,36,QPixmap(":/ui/wallet_ui/logo_center.png").scaled(36,36,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QWidget::paintEvent(e);
}
