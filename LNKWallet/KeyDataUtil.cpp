#include "KeyDataUtil.h"

#include <QFile>
#include <QJsonArray>
#include <QTextCodec>
#include <QJsonObject>
#include <QDebug>

KeyDataUtil::KeyDataUtil()
{

}

bool KeyDataUtil::ReadaPrivateKeyFromPath(const QString &filePath, KeyDataPtr &data)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)) return false;

    QString jsonStr(file.readAll());
    file.close();

    return ReadPrivateKeyFromJson(jsonStr,data);
}

bool KeyDataUtil::WritePrivateKetToPath(const KeyDataPtr &data, const QString &filePath)
{
    QJsonDocument document;
    if(WritePrivateKeyToJson(data,document))
    {
        QByteArray byte_array = document.toJson(QJsonDocument::Compact);
        //写文件
        QFile file(filePath);
        if(!file.open(QIODevice::WriteOnly)) return false;

        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream<<byte_array;
        stream.setGenerateByteOrderMark(true);
        file.close();
        return true;
    }
    return false;
}

bool KeyDataUtil::ReadPrivateKeyFromJson(const QString &jsonStr, KeyDataPtr &data)
{
    if(!data)
    {
        data = std::make_shared<KeyDataInfo>();
    }

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(jsonStr);

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);

    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return false;

    QJsonObject jsonObject = parse_doucment.object();
    //开始分析
    foreach (QString key, jsonObject.keys())
    {
        if(key == "HXAddr")
        {
            data->HXAddr = jsonObject.value(key).toString();
        }
        else
        {
            data->info_key[key] = jsonObject.value(key).toString();
        }

    }
    return true;
}

bool KeyDataUtil::WritePrivateKeyToJson(const KeyDataPtr &data, QJsonDocument &obj)
{
    if(!data) return false;
    //构建QJsonDocument
    QJsonObject mainObject;

    mainObject.insert("HXAddr",data->HXAddr);
    for(auto it = data->info_key.begin();it != data->info_key.end();++it)
    {
        mainObject.insert((*it).first,(*it).second);
    }

    obj.setObject(mainObject);
    return true;
}
