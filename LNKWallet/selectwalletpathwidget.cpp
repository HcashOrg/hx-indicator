#include "selectwalletpathwidget.h"
#include "ui_selectwalletpathwidget.h"
#include "wallet.h"
#include "commondialog.h"

#include <QDesktopServices>

SelectWalletPathWidget::SelectWalletPathWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectWalletPathWidget)
{
    ui->setupUi(this);

#ifdef TARGET_OS_MAC
    ui->pwdLineEdit->setAttribute(Qt::WA_MacShowFocusRect,false);
#endif


    InitWidget();
}

SelectWalletPathWidget::~SelectWalletPathWidget()
{
    delete ui;
}

void SelectWalletPathWidget::on_okBtn_clicked()
{
    qDebug() << "wallet path " << ui->pathLineEdit->text();
    HXChain::getInstance()->configFile->setValue("/settings/chainPath", ui->pathLineEdit->text());
    HXChain::getInstance()->appDataPath = ui->pathLineEdit->text();

    QString path = ui->pathLineEdit->text() + "/wallet.json";
    QFileInfo info(path);
    if(info.exists())
    {
        HXChain::getInstance()->startExe();
        emit enter();
    }
    else
    {
        emit newOrImportWallet();
    }

}

void SelectWalletPathWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(243,241,250)));
    painter.drawRect(0,0,228,24);
    painter.drawPixmap(7,5,32,12,QPixmap(":/ui/wallet_ui/hx_label_logo.png").scaled(32,12,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    painter.drawPixmap(94,38,36,36,QPixmap(":/ui/wallet_ui/logo_center.png").scaled(36,36,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QWidget::paintEvent(e);
}

void SelectWalletPathWidget::InitWidget()
{
    InitStyle();

    ui->pathLineEdit->setText( HXChain::getInstance()->appDataPath);

    ui->label_version->setText(QString("v") + WALLET_VERSION);
}

void SelectWalletPathWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/login_back.png").scaled(this->size())));
    setPalette(palette);


    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(243,241,250));
    ui->label_version->setPalette(pa);

    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);
    ui->okBtn->setStyleSheet(CANCELBTN_STYLE);

    //welcome
    ui->welcome->setPixmap(QPixmap(":/ui/wallet_ui/Welcome.png").scaled(ui->welcome->width(), ui->welcome->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

}


void SelectWalletPathWidget::on_closeBtn_clicked()
{
    emit closeWallet();
}


void SelectWalletPathWidget::on_pathBtn_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, "Select the path to store the blockchain");
    if( !file.isEmpty())
    {
#ifdef WIN32
        file.replace("/","\\");
#endif
        ui->pathLineEdit->setText( file);
    }
}
