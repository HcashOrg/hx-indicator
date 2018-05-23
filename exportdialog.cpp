#include "exportdialog.h"
#include "ui_exportdialog.h"
#include "wallet.h"
#include "commondialog.h"
#include "dialog/exportsetpwddialog.h"
#include "AES/aesencryptor.h"


ExportDialog::ExportDialog( QString name, QWidget *parent) :
    QDialog(parent),
    accoutName(name),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);
    dataInfo = std::make_shared<KeyDataInfo>();

//    UBChain::getInstance()->appendCurrentDialogVector(this);
    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color:rgb(255,255,255);border-radius:10px;}");

    ui->exportBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->pathBtn->setStyleSheet(SELECTBTN_STYLE);
    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));


    ui->nameLabel->setText( tr("Export ") + accoutName + tr(" \'s private key to:"));

    ui->encryptCheckBox->setChecked(true);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::pop()
{
    move(0,0);
    exec();
}

void ExportDialog::on_pathBtn_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this,tr( "Select the path"));
    if( !file.isEmpty())
    {
#ifdef WIN32
        file.replace("\\","/");
#endif
        if(ui->encryptCheckBox->isChecked())
        {
            ui->pathLineEdit->setText( file + "/"  + accoutName + ".elpk");
        }
        else
        {
            ui->pathLineEdit->setText( file + "/"  + accoutName + ".lpk");
        }

    }
}

void ExportDialog::on_cancelBtn_clicked()
{
    CommonDialog commonDialog(CommonDialog::OkAndCancel);
//    commonDialog.setText(QString::fromLocal8Bit("您还没有备份私钥，如果数据丢失/损坏 您将没有任何途径找回您的账户。是否确定取消备份?"));
    commonDialog.setText(tr("You have not backup this account.If your wallet data is lost or corrupted, you will have no way to get your account back.Sure to cancel?"));
    if(commonDialog.pop())
    {
        close();
    }

}

void ExportDialog::getPrivateKey()
{
    UBChain::getInstance()->postRPC( "id-dump_private_key-" + accoutName, toJsonFormat( "dump_private_key", QJsonArray() << accoutName << "0" ));

    //开始查找私钥
    UBChain::getInstance()->postRPC( "export-dump_crosschain_private_keys", toJsonFormat( "dump_crosschain_private_keys", QJsonArray()));
}

void ExportDialog::ParseTunnel(const QString &jsonString)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return ;
    QJsonObject jsonObject = parse_doucment.object();

    if(!jsonObject.value("result").isArray())
    {
        return ;
    }

    QJsonArray object = jsonObject.value("result").toArray();
    if(object.isEmpty()) return ;

    QString accountAddress = object[0].toObject().value("owner").toString();
    QString symbol = object[0].toObject().value("chain_type").toString();
    QString tunnelAddress = object[0].toObject().value("bind_account").toString();

    //根据总的地址公钥私钥信息，匹配该账户对应tunnel对应的私钥
    dataInfo->MatchPrivateKey(symbol,tunnelAddress);
}

void ExportDialog::ParseAllKey(const QString &jsonString)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return ;
    QJsonObject jsonObject = parse_doucment.object();

    if(!jsonObject.value("result").isArray())
    {
        return ;
    }
    QJsonArray arr = jsonObject.value("result").toArray();
    foreach(QJsonValue jsonVal,arr){
        if(!jsonVal.isArray()) continue;
        QJsonArray arrVal = jsonVal.toArray();
        foreach(QJsonValue val,arrVal){
            if(!val.isObject()) continue;
            QJsonObject valObj = val.toObject();
            dataInfo->insertIntoAll(valObj.value("addr").toString(),valObj.value("pubkey").toString(),valObj.value("wif_key").toString());
        }
    }
}

bool ExportDialog::SaveToPath()
{
    QFile file( path);
    QString fName = file.fileName();
#ifdef WIN32
    fName.replace("\\","/");
#endif
    fName = fName.mid( fName.lastIndexOf("\\") + 1);

    if( file.exists())
    {
        CommonDialog tipDialog(CommonDialog::OkAndCancel);
        tipDialog.setText( fName + tr( " already exists!\nCover it or not?") );
        if ( !tipDialog.pop())  return false;
    }

    if( !file.open(QIODevice::WriteOnly))
    {
        qDebug() << "privatekey file open " + fName + " ERROR";

        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText( tr( "Wrong path!") );
        tipDialog.pop();
        return false;
    }

    if(isEncrypt)
    {
        dataInfo->EncryptAES(pwd);
    }
    else
    {
        dataInfo->EncryptBase64();
    }
    return KeyDataUtil::WritePrivateKetToPath(dataInfo,path);
}

void ExportDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith("id-dump_private_key-" + accoutName))
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        ui->exportBtn->setEnabled(true);

        result.prepend("{");
        result.append("}");
qDebug() <<result;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonArray array = jsonObject.take("result").toArray();
        if(array.size() > 0)
        {
            QJsonArray array2 = array.at(0).toArray();

            QString privateKey = array2.at(1).toString();

            dataInfo->info_key["LNK"] = privateKey;
            dataInfo->LNKAddr = array2.at(0).toString();
        }
        else
        {
            qDebug() << "dump_private_key " + accoutName + " ERROR: " + result;
        }
    }
    else if(id == "export-dump_crosschain_private_keys")
    {
        //解析所有私钥
        QString result = UBChain::getInstance()->jsonDataValue( id);
        result.prepend("{");
        result.append("}");

        qDebug()<<result;
        ParseAllKey(result);

        //获取所有的tunnel账户
        QMap<QString,AssetInfo> assetInfoMap = UBChain::getInstance()->assetInfoMap;
        foreach (AssetInfo info, assetInfoMap) {
            if(info.id != "1.3.0")
            {
                UBChain::getInstance()->postRPC( "export-get_binding_account-"+info.symbol,
                                                 toJsonFormat( "get_binding_account", QJsonArray()
                                                 << accoutName<<info.symbol ));
            }

        }
        UBChain::getInstance()->postRPC( "export-finish-tunnel",
                                         toJsonFormat( "export-finish-tunnel", QJsonArray()));

    }
    else if(id.startsWith("export-get_binding_account-"))
    {
        std::lock_guard<std::mutex> loc(mutex);

        QString result = UBChain::getInstance()->jsonDataValue( id);
        result.prepend("{");
        result.append("}");
        //提取通道账户地址
        ParseTunnel(result);
    }
    else if(id == "export-finish-tunnel")
    {
        if(SaveToPath())
        {
            close();

            CommonDialog tipDialog(CommonDialog::OkOnly);
            //            tipDialog.setText( tr( "Export to ") + fName + tr(" succeeded!") + QString::fromLocal8Bit("请妥善保管您的私钥，绝对不要丢失或泄露给任何人!") );
            tipDialog.setText( tr( "Export succeeded!") + tr("Please keep your private key properly.Never lose or leak it to anyone!") );
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


    }
}

void ExportDialog::on_exportBtn_clicked()
{
    if( ui->encryptCheckBox->isChecked() && ui->pathLineEdit->text().endsWith(".elpk") )
    {
        ExportSetPwdDialog exportSetPwdDialog;
        if( !exportSetPwdDialog.pop())  return;

        pwd = exportSetPwdDialog.pwd;
        path = ui->pathLineEdit->text();
        isEncrypt = ui->encryptCheckBox->isChecked();

        getPrivateKey();

        ui->exportBtn->setEnabled(false);
    }
    else if( !ui->encryptCheckBox->isChecked() && ui->pathLineEdit->text().endsWith(".lpk") )
    {
        path = ui->pathLineEdit->text();
        isEncrypt = ui->encryptCheckBox->isChecked();

        getPrivateKey();

        ui->exportBtn->setEnabled(false);
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Wrong file format!"));
        commonDialog.pop();
        return;
    }


}

void ExportDialog::on_encryptCheckBox_stateChanged(int arg1)
{
    if(ui->encryptCheckBox->isChecked())
    {
        QDir dir;
#ifdef WIN32
        QString path = dir.currentPath().replace("\\", "/");
#else
        QString path = dir.currentPath();
#endif
        ui->pathLineEdit->setText( path + "/" + accoutName + ".elpk");
    }
    else
    {
        QDir dir;
#ifdef WIN32
        QString path = dir.currentPath().replace("\\", "/");
#else
        QString path = dir.currentPath();
#endif
        ui->pathLineEdit->setText( path + "/" + accoutName + ".lpk");
    }
}

void ExportDialog::on_closeBtn_clicked()
{
    on_cancelBtn_clicked();
}
void ExportDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(10,10,10,100));//最后一位是设置透明属性（在0-255取值）
    painter.drawRect(QRect(0,0,960,580));

    QWidget::paintEvent(event);
}
