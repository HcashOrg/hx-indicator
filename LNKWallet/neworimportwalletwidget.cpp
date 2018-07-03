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
    UBChain::getInstance()->startExe();
    emit enter();
}

void NewOrImportWalletWidget::on_importWalletBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select your wallet file."),"","(*.json)");
#ifdef WIN32
    file.replace("\\","/");
#endif

    if(file.isEmpty())      return;

    QDir dir(UBChain::getInstance()->appDataPath);
    if(!dir.exists())
    {
        qDebug() << dir.mkdir(UBChain::getInstance()->appDataPath);
    }

    qDebug() << "copy wallet.json to chaindata path : " << QFile::copy(file, UBChain::getInstance()->appDataPath + "/wallet.json");

    UBChain::getInstance()->importedWalletNeedToAddTrackAddresses = true;
    UBChain::getInstance()->configFile->setValue("/settings/importedWalletNeedToAddTrackAddresses",true);


    UBChain::getInstance()->startExe();
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
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/background.png").scaled(this->size())));
    setPalette(palette);

    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_version->setPalette(pa);

    ui->newWalletBtn->setStyleSheet(BIG_BUTTON);
    ui->importWalletBtn->setStyleSheet("QToolButton{font: 11px \"Microsoft YaHei UI Light\";background-color:transparent; border:none;border-radius:15px;color: #5474EB;}" \
                                       "QToolButton:pressed{background-color:rgb(70,95,191);color:white;border-radius:15px;}");

    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);
}

void NewOrImportWalletWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(235,130,470,320,QPixmap(":/ui/wallet_ui/login.png").scaled(470,320));

    QWidget::paintEvent(e);
}
