#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class HttpManager: public QObject
{
    Q_OBJECT
public:
    HttpManager();
    ~HttpManager();
    void get(const QString url);
    void post(const QString url, const QByteArray& data);

public slots:
    void requestFinished(QNetworkReply *reply);

signals:
    void httpReplied(QByteArray data, int statusCode);

private:
    QNetworkRequest httpRequest;
    QNetworkAccessManager   networkAccessManager;
};

#endif // HTTPMANAGER_H
