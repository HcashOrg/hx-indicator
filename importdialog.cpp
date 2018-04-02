#include "importdialog.h"
#include "ui_importdialog.h"
#include "wallet.h"
#include "commondialog.h"
#include "namedialog.h"
#include "control/shadowwidget.h"
#include "dialog/importenterpwddialog.h"
#include "AES/aesencryptor.h"

#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QFile>

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

//    UBChain::getInstance()->appendCurrentDialogVector(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");

    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->importBtn->setEnabled(false);

    ui->importBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->privateKeyLineEdit->setFocus();

    shadowWidget = new ShadowWidget(this);
    shadowWidget->init(this->size());
    shadowWidget->hide();
}

ImportDialog::~ImportDialog()
{
    delete ui;
//    UBChain::getInstance()->removeCurrentDialogVector(this);
}

void ImportDialog::pop()
{
//    QEventLoop loop;
//    show();
//    ui->privateKeyLineEdit->grabKeyboard();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();
}

void ImportDialog::on_cancelBtn_clicked()
{
    close();
//    qDebug() << "on_cancelBtn_clicked";
//    emit accepted();
}

void ImportDialog::on_pathBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose your private key file."),"","(*.key *.upk)");
#ifdef WIN32
    file.replace("\\","/");
#endif	
    ui->privateKeyLineEdit->setText(file);
}

void ImportDialog::on_importBtn_clicked()
{
    // 如果输入框中是 私钥文件的地址
    if( ui->privateKeyLineEdit->text().endsWith(".key") )
    {
        QFile file( ui->privateKeyLineEdit->text());
        if( file.open(QIODevice::ReadOnly))
        {
            QByteArray rd = file.readAll();
            QByteArray ba = QByteArray::fromBase64( rd);
            QString str(ba);
            privateKey = str;

            UBChain::getInstance()->postRPC( "id_wallet_import_private_key", toJsonFormat( "wallet_import_private_key", QStringList() << str));
            ui->importBtn->setEnabled(false);
            shadowWidget->show();

            file.close();
            return;
        }
        else
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Wrong file path."));
            commonDialog.pop();
            return;
        }

    }
    else if( ui->privateKeyLineEdit->text().endsWith(".upk") )      // 如果是加密后的私钥文件
    {
        ImportEnterPwdDialog importEnterPwdDialog;
        if(importEnterPwdDialog.pop())
        {
            QFile file( ui->privateKeyLineEdit->text());
            if( file.open(QIODevice::ReadOnly))
            {
                QByteArray rd = file.readAll();
                QString str(rd);

                QString pwd = importEnterPwdDialog.pwd;
                unsigned char key2[16] = {0};
                memcpy(key2,pwd.toLatin1().data(),pwd.toLatin1().size());
                AesEncryptor aes(key2);
                QString output = QString::fromStdString( aes.DecryptString( str.toStdString()) );

                if( output.startsWith("privateKey="))
                {
                    file.close();

                    QString pk = output.mid( QString("privateKey=").size());
                    privateKey = pk;

                    UBChain::getInstance()->postRPC( "id_wallet_import_private_key", toJsonFormat( "wallet_import_private_key", QStringList() << pk));
                    ui->importBtn->setEnabled(false);
                    shadowWidget->show();
                }
                else
                {
                    file.close();

                    CommonDialog commonDialog(CommonDialog::OkOnly);
                    commonDialog.setText( tr("Wrong password!"));
                    commonDialog.pop();
                    return;
                }

            }
            else
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("Wrong file path."));
                commonDialog.pop();
                return;
            }
        }
        else
        {
            return;
        }

    }
    else // 如果输入框中是 私钥
    {
        QString privateKey = ui->privateKeyLineEdit->text();
        privateKey = privateKey.simplified();
        if( (privateKey.size() == 51 || privateKey.size() == 52) && (privateKey.startsWith("5") || privateKey.startsWith("KL")) )
        {
            UBChain::getInstance()->postRPC( "id_wallet_import_private_key", toJsonFormat( "wallet_import_private_key", QStringList() << privateKey));

             ui->importBtn->setEnabled(false);
             shadowWidget->show();
        }
        else
        {
            // 如果不是未注册账户 是错误的私钥格式
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Illegal private key!"));
            commonDialog.pop();
        }

    }

}

QString toThousandFigure( int);

