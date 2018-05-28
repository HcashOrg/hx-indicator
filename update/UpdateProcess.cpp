#include "UpdateProcess.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcess>

#include "UpdateProgressUtil.h"
#include "UpdateNetWork.h"

class UpdateProcess::DataPrivate
{
public:
    DataPrivate()
        :updateNetwork(new UpdateNetWork())
        ,networkManager(new QNetworkAccessManager())
        ,serverUrl("http://192.168.1.121:5005/api")
        ,downloadPath(QCoreApplication::applicationDirPath() + QDir::separator() + "temp")
        ,localVersionData(std::make_shared<VersionInfo>())
        ,serverVersionData(std::make_shared<VersionInfo>())
        ,isWrongHappened(false)
    {
        qDebug()<<downloadPath;
    }
public:
    UpdateNetWork *updateNetwork;
    QNetworkAccessManager   *networkManager;

    QString serverUrl;
    QString clientID;

    VersionInfoPtr localVersionData;//本地文件解析结果
    VersionInfoPtr serverVersionData;//服务器下载的文件解析结果

    QString downloadPath;

    QList<DownLoadData> updateList;//需要更新的列表

    bool isWrongHappened;
};

UpdateProcess::UpdateProcess(QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate())
{
    InitData();
}

void UpdateProcess::InitServerURL(const QString &url)
{
    _p->serverUrl = url;
}

void UpdateProcess::checkUpdate()
{
    //设置当前没有错误
    _p->isWrongHappened = false;

    disconnect(_p->updateNetwork,&UpdateNetWork::TaskEmpty,this,&UpdateProcess::DownloadEmptySlot);
    //先清空下载的数据
    UpdateProgressUtil::deleteDir(_p->downloadPath);

    //本地版本文件解析
    qDebug()<<"解析本地文件``"<<QCoreApplication::applicationDirPath() + QDir::separator() + "update.xml";
    UpdateProgressUtil::ParseXmlPath(QCoreApplication::applicationDirPath() + QDir::separator() + "update.xml",
                                     _p->localVersionData);

    //获取最新的配置
    GetLatestVersionInfo();

}

void UpdateProcess::startDownload()
{    //开始下载
    //链接最终下载完毕信号
    connect(_p->updateNetwork,&UpdateNetWork::TaskEmpty,this,&UpdateProcess::DownloadEmptySlot);
    qDebug()<<_p->updateList.size();
    _p->updateNetwork->DownLoadFile(_p->updateList);
}

void UpdateProcess::startUpdate()
{
    //关闭主程序，启动复制程序
    startDownload();

}

void UpdateProcess::GetLatestVersionInfoSlots()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if(!reply) return;

    VersionData serverVersion ;//中间件返回的config地址
    UpdateProgressUtil::ParseVersion(QString(reply->readAll()),serverVersion);

    //下载config配置
    //下载当前服务器版本
    DownLoadData up;
    up.fileName = "update.xml";
    up.url = /*_p->serverUrl + serverVersion.url;*/"http://192.168.1.161/down/update.xml";//测试用，本地文件
    qDebug()<<up.url;
    up.filePath = _p->downloadPath + QDir::separator() + up.fileName;
    connect(_p->updateNetwork,&UpdateNetWork::DownLoadFinish,this,&UpdateProcess::DownLoadVersionConfigFinsihed);

    _p->updateNetwork->DownLoadFile(up);
}

void UpdateProcess::DownLoadVersionConfigFinsihed()
{
    disconnect(_p->updateNetwork,&UpdateNetWork::DownLoadFinish,this,&UpdateProcess::DownLoadVersionConfigFinsihed);

    //下载完配置文件后，对比本地、服务器配置，列出需要更新的配置项
    //服务端下载的版本文件解析
    UpdateProgressUtil::ParseXmlPath(_p->downloadPath + QDir::separator() + "update.xml",_p->serverVersionData);

    if(_p->serverVersionData->version.isEmpty())
    {
        return;//说明出错，没必要进行了
    }

    //检查更新器是否需要更新
    if(UpdateProgressUtil::AFTER == UpdateProgressUtil::CompareVersion(_p->localVersionData->updateVersion,_p->serverVersionData->updateVersion))
    {
        //更新更新器
        DownLoadData down;
        down.filePath = QCoreApplication::applicationDirPath() + QDir::separator()+"Copy.exe";
        down.fileName = "Copy.exe";
        down.version = _p->serverVersionData->updateVersion;
        down.url = _p->serverVersionData->serverPath + "/"+"Copy.exe";

        //删除文件
        QFile::remove(down.filePath);
        _p->updateNetwork->DownLoadFile(down);
    }

    //对比分析出需要下载的版本信息及目录
    UpdateProgressUtil::ExtractUpdateData(_p->localVersionData,_p->serverVersionData,_p->downloadPath,_p->updateList);
    //发送版本信息
    emit NewstVersionSignal(_p->updateList.empty()?"":_p->serverVersionData->version);
}

void UpdateProcess::DownloadEmptySlot()
{
    //下载完毕，发出更新完毕的信号
    if(!_p->isWrongHappened)
    {
        emit updateFinish();
    }
}

void UpdateProcess::DownloadWrongSlot(const QString &fileName)
{
    _p->isWrongHappened = true;
    emit updateWrong();
}

void UpdateProcess::GetLatestVersionInfo()
{
    //请求头
    QNetworkRequest request;
    request.setRawHeader("Content-Type","application/json");
    request.setUrl(QUrl(_p->serverUrl));

    //构造请求内容
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","Client.Upgrade.checkNewVersion");
    QJsonObject paramObject;
    paramObject.insert("clientId",_p->clientID);
    paramObject.insert("localVersion",_p->localVersionData->version);
    object.insert("params",paramObject);

   QNetworkReply *reply = _p->networkManager->post(request,QJsonDocument(object).toJson());
   connect(reply,&QNetworkReply::finished,this,&UpdateProcess::GetLatestVersionInfoSlots);
}

void UpdateProcess::InitData()
{
    connect(_p->updateNetwork,&UpdateNetWork::DwonLoadWrong,this,&UpdateProcess::DownloadWrongSlot);
}



