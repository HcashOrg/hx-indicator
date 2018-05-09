#include "backupwalletdialog.h"
#include "ui_backupwalletdialog.h"

#include "wallet.h"
#include "commondialog.h"


BackupWalletDialog::BackupWalletDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BackupWalletDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color:rgb(255,255,255);border-radius:10px;}");

    ui->backupBtn->setStyleSheet("QToolButton#backupBtn{border:none;color:white;border-radius:10px;font-size:12pt;background-color:#5474EB;}"
                             "QToolButton#backupBtn::hover{background-color:#00D2FF;}");
    ui->cancelBtn->setStyleSheet("QToolButton#cancelBtn{border:none;color:white;border-radius:10px;font-size:12pt;background-color:#E5E5E5;}"
                                 "QToolButton#cancelBtn::hover{background-color:#00D2FF;}");

    ui->closeBtn->setIconSize(QSize(12,12));
    ui->closeBtn->setIcon(QIcon(":/ui/wallet_ui/close.png"));
    ui->closeBtn->setStyleSheet("QToolButton{background-color:transparent;border:none;}"
                                "QToolButton:hover{background-color:rgb(208,228,255);}");

    ui->pathBtn->setStyleSheet("QToolButton#pathBtn{border:none;color:gray;border-radius:10px;font-size:9pt;background-color:transparent;}");
    ui->pathLineEdit->setStyleSheet("QLineEdit{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;}");
}

BackupWalletDialog::~BackupWalletDialog()
{
    delete ui;
}

void BackupWalletDialog::pop()
{
    move(0,0);
    exec();
}

void BackupWalletDialog::on_backupBtn_clicked()
{
    if(ui->pathLineEdit->text().isEmpty())      return;

    if(  ui->pathLineEdit->text().endsWith(".json") )
    {
        QString path = ui->pathLineEdit->text();

        QFile file(UBChain::getInstance()->appDataPath + "/wallet.json");
        if(file.exists())
        {
            qDebug() << "backup wallet.json " << file.copy(path);

            close();

            CommonDialog tipDialog(CommonDialog::OkOnly);
            tipDialog.setText( tr("Your wallet has been backed up! Please keep it properly.Never lose or leak it to anyone!") );
            tipDialog.pop();

            QString dirPath = path;
#ifdef WIN32
            dirPath.replace( "/", "\\");
            dirPath = dirPath.left( dirPath.lastIndexOf("\\") );

            QProcess::startDetached("explorer \"" + dirPath + "\"");
#else
            dirPath = dirPath.left( dirPath.lastIndexOf("/") );
            QProcess::startDetached("open \"" + dirPath + "\"");
#endif
        }
        else
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Can not find the wallet file!"));
            commonDialog.pop();
        }
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Wrong file format!"));
        commonDialog.pop();

        return;
    }
}

void BackupWalletDialog::on_cancelBtn_clicked()
{
    CommonDialog commonDialog(CommonDialog::OkAndCancel);
    commonDialog.setText(tr("You have not backup this wallet.If your wallet data is lost or corrupted, you will have no way to get your accounts back.Sure to cancel?"));
    if(commonDialog.pop())
    {
        close();
    }
}

void BackupWalletDialog::on_pathBtn_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this,tr( "Select the path"));
    if( !file.isEmpty())
    {
#ifdef WIN32
        file.replace("\\","/");
#endif
        QDateTime currentTime =QDateTime::currentDateTime();
        QString str = currentTime.toString("_yyyy.MM.dd_hh.mm.ss");
        ui->pathLineEdit->setText( file + "/wallet"  + str + ".json");
    }
}

void BackupWalletDialog::on_closeBtn_clicked()
{
    on_cancelBtn_clicked();
}

void BackupWalletDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(10,10,10,100));//最后一位是设置透明属性（在0-255取值）
    painter.drawRect(QRect(0,0,960,580));

    QWidget::paintEvent(event);
}
