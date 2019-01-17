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
    ~UpdateNetWork();
public:
    void DownLoadFile(const DownLoadData &data);
    void DownLoadFile(const QList<DownLoadData> &data);
private:
    void startDownLoad();
signals:
    void TaskEmpty();
    void DownLoadFinish(const QString &filePath);
    void DwonLoadWrong(const QString &filePath);

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
