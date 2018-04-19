#include "selectwalletpathwidget.h"
#include "ui_selectwalletpathwidget.h"
#include "wallet.h"
#include "commondialog.h"

#include <QDesktopServices>

SelectWalletPathWidget::SelectWalletPathWidget(QWidget *parent) :
    QWidget(parent),
    updateExe(NULL),
    reply(NULL),
    ui(new Ui::SelectWalletPathWidget)
{
    ui->setupUi(this);

#ifdef TARGET_OS_MAC
    ui->pwdLineEdit->setAttribute(Qt::WA_MacShowFocusRect,false);
#endif


    InitWidget();
//    getUpdateXml();
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

//    UBChain::getInstance()->proc->start(WALLET_EXE_FILE,QJsonArray() << "--data-dir" << ui->pathLineEdit->text()
//                                       << "--rpcuser" << "a" << "--rpcpassword" << "b" << "--rpcport" << QString::number( RPC_PORT) << "--server" );

//    if( UBChain::getInstance()->proc->waitForStarted())
//    {
//        qDebug() << QString("launch %1 successfully").arg(WALLET_EXE_FILE);

//        mutexForConfigFile.lock();
//        UBChain::getInstance()->configFile->setValue("/settings/chainPath", ui->pathLineEdit->text());
//        mutexForConfigFile.unlock();
//        UBChain::getInstance()->appDataPath = ui->pathLineEdit->text();
//        emit enter();
//    }
//    else
//    {
//        qDebug() << QString("launch %1 failed").arg(WALLET_EXE_FILE);
//    }
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

void SelectWalletPathWidget::getUpdateXml()
{
    QFile updateFile("update.xml");

    if (!updateFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "update.xml not exist.";
        updateFile.close();
        return;
    }
    else
    {
        if( !localXml.setContent(&updateFile))
        {
            qDebug() << "localxml setcontent false";
            updateFile.close();
            return;
        }

        updateFile.close();
    }

    QString serverPath = getXmlElementText( localXml, "UpdateServer");
qDebug() << "serverPath : " << serverPath;
    reply = qnam.get( QNetworkRequest(QUrl(serverPath + "/update.xml")));
    connect(reply, SIGNAL(finished()),this, SLOT(httpFinished()));
}

void SelectWalletPathWidget::httpFinished()
{
    if( !serverXml.setContent( reply->readAll()) )
    {
        qDebug() << "get serverXml error.";
        return;
    }

    if( compareVersion( getXmlElementText( localXml, "Version"), getXmlElementText( serverXml, "Version")) != -1)
    {
        qDebug() << "Update.xml already newest.";
        return;
    }
    else
    {
        // 由于3.0.4的uac问题 Update条目改为小写的u
        if( compareVersion( getXmlElementText( localXml, "update"), getXmlElementText( serverXml, "update")) == -1)
        {
            // 如果GOPWalletUpdate2.exe 需要更新  由于3.0.4的uac问题 改为GOPWalletUpdate2.exe
            updateExe = new QFile("GOPWalletUpdate2.exe.update");
            if (!updateExe->open(QIODevice::WriteOnly|QIODevice::Truncate))
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("New GOPWalletUpdate2.exe error: %1").arg(updateExe->errorString()) );
                commonDialog.pop();
                delete updateExe;
                updateExe = NULL;
                return;
            }

            QString serverPath = getXmlElementText( localXml, "UpdateServer");
            reply = qnam.get( QNetworkRequest(QUrl(serverPath + "/GOPWalletUpdate2.exe")));
            connect(reply, SIGNAL(finished()),
                    this, SLOT(getUpdateExeFinished()));
            connect(reply, SIGNAL(readyRead()),
                    this, SLOT(getUpdateExeReadyRead()));

            updateOrNot = true;  // 下完 GOPWalletUpdate2.exe 前不进入钱包

        }
        else
        {
            // 如果GOPWalletUpdate2.exe 不需要更新

            if( compareVersion( getXmlElementText( localXml, "Version"), getXmlElementText( serverXml, "LastAvailableVersion")) == -1)
            {
                // 如果版本低于最后可运行版本 强制更新
          qDebug() << "Force updating:  " << QDesktopServices::openUrl(QUrl::fromLocalFile("GOPWalletUpdate2.exe"));
//                qDebug() << "Force updating: " << QProcess::startDetached("GOPWalletUpdate2.exe");
                emit closeWallet();
            }
            else
            {
                // 否则选择是否更新
                updateOrNot = true;
                CommonDialog commonDialog(CommonDialog::OkAndCancel);
                commonDialog.setText( tr("New version detected,\nupdate or not?"));
                bool yesOrNo = commonDialog.pop();
                if( yesOrNo)
                {
                    qDebug() << "start update " << QDesktopServices::openUrl(QUrl::fromLocalFile("GOPWalletUpdate2.exe"));

//                    qDebug() << "start update " << QProcess::startDetached("GOPWalletUpdate2.exe");
                    emit closeWallet();
                }
                else
                {
                    updateOrNot = false;
                }

            }

        }

    }


}

QString SelectWalletPathWidget::getXmlElementText(QDomDocument& doc, QString name)
{
    QDomElement docElem = doc.documentElement();  //返回根元素
    QDomNode n = docElem.firstChild();   //返回根节点的第一个子节点
    while(!n.isNull())   //如果节点不为空
    {
        if (n.isElement())  //如果节点是元素
        {
            QDomElement e = n.toElement();  //将其转换为元素

            if( e.tagName() == name)
            {
                return e.text();
            }
        }
        n = n.nextSibling();  //下一个兄弟节点
    }

    return "";
}

void SelectWalletPathWidget::getUpdateExeFinished()
{
    updateExe->flush();
    updateExe->close();

    if (reply->error() != QNetworkReply::NoError)
    {
        // 如果 error
        qDebug() << "getUpdateExe error: " << reply->errorString();
        updateExe->remove();
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText( tr("Get GOPWalletUpdate2.exe error: %1").arg(reply->errorString()) );
        commonDialog.pop();
    }
    else
    {
        // 下载完成后 删除原来的文件 重命名临时文件
        QString fileName = updateExe->fileName();
        QString oldName = fileName;
        oldName.remove(".update");

        qDebug() << "remove exe" << oldName <<  QFile::remove(oldName);
        qDebug() << "rename exe" << QFile::rename( fileName, oldName);

    }

    delete updateExe;
    updateExe = NULL;

    if( compareVersion( getXmlElementText( localXml, "Version"), getXmlElementText( serverXml, "LastAvailableVersion")) == -1)
    {
        // 如果版本低于最后可运行版本 强制更新
        qDebug() << "Force updating:  " << QDesktopServices::openUrl(QUrl::fromLocalFile("GOPWalletUpdate2.exe"));
        emit closeWallet();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText( tr("New version detected,\nupdate or not?"));
        bool yesOrNo = commonDialog.pop();
        if( yesOrNo)
        {
            qDebug() << "start update " << QDesktopServices::openUrl(QUrl::fromLocalFile("GOPWalletUpdate2.exe"));
            emit closeWallet();
        }
        else
        {
            updateOrNot = false;
        }
    }


}

void SelectWalletPathWidget::getUpdateExeReadyRead()
{
    if (updateExe)
        updateExe->write(reply->readAll());
}
