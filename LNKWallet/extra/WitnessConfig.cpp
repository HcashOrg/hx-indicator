#include "WitnessConfig.h"

#include "wallet.h"

#ifdef WIN32
#define NEWLINE_CHAR    "\r\n"
#else
#define NEWLINE_CHAR    "\n"
#endif

WitnessConfig::WitnessConfig(QObject *parent) : QObject(parent)
{

}

WitnessConfig::~WitnessConfig()
{
    if(file)
    {
        delete file;
    }
}

int WitnessConfig::init()
{
    QString filePath;
    if( HXChain::getInstance()->configFile->contains("/settings/chainPath"))
    {
        filePath = HXChain::getInstance()->configFile->value("/settings/chainPath").toString() + "/config.ini";
    }
    else
    {
        filePath = HXChain::getInstance()->appDataPath + "/config.ini";
    }

    file = new QFile(filePath);
    inited = file->open(QIODevice::ReadOnly);
    if(inited)
    {
        QString str = file->readAll();
        file->close();

        data = str.split(NEWLINE_CHAR);
    }
    else
    {
        qDebug() << "read witness config file error: " << file->errorString();
    }

    return inited;
}

void WitnessConfig::save()
{
    if(!inited)     return;
    mutex.lock();
    if(file->open(QIODevice::WriteOnly))
    {
        QString dataStr;

        for(int i = 0; i < data.size(); i++)
        {
            dataStr += data.at(i);
            if( i != data.size() - 1)  dataStr += NEWLINE_CHAR;
        }

        file->resize(0);
        QTextStream ts(file);
        ts << dataStr.toUtf8();
        file->close();
    }
    mutex.unlock();
}

void WitnessConfig::modify(QString key, QString value, bool isString)
{
    if(!inited)     return;
    if(this->value(key).isEmpty())
    {
        this->append(key,value,isString);
    }
    else
    {
        for (QString& str : data)
        {
            QString str2 = str;
            str2 = str2.remove(' ');
            if(str2.startsWith("#"))    continue;
            if(!str2.contains("="))     continue;

            QStringList strList2 = str2.split('=');
            QString k = strList2.at(0);

            if(k == key)
            {
                if(isString)
                {
                    value.prepend("\"");
                    value.append("\"");
                }
                str = key + "=" + value;
                break;
            }
        }
    }

}

void WitnessConfig::append(QString key, QString value, bool isString)
{
    if(!inited)     return;
    if(isString)
    {
        value.prepend("\"");
        value.append("\"");
    }
    QString str = key + "=" + value;
    data.prepend(str);
}

void WitnessConfig::remove(QString key)
{
    if(!inited)     return;
    for (QString& str : data)
    {
        QString str2 = str;
        str2 = str2.remove(' ');
        if(str2.startsWith("#"))    continue;
        if(!str2.contains("="))     continue;

        QStringList strList2 = str2.split('=');
        QString k = strList2.at(0);
        if(k == key)
        {
            str = "";
            break;
        }
    }
}

QString WitnessConfig::value(QString key)
{
    for (const QString& str : data)
    {
        QString str2 = str;
        str2 = str2.remove(' ');
        if(str2.startsWith("#"))    continue;
        if(!str2.contains("="))     continue;

        QStringList strList2 = str2.split('=');
        QString k = strList2.at(0);
        if(k == key)
        {
            return strList2.at(1);
        }
    }

    return "";
}

QStringList WitnessConfig::allValue(QString key)
{
    QStringList result;
    for (const QString& str : data)
    {
        QString str2 = str;
        str2 = str2.remove(' ');
        if(str2.startsWith("#"))    continue;
        if(!str2.contains("="))     continue;

        QStringList strList2 = str2.split('=');
        QString k = strList2.at(0);
        if(k == key)
        {
            QString value = strList2.at(1);
            value.replace("\"\"", " ");
            value.remove("\"");
            result += value.split(" ");
        }
    }

    result.removeDuplicates();

    return result;
}

void WitnessConfig::addTrackAddress(QString address)
{
    if(address.isEmpty())       return;
    this->append("track-address", address, true);
}

QStringList WitnessConfig::getTrackAddresses()
{
    return this->allValue("track-address");
}


void WitnessConfig::addPrivateKey(QString pubKey, QString privateKey)
{
    if(pubKey.isEmpty() || privateKey.isEmpty())    return;
    QString pks = this->value("private-key");
    QJsonArray array = QJsonDocument::fromJson( pks.toUtf8()).array();
    QJsonArray array2;
    array2 << pubKey << privateKey;
    array << array2;
    this->modify("private-key" , QString( QJsonDocument(array).toJson()).remove(" ").remove("\n"));
}

void WitnessConfig::addMiner(QString minerId)
{
    if(minerId.isEmpty())       return;
    this->append("miner-id", minerId, true);
}

QStringList WitnessConfig::getMiners()
{
    QStringList minerIds = this->allValue("miner-id");
    return minerIds;
}

bool WitnessConfig::isProductionEnabled()
{
    QString str = this->value("enable-stale-production");
    return str == "true";
}

void WitnessConfig::setProductionEnabled(bool enabled)
{
    this->modify("enable-stale-production", enabled ? "true" : "false");
}

QStringList WitnessConfig::getChainTypes()
{
    QStringList result;
    QString chainTypeArray = this->value("chain-type");
    QJsonDocument document = QJsonDocument::fromJson(chainTypeArray.toUtf8());
    QJsonArray array = document.array();
    foreach(const QJsonValue& v, array)
    {
        if(!v.toString().isEmpty())     result += v.toString();
    }
    return result;
}

void WitnessConfig::setChainTypes(QStringList chainTypes)
{
    QJsonArray array;
    foreach(const QString& type, chainTypes)
    {
        array << type;
    }

    this->modify("chain-type" , QString( QJsonDocument(array).toJson()).remove(" ").remove("\n"));
}
