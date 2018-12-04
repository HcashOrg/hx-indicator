#include "DepositDataUtil.h"
#include "control/qrencode.h"

#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

DepositDataUtil::DepositDataUtil()
{

}

QImage DepositDataUtil::CreateQRcodeImage(const QString &data)
{
    if(data.isEmpty())
    {
        return QImage(1,1,QImage::Format_RGB32);
    }

    QRcode *qr = QRcode_encodeString(data.toStdString().c_str(),1,QR_ECLEVEL_L,QR_MODE_8,1);
    if(!qr)
    {
        return QImage(1,1,QImage::Format_RGB32);
    }
    QImage image(qr->width,qr->width,QImage::Format_RGB32);
    image.fill(Qt::white);
    for(int i = 0; i < qr->width; ++i)
    {
        for(int j = 0; j < qr->width; ++j)
        {
            if(qr->data[i * qr->width + j] & 0x01)
            {
                image.setPixelColor(i,j,Qt::black);
            }
        }
    }
    return image;
}
#include <QDebug>
bool DepositDataUtil::ParseTunnelData(const QString &jsonString, std::shared_ptr<TunnelData> &resultData)
{
    if(!resultData) resultData = std::make_shared<TunnelData>();
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return false;
    QJsonObject jsonObject = parse_doucment.object();
    QJsonObject object = jsonObject.value("result").toObject();

    resultData->address = object.value("addr").toString();
    resultData->publicKey = object.value("pubkey").toString();
    resultData->privateKey = object.value("wif_key").toString();
    return true;

}

QString DepositDataUtil::parseTunnelAddress(const QString &jsonString)
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
