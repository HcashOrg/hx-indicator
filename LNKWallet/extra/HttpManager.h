#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTimer>

class HttpManager: public QObject
{
    Q_OBJECT
public:
    HttpManager();
    ~HttpManager();
    void get(const QString url);
    void post(const QString url, const QByteArray& data);

    void fetchCoinBalance(int id, QString chainId, QString address);

public slots:
    void requestFinished(QNetworkReply *reply);

signals:
    void httpReplied(QByteArray data, int statusCode);
    void httpError(int statusCode);

public:
    void setTimeoutSeconds(int _second);
private:
    QTimer timer;
    int timeoutSeconds = 0;

private:
    QNetworkRequest httpRequest;
    QNetworkAccessManager   networkAccessManager;
};

#endif // HTTPMANAGER_H
