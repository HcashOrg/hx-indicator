#include "UpdateNetWork.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <mutex>
#include <QDir>
#include "UpdateProgressUtil.h"

class UpdateNetWork::DataPrivate
{
public:
    DataPrivate()
        :networkManager(new QNetworkAccessManager())
        ,currentFile(nullptr)
    {

    }
public:
    QNetworkAccessManager   *networkManager;

    QList<DownLoadData> downloadList;

    std::mutex listMutex;

    QFile *currentFile;
};

UpdateNetWork::UpdateNetWork(QObject *parent) : QObject(parent)
  ,_p(new DataPrivate())
{

}

void UpdateNetWork::DownLoadFile(const DownLoadData &data)
{
    std::lock_guard<std::mutex> loc(_p->listMutex);
    if(_p->downloadList.empty())
    {
        _p->downloadList.append(data);
        startDownLoad();
    }
    else
    {
        _p->downloadList.append(data);
    }

}

void UpdateNetWork::DownLoadFile(const QList<DownLoadData> &data)
{
    std::lock_guard<std::mutex> loc(_p->listMutex);
    if(_p->downloadList.empty())
    {
        _p->downloadList.append(data);
        startDownLoad();
    }
    else
    {
        _p->downloadList.append(data);
    }
}

void UpdateNetWork::startDownLoad()
{
    //开始下载
    if(_p->downloadList.empty()) return;

    //打开文件
    if(_p->currentFile)
    {
        delete _p->currentFile;
        _p->currentFile = nullptr;
    }

    //创建目录
    QDir dir(QFileInfo(_p->downloadList.front().filePath).path());
    if(!dir.exists())
    {
        dir.mkpath(QFileInfo(_p->downloadList.front().filePath).path());
    }
    //删除原文件
    QFile::remove(_p->downloadList.front().filePath);

    _p->currentFile = new QFile(_p->downloadList.front().filePath);
    if(!_p->currentFile->open(QIODevice::WriteOnly|QIODevice::Append))
    {
        qDebug()<<"wrong open file!"<<_p->downloadList.front().filePath;
        return;
    }
    qDebug()<<_p->downloadList.front().filePath <<"url --"<< _p->downloadList.front().url;
    QNetworkReply *reply = _p->networkManager->get(QNetworkRequest(_p->downloadList.front().url));
    connect(reply,&QNetworkReply::readyRead,this,&UpdateNetWork::DownLoadReadSlots);
    connect(reply,&QNetworkReply::downloadProgress,this,&UpdateNetWork::DownLoadProgressSlots);
    connect(reply,&QNetworkReply::finished,this,&UpdateNetWork::DownLoadFinishSlots);

    connect(reply,static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),this,&UpdateNetWork::DownLoadErrorSlots);

}

void UpdateNetWork::DownLoadReadSlots()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if(!reply) return;

    _p->currentFile->write(reply->readAll());
}

void UpdateNetWork::DownLoadProgressSlots(qint64 bytesReceived,qint64 bytesTotal)
{
    //todo
}

void UpdateNetWork::DownLoadFinishSlots()
{
    _p->currentFile->flush();
    _p->currentFile->close();

    emit DownLoadFinish(_p->downloadList.front().fileName);

    std::lock_guard<std::mutex> loc(_p->listMutex);
    _p->downloadList.removeFirst();
    if(!_p->downloadList.isEmpty())
    {
        startDownLoad();
    }
    else
    {
        emit TaskEmpty();
    }

}

void UpdateNetWork::DownLoadErrorSlots(QNetworkReply::NetworkError code)
{

    emit DwonLoadWrong(_p->downloadList.front().fileName);
}

