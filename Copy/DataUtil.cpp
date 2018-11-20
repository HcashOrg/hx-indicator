#include "DataUtil.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QMutex>
#include "quazip/quazip/quazip.h"
#include "quazip/quazip/quazipfile.h"

DataUtil::DataUtil()
{

}

bool DataUtil::deleteDir(const QString &dirName)
{
    qDebug()<<dirName;
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

bool DataUtil::copyDir(const QString &source, const QString &destination, bool override)
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
                //qDebug()<<"override"<<dstFilePath;
                //QFile::setPermissions(dstFilePath, QFile::WriteOwner);
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

bool DataUtil::unCompress(const QString &in_file_path, const QString &out_file_path)
{
    QuaZip archive(in_file_path);
        if (!archive.open(QuaZip::mdUnzip))
            return false;

        QString path = out_file_path;
        if (!path.endsWith("/") && !out_file_path.endsWith("\\"))
            path += "/";

        QDir dir(out_file_path);
        if (!dir.exists())
            dir.mkpath(out_file_path);

        for( bool f = archive.goToFirstFile(); f; f = archive.goToNextFile() )
        {
            QString filePath = archive.getCurrentFileName();
            QuaZipFile zFile(archive.getZipName(), filePath);
            zFile.open(QIODevice::ReadOnly );
            QByteArray ba = zFile.readAll();
            zFile.close();

            if (filePath.endsWith("/"))
            {
                dir.mkpath(filePath);
            }
            else
            {
                QFile dstFile(path + filePath);
                if (!dstFile.open(QIODevice::WriteOnly))
                    return false;
                dstFile.write(ba);
                dstFile.close();
            }
        }

        return true;
}
void DataUtil::myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("DateTime:%1 Message:%2").arg(strDateTime).arg(localMsg.constData());

    // 输出信息至文件中（读写、追加形式），超过50M删除日志
    QFileInfo info("update_log.txt");
    if(info.size() > 1024*1024*50) QFile::remove("update_log.txt");
    QFile file("update_log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    // 解锁
    mutex.unlock();
}
