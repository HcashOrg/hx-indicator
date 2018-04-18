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

QString CapitalTransferDataUtil::parseMutiAddress(const QString &jsonString)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return "";
    QJsonObject jsonObject = parse_doucment.object();

    if(!jsonObject.value("result").isObject())
    {
        return "";
    }

    return jsonObject.value("result").toObject().value("new_address_hot").toString();
}

QString CapitalTransferDataUtil::parseTradeDetail(const QString &jsonString)
{
    return "";
}

QString CapitalTransferDataUtil::parseTransaction(const QString &jsonString)
{
    return "";
}
