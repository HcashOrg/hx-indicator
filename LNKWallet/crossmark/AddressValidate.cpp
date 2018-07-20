#include "AddressValidate.h"

#include "extra/HttpManager.h"
#include "wallet.h"

class AddressValidate::DataPrivate
{
public:
    DataPrivate()
    {

    }
public:
    HttpManager httpManager;
};

AddressValidate::AddressValidate(QObject *parent) : QObject(parent)
  ,_p(new DataPrivate())
{
    connect(&_p->httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));
}

void AddressValidate::startValidateAddress(const QString &chainId, const QString &address)
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","Zchain.Address.validate");
    QJsonObject paramObject;
    paramObject.insert("chainId",chainId);
    paramObject.insert("addr",address);
    object.insert("params",paramObject);
    _p->httpManager.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());

}

void AddressValidate::httpReplied(QByteArray _data, int _status)
{

    QJsonObject object  = QJsonDocument::fromJson(_data).object().value("result").toObject();
    qDebug()<<object;
    emit AddressValidateSignal(object.value("valid").toBool());
}
