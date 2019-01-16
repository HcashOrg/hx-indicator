#ifndef UPDATEPROCESS_H
#define UPDATEPROCESS_H

#include <QObject>

class UpdateProcess : public QObject
{
    Q_OBJECT
public:
    explicit UpdateProcess(QObject *parent = 0);
    ~UpdateProcess();
signals:
    void NewstVersionSignal(const QString &version,bool isUpdateForced = false);//没有更新则为空,,true则表示强制更新版本
    void updateFinish();
    void updateWrong();
public:
  void InitServerURL(const QString &url);
  void checkUpdate();
  void startUpdate();
  const QString &getUpdateLogInfo()const;
private:
    //获取最新版本信息，下载配置文件
    void GetLatestVersionInfo();
    //开始下载需要更新的程序
    void startDownload();
private slots:
    //获取最新版本的响应，内容估计不多，直接链接finish信号，无需断点传送
    void GetLatestVersionInfoSlots();
    void DownLoadVersionConfigFinsihed(const QString &dstFilename);

    //获取下载完毕信号
    void DownloadEmptySlot();

    //下载出错
    void DownloadWrongSlot(const QString &fileName);
private:
    void InitData();//读取本地版本文件等初始化工作
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // UPDATEPROCESS_H
