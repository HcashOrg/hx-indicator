#include "importdialog.h"
#include "ui_importdialog.h"
#include "wallet.h"
#include "commondialog.h"
#include "namedialog.h"
#include "control/shadowwidget.h"
#include "dialog/importenterpwddialog.h"
#include "AES/aesencryptor.h"
#include "KeyDataUtil.h"

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    ui->privateKeyLineEdit->setEnabled(false);
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

    ui->accountNameLineEdit->setFocus();

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
    QString file = QFileDialog::getOpenFileName(this, tr("Choose your private key file."),"","(*.lpk *.elpk)");
#ifdef WIN32
    file.replace("\\","/");
#endif	
    ui->privateKeyLineEdit->setText(file);
}

void ImportDialog::on_importBtn_clicked()
{
    if(ui->accountNameLineEdit->text().isEmpty())   return;
    if(ui->privateKeyLineEdit->text().isEmpty())    return;

    KeyDataPtr data = std::make_shared<KeyDataInfo>();
    if(KeyDataUtil::ReadaPrivateKeyFromPath(ui->privateKeyLineEdit->text(),data))
    {
        //解密
        if(ui->privateKeyLineEdit->text().endsWith(".lpk"))
        {
            data->DecryptBase64();
        }
        else if(ui->privateKeyLineEdit->text().endsWith(".elpk"))
        {
            ImportEnterPwdDialog importEnterPwdDialog;
            if(importEnterPwdDialog.pop())
            {
                data->DecryptAES(importEnterPwdDialog.pwd);
            }
        }
        else
        {
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

    //验证该地址是否已经存在
    foreach(AccountInfo info,UBChain::getInstance()->accountInfoMap){
        qDebug()<<info.address << " "<<data->LNKAddr;
        if(info.address == data->LNKAddr)
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("LNK Address:%1  Already Exists!").arg(data->LNKAddr));
            commonDialog.pop();
            return;
        }
    }

    //开始导入私钥
    for(auto it = data->info_key.begin();it != data->info_key.end();++it)
    {
        if((*it).first == "LNK")
        {
            //导入lnk私钥
            UBChain::getInstance()->postRPC( "import-import_key",
                                             toJsonFormat( "import_key", QJsonArray() << ui->accountNameLineEdit->text() << (*it).second));
        }
        else
        {
            //导入跨连私钥
            UBChain::getInstance()->postRPC( "import-import_crosschain_key",
                                             toJsonFormat( "import_crosschain_key", QJsonArray() << (*it).second <<(*it).first));
        }
    }
    UBChain::getInstance()->postRPC( "import-finish_import_key", toJsonFormat( "finish_import_key", QJsonArray()));

    // 如果输入框中是 私钥文件的地址
//    if( ui->privateKeyLineEdit->text().endsWith(".lpk") )
//    {
//        QFile file( ui->privateKeyLineEdit->text());
//        if( file.open(QIODevice::ReadOnly))
//        {
//            QByteArray rd = file.readAll();
//            QByteArray ba = QByteArray::fromBase64( rd);
//            QString str(ba);

//            UBChain::getInstance()->postRPC( "id-import_key", toJsonFormat( "import_key", QJsonArray() << ui->accountNameLineEdit->text() << str));
//            ui->importBtn->setEnabled(false);
//            shadowWidget->show();

//            file.close();
//            return;
//        }
//        else
//        {
//            CommonDialog commonDialog(CommonDialog::OkOnly);
//            commonDialog.setText( tr("Wrong file path."));
//            commonDialog.pop();
//            return;
//        }

//    }
//    else if( ui->privateKeyLineEdit->text().endsWith(".elpk") )      // 如果是加密后的私钥文件
//    {
//        ImportEnterPwdDialog importEnterPwdDialog;
//        if(importEnterPwdDialog.pop())
//        {
//            QFile file( ui->privateKeyLineEdit->text());
//            if( file.open(QIODevice::ReadOnly))
//            {
//                QByteArray rd = file.readAll();
//                QString str(rd);

//                QString pwd = importEnterPwdDialog.pwd;
//                unsigned char key2[16] = {0};
//                memcpy(key2,pwd.toLatin1().data(),pwd.toLatin1().size());
//                AesEncryptor aes(key2);
//                QString output = QString::fromStdString( aes.DecryptString( str.toStdString()) );

//                if( output.startsWith("privateKey="))
//                {
//                    file.close();

//                    QString pk = output.mid( QString("privateKey=").size());

//                    UBChain::getInstance()->postRPC( "id-import_key", toJsonFormat( "import_key", QJsonArray() << ui->accountNameLineEdit->text() << pk));
//                    ui->importBtn->setEnabled(false);
//                    shadowWidget->show();
//                }
//                else
//                {
//                    file.close();

//                    CommonDialog commonDialog(CommonDialog::OkOnly);
//                    commonDialog.setText( tr("Wrong password!"));
//                    commonDialog.pop();
//                    return;
//                }

//            }
//            else
//            {
//                CommonDialog commonDialog(CommonDialog::OkOnly);
//                commonDialog.setText( tr("Wrong file path."));
//                commonDialog.pop();
//                return;
//            }
//        }
//        else
//        {
//            return;
//        }

//    }
//    else // 如果输入框中是 私钥
//    {
//        QString privateKey = ui->privateKeyLineEdit->text();
//        privateKey = privateKey.simplified();
//        if( (privateKey.size() == 51 || privateKey.size() == 52) && (privateKey.startsWith("5") ) )
//        {
//            UBChain::getInstance()->postRPC( "id-import_key", toJsonFormat( "import_key", QJsonArray() << ui->accountNameLineEdit->text() << privateKey));

//             ui->importBtn->setEnabled(false);
//             shadowWidget->show();
//        }
//        else
//        {
//            // 如果不是未注册账户 是错误的私钥格式
//            CommonDialog commonDialog(CommonDialog::OkOnly);
//            commonDialog.setText( tr("Illegal private key!"));
//            commonDialog.pop();
//        }

//    }

}

