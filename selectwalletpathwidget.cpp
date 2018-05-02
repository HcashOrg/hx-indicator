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

void SelectWalletPathWidget::on_selectPathBtn_clicked()
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

void SelectWalletPathWidget::on_okBtn_clicked()
{
    qDebug() << "wallet path " << ui->pathLineEdit->text();
    UBChain::getInstance()->configFile->setValue("/settings/chainPath", ui->pathLineEdit->text());
    UBChain::getInstance()->appDataPath = ui->pathLineEdit->text();

    QString path = ui->pathLineEdit->text() + "/wallet.json";
    QFileInfo info(path);
    if(info.exists())
    {
        UBChain::getInstance()->startExe();
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
    painter.drawPixmap(238,130,490,320,QPixmap(":/ui/wallet_ui/login.png").scaled(490,320));

    QWidget::paintEvent(e);
}

void SelectWalletPathWidget::InitWidget()
{
    InitStyle();
    ui->pathLineEdit->setText( UBChain::getInstance()->appDataPath);
}

void SelectWalletPathWidget::InitStyle()
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

    ui->closeBtn->setIconSize(QSize(12,12));
    ui->closeBtn->setIcon(QIcon(":/ui/wallet_ui/close.png"));
    ui->closeBtn->setStyleSheet("QToolButton{background-color:transparent;border:none;}"
                                "QToolButton:hover{background-color:rgb(208,228,255);}");
    ui->okBtn->setStyleSheet("QToolButton{background-color:#5474EB; border:none;border-radius:15px;color: rgb(255, 255, 255);}"
                                "QToolButton:hover{background-color:#00D2FF;}");

    ui->selectPathBtn->setStyleSheet("QPushButton{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                                "QPushButton:hover{background-color:#00D2FF;}");

    ui->pathLineEdit->setStyleSheet("color:blue;background:transparent;border-width:0;border-style:outset;lineedit-password-mask-delay: 1000;\
                                    font-size:12pt;");
}


void SelectWalletPathWidget::on_closeBtn_clicked()
{
    emit closeWallet();
}

