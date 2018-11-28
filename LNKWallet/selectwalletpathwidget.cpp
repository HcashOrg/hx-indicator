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
//    QPainter painter(this);
//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QBrush(QColor(243,241,250)));
//    painter.drawRect(0,0,228,24);
//    painter.drawPixmap(7,5,32,12,QPixmap(":/ui/wallet_ui/hx_label_logo.png").scaled(32,12,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
//    painter.drawPixmap(94,38,36,36,QPixmap(":/ui/wallet_ui/logo_center.png").scaled(36,36,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

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
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/cover.png").scaled(this->size())));
    setPalette(palette);


    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(243,241,250));
    ui->label_version->setPalette(pa);

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/cover_close.png);background-repeat: no-repeat;background-position: center;border: none;}");
    ui->okBtn->setStyleSheet("QToolButton{font: 12px \"Microsoft YaHei UI Light\";background-color:rgb(255,255,255); border:none;border-radius:3px;color: rgb(59, 22, 136);}" \
                             );


    ui->pathLineEdit->setStyleSheet("QLineEdit{max-height:32px;background: transparent;color: rgb(243,241,250);font: 12px \"Microsoft YaHei UI Light\";border:none;border-radius:0px;border-bottom:1px solid rgb(243,241,250);padding: 0px 10px 0px 6px;}"
                                    "QLineEdit:disabled{color: rgb(151,151,151);}");
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
