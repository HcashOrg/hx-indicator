#include "CapitalTransferDataUtil.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

CapitalTransferDataUtil::CapitalTransferDataUtil()
{

}

QString CapitalTransferDataUtil::parseTunnelAddress(const QString &jsonString)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return "";
    QJsonObject jsonObject = parse_doucment.object();

    if(!jsonObject.value("result").isArray())
    {
        return "";
    }

    QJsonArray object = jsonObject.value("result").toArray();
    if(object.isEmpty()) return "";

    return object[0].toObject().value("bind_account").toString();
}
#include <QDebug>
QString CapitalTransferDataUtil::parseMutiAddress(const QString &jsonString)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return "";
    QJsonObject jsonObject = parse_doucment.object();
    qDebug()<<jsonString;
    if(!jsonObject.value("result").isArray())
    {
        return "";
    }

    return jsonObject.value("result").toArray().last().toObject().value("bind_account_hot").toString();
}

QString CapitalTransferDataUtil::parseTradeDetail(const QString &jsonString)
{
    QString data = jsonString;
    data.remove(0,10);
    data.remove(data.length()-1,1);
    qDebug()<<data;
    return data;
}

QString CapitalTransferDataUtil::parseTransaction(const QString &jsonString)
{
    return "";
}
