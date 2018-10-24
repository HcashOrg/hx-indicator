#include "HttpManager.h"

#include "wallet.h"

HttpManager::HttpManager()
{
    httpRequest.setRawHeader("Content-Type","application/json");

    connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*))); //关联信号和槽
}

HttpManager::~HttpManager()
{

}

void HttpManager::get(const QString url)
{
    httpRequest.setUrl(QUrl(url));
    networkAccessManager.get(httpRequest);
}

void HttpManager::post(const QString url, const QByteArray &data)
{
    httpRequest.setUrl(QUrl(url));
    networkAccessManager.post(httpRequest, data);
}

void HttpManager::fetchCoinBalance(int id, QString chainId, QString address)
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",id);
    object.insert("method","Zchain.Address.GetBalance");
    QJsonObject paramObject;
    paramObject.insert("chainId",chainId);
    paramObject.insert("addr",address);
    object.insert("params",paramObject);
    post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());
}

void HttpManager::requestFinished(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

//    qDebug() << "HttpManager statusCode: " << statusCode;

    if(reply->error() == QNetworkReply::NoError)
    {
        emit httpReplied(reply->readAll(),statusCode);
    }
    else
    {

    }

    reply->deleteLater();
}
