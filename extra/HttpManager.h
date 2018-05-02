#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class HttpManager: public QObject
{
public:
    HttpManager();
};

#endif // HTTPMANAGER_H
