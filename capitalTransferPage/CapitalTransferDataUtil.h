#ifndef CAPITALTRANSFERDATAUTIL_H
#define CAPITALTRANSFERDATAUTIL_H

#include <QString>
#include <QJsonObject>

class CapitalTransferDataUtil
{
public:
    CapitalTransferDataUtil();
public:
    static QString parseTunnelAddress(const QString &jsonString);

    static QString parseMutiAddress(const QString &jsonString);

    static QJsonObject parseTradeDetail(const QString &jsonString);

    static QString parseTransaction(const QString &jsonString);
};

#endif // CAPITALTRANSFERDATAUTIL_H
