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
}

void NewOrImportWalletWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/background.png").scaled(this->size())));
    setPalette(palette);

    QFont font("黑体",14,70);
    ui->label_wel->setFont(font);

    QFont fontb("黑体",21,100);
    ui->label_bloc->setFont(fontb);

    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_wel->setPalette(pa);
    ui->label_bloc->setPalette(pa);
    ui->label_version->setPalette(pa);

    ui->newWalletBtn->setStyleSheet("QPushButton{background-color:#5474EB; border:none;border-radius:15px;color: rgb(255, 255, 255);font-family:\"Microsoft YaHei UI Light\";}"
                                "QPushButton:hover{background-color:#00D2FF;}");
    ui->importWalletBtn->setStyleSheet("QPushButton{background-color:transparent;border:none;border-radius:15px;color: #5474EB;font-family:\"Microsoft YaHei UI Light\";}"
                                 "QPushButton:hover{background-color:#00D2FF;color: rgb(255, 255, 255);}");


    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);
}

void NewOrImportWalletWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(235,130,490,320,QPixmap(":/ui/wallet_ui/login.png").scaled(490,320));

    QWidget::paintEvent(e);
}
