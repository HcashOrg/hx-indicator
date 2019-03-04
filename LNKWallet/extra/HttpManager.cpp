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
    if(timeoutSeconds > 0)
    {
//        timer.singleShot(timeoutSeconds * 1000, [this](){
//            qDebug() << "cccccccccccccc ";
//            Q_EMIT httpError(0);});
        connect(&timer, &QTimer::timeout, [&](){
            Q_EMIT httpError(0);
            timer.stop();
        });
        timer.start(timeoutSeconds * 1000);
    }

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
    timer.stop();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

//    qDebug() << "HttpManager statusCode: " << statusCode;

    if(reply->error() == QNetworkReply::NoError)
    {
        emit httpReplied(reply->readAll(),statusCode);
    }
    else
    {
        emit httpError(statusCode);
    }

    reply->deleteLater();
}

void HttpManager::setTimeoutSeconds(int _second)
{
    timeoutSeconds = _second;
}
