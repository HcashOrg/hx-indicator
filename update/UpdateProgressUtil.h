#ifndef UPDATEPROGRESSUTIL_H
#define UPDATEPROGRESSUTIL_H

#include <QString>
#include <QDomDocument>
#include <memory>
#include <vector>

class VersionData
{
public:
    VersionData()
    {

    }
public:
    QString version;
    QString url;
    QString checksum;
    QString signature;
};

class VersionInfo
{
public:
    QString version;
    QString serverPath;
    QString updateVersion;//更新器版本
    std::vector<std::shared_ptr<VersionData>> file_data;
    std::vector<std::shared_ptr<VersionData>> update_data;
};
typedef std::shared_ptr<VersionInfo> VersionInfoPtr;

struct DownLoadData{
    QString fileName;//相对路径，，有后缀名
    QString filePath;//存储全路径
    QString version;//版本
    QString url;//目标地址
    QString checksum;//MD5验证
    QString signature;//签名验证
};

class UpdateProgressUtil
{
public:
    UpdateProgressUtil();

public:
    //检测版本信息，相同、落后、超前
    enum VersionEnum{EQUAL,AFTER,BEFORE};
    static VersionEnum CompareVersion(const QString &oldVersion,const QString &newVersion);

    static void ParseVersion(const QString &jsonStr, VersionData &data);


    static bool ParseXmlPath(const QString &fullPath,VersionInfoPtr &data);
    static bool MakeUpVersionData(VersionInfoPtr &data,const QString &headPath);

    //版本对比，提取需要更新的版本
    static bool ExtractUpdateData(const VersionInfoPtr &oldVersion,const VersionInfoPtr &newVersion,
                                  const QString &updateDir,const QString &dirPath,QList<DownLoadData> &data);
public:
    static bool deleteDir(const QString &dirName);
    static bool copyDir(const QString &source, const QString &destination, bool override = true);
private:
    static bool ParseXmlDoc(const QDomDocument &doc,VersionInfoPtr &data);

};

#endif // UPDATEPROGRESSUTIL_H
