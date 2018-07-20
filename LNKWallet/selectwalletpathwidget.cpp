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
    painter.drawPixmap(238,130,470,320,QPixmap(":/ui/wallet_ui/login.png").scaled(470,320));

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
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/background.png").scaled(this->size())));
    setPalette(palette);


    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_version->setPalette(pa);

    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);
    ui->okBtn->setStyleSheet(BIG_BUTTON);


    ui->pathLineEdit->setStyleSheet("color:blue;background:transparent;border-width:0;border-style:outset;lineedit-password-mask-delay: 1000;\
                                    font-size:12pt;");
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
