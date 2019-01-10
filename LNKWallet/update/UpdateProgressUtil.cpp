#include "UpdateProgressUtil.h"

#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

UpdateProgressUtil::UpdateProgressUtil()
{

}


UpdateProgressUtil::VersionEnum UpdateProgressUtil::CompareVersion(const QString &oldVersion, const QString &newVersion)
{

    if(oldVersion == newVersion || oldVersion.isEmpty() || newVersion.isEmpty()) return EQUAL;

    QStringList aList = oldVersion.split(".");
    QStringList bList = newVersion.split(".");
    if( aList.at(0).toInt() > bList.at(0).toInt() )
    {
        return BEFORE;
    }
    else if( aList.at(0).toInt() < bList.at(0).toInt())
    {
        return AFTER;
    }
    else
    {
        if( aList.at(1).toInt() > bList.at(1).toInt() )
        {
            return BEFORE;
        }
        else if( aList.at(1).toInt() < bList.at(1).toInt())
        {
            return AFTER;
        }
        else
        {
            if( aList.at(2).toInt() > bList.at(2).toInt() )
            {
                return BEFORE;
            }
            else if( aList.at(2).toInt() < bList.at(2).toInt())
            {
                return AFTER;
            }
            else
            {
                return EQUAL;
            }
        }
    }
}

void UpdateProgressUtil::ParseVersion(const QString &jsonStr,VersionData &data)
{
    QJsonObject object  = QJsonDocument::fromJson(jsonStr.toUtf8()).object().value("result").toObject();
    data.url = object.value("downloadUrl").toString();
    qDebug()<< object.value("latestVersion").toString();
}

bool UpdateProgressUtil::ParseXmlDoc(const QDomDocument &doc, VersionInfoPtr &data)
{
    if(!data)
    {
        data = std::make_shared<VersionInfo>();
    }

    data->file_data.clear();
    QDomElement docElem = doc.documentElement();  //返回根元素
    QDomNode n = docElem.firstChild();   //返回根节点的第一个子节点,info
    while(!n.isNull())   //如果节点不为空
    {
        if (n.isElement())  //如果节点是元素
        {
            QDomElement e = n.toElement();  //将其转换为元素
            if( e.tagName() == "MainFiles")
            {
                QDomNodeList list = e.childNodes(); //获得元素e的所有子节点的列表

                for(int i=0; i<list.count(); i++) //遍历该列表
                {
                    QDomNode node = list.at(i);
                    QDomNode nameNode = node.childNodes().at(0);
                    QDomNode latestVerNode = node.childNodes().at(1);
                    QDomNode MD5Node = node.childNodes().at(2);
                    QDomNode signatureNode = node.childNodes().at(3);

                    std::shared_ptr<VersionData>  ver = std::make_shared<VersionData>();
                    ver->url = nameNode.toElement().text();
                    ver->version = latestVerNode.toElement().text();
                    ver->checksum = MD5Node.toElement().text();
                    ver->signature = signatureNode.toElement().text();
                    data->file_data.push_back(ver);
                }
            }
            else if(e.tagName() == "UpdateFiles")
            {
                QDomNodeList list = e.childNodes(); //获得元素e的所有子节点的列表

                for(int i=0; i<list.count(); i++) //遍历该列表
                {
                    QDomNode node = list.at(i);
                    QDomNode nameNode = node.childNodes().at(0);
                    QDomNode latestVerNode = node.childNodes().at(1);
                    QDomNode MD5Node = node.childNodes().at(2);
                    QDomNode signatureNode = node.childNodes().at(3);

                    std::shared_ptr<VersionData>  ver = std::make_shared<VersionData>();
                    ver->url = nameNode.toElement().text();
                    ver->version = latestVerNode.toElement().text();
                    ver->checksum = MD5Node.toElement().text();
                    ver->signature = signatureNode.toElement().text();
                    data->update_data.push_back(ver);
                }
            }
            else if(e.tagName() == "Version")
            {
                data->version = e.text();
            }
            else if("UpdateServer" == e.tagName())
            {
                data->serverPath = e.text();
            }
            else if("update" == e.tagName())
            {
                data->updateVersion = e.text();
            }
            else if("ForceUpdate" == e.tagName())
            {
                data->isUpdateForced = (e.text()=="true");
            }
            else if("MinimalVersion" == e.tagName())
            {
                data->minimalVersion = e.text();
            }

        }
        n = n.nextSibling();  //下一个兄弟节点
    }

    return true;
}

