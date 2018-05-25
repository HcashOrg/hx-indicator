#ifndef UPDATENETWORK_H
#define UPDATENETWORK_H

#include <QObject>
#include <QNetworkReply>
struct DownLoadData;
class UpdateNetWork : public QObject
{
    Q_OBJECT
public:

public:
    explicit UpdateNetWork(QObject *parent = 0);

public:
    void DownLoadFile(const DownLoadData &data);
    void DownLoadFile(const QList<DownLoadData> &data);
private:
    void startDownLoad();
signals:
    void TaskEmpty();
    void DownLoadFinish(const QString &filename);
    void DwonLoadWrong(const QString &filename);

private slots:
    void DownLoadReadSlots();
    void DownLoadProgressSlots(qint64 bytesReceived, qint64 bytesTotal);
    void DownLoadFinishSlots();

    void DownLoadErrorSlots(QNetworkReply::NetworkError code);
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // UPDATENETWORK_H
