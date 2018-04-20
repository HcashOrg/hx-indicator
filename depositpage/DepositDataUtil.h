#ifndef DEPOSITDATAUTIL_H
#define DEPOSITDATAUTIL_H

#include <memory>
#include <QString>

class QImage;

class TunnelData
{
public:
    TunnelData(const QString &add="",const QString &pubKey="",const QString &priKey="")
        :address(add),publicKey(pubKey),privateKey(priKey)
    {

    }
public:
    QString address;
    QString publicKey;
    QString privateKey;
};

class DepositDataUtil
{
public:
    DepositDataUtil();
public:
    static QImage CreateQRcodeImage(const QString &data);

    static bool ParseTunnelData(const QString &jsonString,std::shared_ptr<TunnelData> &resultData);

    static QString parseTunnelAddress(const QString &jsonString);
};

#endif // DEPOSITDATAUTIL_H
