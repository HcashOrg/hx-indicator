#ifndef CONTRACTSERVERINFOMANAGER_H
#define CONTRACTSERVERINFOMANAGER_H

#include <QObject>
#include <QtNetwork>

struct ContractServerInfo   // 来自服务器的合约信息
{
    QString contractAddress;
    QString contractName;
    QString website;
    QString description;
    int     guarantee;
    int     hotValue;           // 热度值
    QString logoPath;
    int     logoVersion;
    QString updateTime;
    QString createTime;
    QString summary;
    bool    enabled;
};
typedef QMap<QString,ContractServerInfo>   ContractServerInfoMap;

struct  ContractServerInfoSlice
{
    int version = 0;
    ContractServerInfoMap map;
};


class ContractServerInfoManager : public QObject
{
    Q_OBJECT
public:
    explicit ContractServerInfoManager(QObject *parent = 0);

    ContractServerInfoSlice   contractServerInfoSlice;
    void getContractInfoFromServer();

    void getContractInfoFromLocalFile();

    QString serverPathToLocalPath(QString _serverPath);     // 服务端路径转换为本地文件路径(相对路径)


private:
    ContractServerInfoSlice  parseContractServerInfo(QString str);

    QStringList downloadList;
    void getDownloadList();
private slots:
    void startDownload();
    void downloadFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager qnam;
    QNetworkAccessManager downloadManager;
private slots:
    void httpFinished(QNetworkReply* reply);

};

#endif // CONTRACTSERVERINFOMANAGER_H