void ImportDialog::jsonDataUpdated(QString id)
{
    if( id == "id_wallet_import_private_key")
    {
        shadowWidget->hide();

        QString result = UBChain::getInstance()->jsonDataValue(id);
        if( result.mid(0,9) == "\"result\":")
        {
            ui->importBtn->setEnabled(true);

            UBChain::getInstance()->needToScan = true;  // 导入成功，需要scan

            QString name = result.mid(10, result.indexOf("\"", 10) - 10);
            if( accountNameAlreadyExisted(name))        //  如果已存在该账号
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( name + tr(" already existed") );
                commonDialog.pop();

            }
            else
            {
                mutexForAddressMap.lock();
                int size = UBChain::getInstance()->addressMap.size();
                mutexForAddressMap.unlock();

                mutexForConfigFile.lock();
                UBChain::getInstance()->configFile->setValue( QString("/accountInfo/") + QString::fromLocal8Bit("账户") + toThousandFigure(size+1),name);
                mutexForConfigFile.unlock();
                UBChain::getInstance()->balanceMapInsert( name, "0.00000 " + QString(ASSET_NAME));
                UBChain::getInstance()->registerMapInsert( name, "UNKNOWN");
                UBChain::getInstance()->addressMapInsert( name, UBChain::getInstance()->getAddress(name));

                emit accountImported();

                UBChain::getInstance()->setRecollect();

                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( name + tr( " has been imported!"));
                commonDialog.pop();

//                RpcThread* rpcThread = new RpcThread;
//                connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//                rpcThread->setLogin("a","b");
//                rpcThread->setWriteData( toJsonFormat( "id_scan", "scan", QStringList() << "0" ));
//                rpcThread->setWriteData( toJsonFormat( "id_wallet_scan_transaction", "wallet_scan_transaction", QStringList() << "ALL" << "true" ));
//                rpcThread->start();

                UBChain::getInstance()->postRPC( "id_scan", toJsonFormat( "scan", QStringList() << "0" ));
                UBChain::getInstance()->postRPC( "id_wallet_scan_transaction", toJsonFormat( "wallet_scan_transaction", QStringList() << "ALL" << "true" ));


                close();
//                emit accepted();
            }

        }
        else if( result.mid(0,8) == "\"error\":")
        {
            qDebug() << "import error: " << result;

            if( !result.contains("Unknown key! You must specify an account name!") )
            {
                // 如果不是未注册账户 是错误的私钥格式
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("Illegal private key!"));
                commonDialog.pop();

                ui->importBtn->setEnabled(true);
                return;
            }

            NameDialog nameDialog;
            QString name = nameDialog.pop();

            if( name.isEmpty())    // 如果取消 命名
            {
                ui->importBtn->setEnabled(true);
                return;
            }

            if( ui->privateKeyLineEdit->text().endsWith(".key") || ui->privateKeyLineEdit->text().endsWith(".upk") )  // 如果是路径
            {
                UBChain::getInstance()->postRPC( "id_wallet_import_private_key_" + name, toJsonFormat( "wallet_import_private_key", QStringList() << privateKey << name << "true"));
                shadowWidget->show();
                return;
            }
            else // 如果输入框中是 私钥
            {
                UBChain::getInstance()->postRPC( "id_wallet_import_private_key_" + name, toJsonFormat( "wallet_import_private_key", QStringList() << ui->privateKeyLineEdit->text() << name << "true" ));
                shadowWidget->show();
            }
        }

        return;
    }

    if( id.startsWith("id_wallet_import_private_key_") )
    {
        ui->importBtn->setEnabled(true);
        shadowWidget->hide();

        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.mid(0,9) == "\"result\":")
        {
            UBChain::getInstance()->needToScan = true;  // 导入成功，需要scan

            QString name = result.mid(10, result.indexOf("\"", 10) - 10);

            mutexForAddressMap.lock();
            int size = UBChain::getInstance()->addressMap.size();
            mutexForAddressMap.unlock();

            mutexForConfigFile.lock();
            UBChain::getInstance()->configFile->setValue( QString("/accountInfo/") + QString::fromLocal8Bit("账户") + toThousandFigure(size+1),name);
            mutexForConfigFile.unlock();
            UBChain::getInstance()->balanceMapInsert( name, "0.00000 " + QString(ASSET_NAME));
            UBChain::getInstance()->registerMapInsert( name, "NO");
            UBChain::getInstance()->addressMapInsert( name, UBChain::getInstance()->getAddress(name));

            emit accountImported();

            UBChain::getInstance()->setRecollect();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( name + tr(" has been imported!") );
            commonDialog.pop();

            UBChain::getInstance()->postRPC( "id_scan", toJsonFormat( "scan", QStringList() << "0" ));
            UBChain::getInstance()->postRPC( "id_wallet_scan_transaction", toJsonFormat( "wallet_scan_transaction", QStringList() << "ALL" << "true" ));


            close();
//            emit accepted();
        }
        else if( result.mid(0,8) == "\"error\":")
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Illegal private key!"));
            commonDialog.pop();
        }

        return;
    }

}

bool ImportDialog::accountNameAlreadyExisted(QString name)
{
    mutexForConfigFile.lock();
    UBChain::getInstance()->configFile->beginGroup("/accountInfo");

    QStringList keys = UBChain::getInstance()->configFile->childKeys();
    QString str;
    bool existed = false;
    foreach (str, keys)
    {
        if( UBChain::getInstance()->configFile->value(str) == name)
        {
            existed = true;
            break;
        }
    }
    UBChain::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();
    return existed;

}

void ImportDialog::on_privateKeyLineEdit_textChanged(const QString &arg1)
{
    QString str = arg1.simplified();
    if( str.isEmpty())
    {
        ui->importBtn->setEnabled(false);
    }
    else
    {
        ui->importBtn->setEnabled(true);
    }
}

void ImportDialog::on_privateKeyLineEdit_returnPressed()
{
    if( ui->importBtn->isEnabled())
    {
        on_importBtn_clicked();
    }
}

void ImportDialog::on_closeBtn_clicked()
{
    on_cancelBtn_clicked();
}