bool UpdateProgressUtil::MakeUpVersionData(VersionInfoPtr &data,const QString &headPath)
{
    if(!data) return false;
    for(auto it = data->file_data.begin();it != data->file_data.end();++it)
    {
        (*it)->url = headPath + "/"+(*it)->url;
    }
}

bool UpdateProgressUtil::ExtractUpdateData(const VersionInfoPtr &oldVersion, const VersionInfoPtr &newVersion,
                                           const QString &updateDir,const QString &mainDir,QList<DownLoadData> &data)
{
    data.clear();    

    if(!oldVersion || !newVersion) return false;
    //更新器文件
    for(auto newit = newVersion->update_data.begin();newit != newVersion->update_data.end();++newit)
    {
        auto oldit = std::find_if(oldVersion->update_data.begin(),oldVersion->update_data.end(),[newit](const std::shared_ptr<VersionData> &ve){
            return (*newit)->url == ve->url;
        });
        if(oldit == oldVersion->update_data.end() || (*oldit)->version != (*newit)->version)
        {
            DownLoadData down;
            down.filePath = updateDir + QDir::separator()+(*newit)->url;
            down.fileName = (*newit)->url;
            down.version = (*newit)->version;
            down.url = newVersion->serverPath + "/"+(*newit)->url;
            down.checksum = (*newit)->checksum;
            down.signature = (*newit)->signature;
            data.append(down);
        }
    }
    //主程序文件
    for(auto newit = newVersion->file_data.begin();newit != newVersion->file_data.end();++newit)
    {
        auto oldit = std::find_if(oldVersion->file_data.begin(),oldVersion->file_data.end(),[newit](const std::shared_ptr<VersionData> &ve){
            return (*newit)->url == ve->url;
        });
        if(oldit == oldVersion->file_data.end() || (*oldit)->version != (*newit)->version)
        {
            DownLoadData down;
            down.filePath = mainDir + QDir::separator()+(*newit)->url;
            down.fileName = (*newit)->url;
            down.version = (*newit)->version;
            down.url = newVersion->serverPath + "/"+(*newit)->url;
            down.checksum = (*newit)->checksum;
            down.signature = (*newit)->signature;
            data.append(down);
        }
    }
    return !data.empty();
}

bool UpdateProgressUtil::ParseXmlPath(const QString &fullPath, VersionInfoPtr &data)
{
    if(!data)
    {
        data = std::make_shared<VersionInfo>();
    }

    data->file_data.clear();

    QFile file(fullPath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"open file"<<fullPath << "failed!";
        return false;
    }

    QDomDocument xmlDoc;
    if(!xmlDoc.setContent(&file))
    {
        qDebug() << "localxml setcontent false "<<fullPath;
        file.close();
        return false;
    }

    file.close();
    return ParseXmlDoc(xmlDoc,data);
}

bool UpdateProgressUtil::deleteDir(const QString &dirName)
{
    QDir directory(dirName);
    if (!directory.exists())
    {
        return true;
    }

    QString srcPath = QDir::toNativeSeparators(dirName);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();

    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    bool error = false;
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString filePath = srcPath + fileNames.at(i);
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            QFile::setPermissions(filePath, QFile::WriteOwner);
            if (!QFile::remove(filePath))
            {
                qDebug() << "remove file" << filePath << " faild!";
                error = true;
            }
        }
        else if (fileInfo.isDir())
        {
            if (!deleteDir(filePath))
            {
                error = true;
            }
        }
    }

    if (!directory.rmdir(QDir::toNativeSeparators(directory.path())))
    {
        qDebug() << "remove dir" << directory.path() << " faild!";
        error = true;
    }

    return !error;
}

bool UpdateProgressUtil::copyDir(const QString &source, const QString &destination, bool override)
{
    QDir directory(source);
    if (!directory.exists())
    {
        return false;
    }

    QString srcPath = QDir::toNativeSeparators(source);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();
    QString dstPath = QDir::toNativeSeparators(destination);
    if (!dstPath.endsWith(QDir::separator()))
        dstPath += QDir::separator();

    bool error = false;
    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString fileName = fileNames.at(i);
        QString srcFilePath = srcPath + fileName;
        QString dstFilePath = dstPath + fileName;
        QFileInfo fileInfo(srcFilePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            if (override && QFile::exists(dstFilePath))
            {
                QFile::remove(dstFilePath);
            }
            QFile::copy(srcFilePath, dstFilePath);
        }
        else if (fileInfo.isDir())
        {
            QDir dstDir(dstFilePath);
            dstDir.mkpath(dstFilePath);
            if (!copyDir(srcFilePath, dstFilePath, override))
            {
                error = true;
            }
        }
    }

    return !error;
}