QString toThousandFigure( int);

void ImportDialog::jsonDataUpdated(QString id)
{
    if( id == "import-import_key")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
qDebug()  << id << result;
        if( result == "\"result\":true")
        {
//            ui->importBtn->setEnabled(true);

//            emit accountImported();

            UBChain::getInstance()->postRPC( "id-get_account-ImportDialog-" + ui->accountNameLineEdit->text(), toJsonFormat( "get_account", QJsonArray() << ui->accountNameLineEdit->text() ));

        }
        else if( result.mid(0,8) == "\"error\":")
        {
            qDebug() << "import error: " << result;
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Import key error: ") + errorMessage);
            commonDialog.pop();

            ui->importBtn->setEnabled(true);
            return;
        }

        return;
    }

    if( id == "id-get_account-ImportDialog-" + ui->accountNameLineEdit->text())
    {
        shadowWidget->hide();

        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug()  << id << result;

        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QString accountAddress = jsonObject.take("result").toObject().take("addr").toString();

        UBChain::getInstance()->addTrackAddress(accountAddress);

        UBChain::getInstance()->resyncNextTime = true;
        UBChain::getInstance()->configFile->setValue("/settings/resyncNextTime", true);

        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText( ui->accountNameLineEdit->text() + tr( " has been imported!"));
        commonDialog.pop();

        CommonDialog commonDialog2(CommonDialog::OkOnly);
        commonDialog2.setText( tr("Everytime a new account is imported, the wallet will rescan the blockchain data when launched next time."
                                  " After that the transactions of the account will be shown.") );
        commonDialog2.pop();

        close();
        return;
    }

    if( id == "import-finish_import_key")
    {
        UBChain::getInstance()->autoSaveWalletFile();

        return;
    }
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

void ImportDialog::on_closeBtn_clicked()
{
    on_cancelBtn_clicked();
}

