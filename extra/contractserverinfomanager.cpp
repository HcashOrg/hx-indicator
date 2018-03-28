#include "contractserverinfomanager.h"

#include "lnk.h"

#include <QFile>
#include <QDir>

#define CONTRACT_SERVER_IP_AND_PORT     "http://52.89.234.162"

ContractServerInfoManager::ContractServerInfoManager(QObject *parent) : QObject(parent)
{
    connect(&qnam, SIGNAL(finished(QNetworkReply    *)), this, SLOT(httpFinished(QNetworkReply*)));
    connect(&downloadManager, SIGNAL(finished(QNetworkReply    *)), this, SLOT(downloadFinished(QNetworkReply*))); //关联信号和槽
}

void ContractServerInfoManager::getContractInfoFromServer()
{
    qnam.get( QNetworkRequest(QUrl(QString(CONTRACT_SERVER_IP_AND_PORT) + "/walletApi/v1/queryContractList")));
}

void ContractServerInfoManager::getContractInfoFromLocalFile()
{
    QFile file(UBChain::getInstance()->walletConfigPath + "/contract.info");
    if(file.open(QIODevice::ReadOnly))
    {
        QString str = file.readAll();

        contractServerInfoSlice = parseContractServerInfo(str);
    }
    else
    {
        qDebug() << "read contract.info failed: " << file.errorString();
    }

    QDir dir(UBChain::getInstance()->walletConfigPath + "/logoPic");
    if(!dir.exists())
    {
        dir.mkdir(UBChain::getInstance()->walletConfigPath + "/logoPic");
    }
}


ContractServerInfoSlice ContractServerInfoManager::parseContractServerInfo(QString str)
{
    ContractServerInfoSlice slice;

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(str);

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if( parse_doucment.isObject())
        {
            QJsonObject jsonObject = parse_doucment.object();
            int retCode = jsonObject.take("retCode").toInt();
            if(retCode == 200)
            {
                int version = jsonObject.take("version").toInt();
                slice.version = version;

                QJsonArray array = jsonObject.take("data").toArray();

                int size = array.size();
                for(int i = 0; i < size; i++)
                {
                    QJsonObject object = array.at(i).toObject();
                    ContractServerInfo info;
                    info.contractAddress    = object.take("contractId").toString();
                    info.contractName       = object.take("contractName").toString();
                    info.website            = object.take("website").toString();
                    info.description        = object.take("description").toString();
                    info.guarantee          = object.take("guarantee").toInt();
                    info.hotValue           = object.take("hot").toInt();
                    info.logoPath           = object.take("logoPath").toString();
                    info.logoVersion        = object.take("logoVersion").toInt();
                    info.summary            = object.take("summary").toString();

                    slice.map.insert(info.contractAddress,info);
                }
            }
            else
            {
                qDebug() << retCode << str;
            }
        }
    }

    return slice;
}

void ContractServerInfoManager::getDownloadList()
{
    QStringList contracts = contractServerInfoSlice.map.keys();
    if(contracts.size() <= 0)    return;

    downloadList = contracts;
}

QString ContractServerInfoManager::serverPathToLocalPath(QString _serverPath)
{
    _serverPath.replace("\\","/");
    QString fileName = _serverPath.mid( _serverPath.lastIndexOf("/") + 1);
    return  "/logoPic/" + fileName;
}

void ContractServerInfoManager::startDownload()
{
   if(downloadList.size() <= 0)    return;

   QString path = contractServerInfoSlice.map.value(downloadList.at(0)).logoPath;
   downloadManager.get( QNetworkRequest(QUrl(path)));
}

void ContractServerInfoManager::downloadFinished(QNetworkReply* reply)
{
    QByteArray ba = reply->readAll();
    reply->deleteLater();

    QString path = contractServerInfoSlice.map.value(downloadList.at(0)).logoPath;
    QString logoPath = serverPathToLocalPath(path);

    QFile file(UBChain::getInstance()->walletConfigPath + logoPath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(ba);
    }
    else
    {
        qDebug() << "download logo open file error: "  << file.errorString();
    }

    downloadList.removeFirst();
    startDownload();
}

void ContractServerInfoManager::httpFinished(QNetworkReply *reply)
{
    QString str = reply->readAll();
    reply->deleteLater();

    ContractServerInfoSlice slice = parseContractServerInfo(str);

    if(slice.version > contractServerInfoSlice.version)
    {
        contractServerInfoSlice = slice;

        downloadList = contractServerInfoSlice.map.keys();
        startDownload();

        QFile file(UBChain::getInstance()->walletConfigPath + "/contract.info");
        if(file.open(QIODevice::WriteOnly))
        {
            file.resize(0);
            QTextStream ts( &file);
            ts << str;
            file.close();

        }
        else
        {
            qDebug() << "write contract.info failed: " << file.errorString();
        }
    }

}
