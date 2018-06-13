#include "DataUtil.h"

#include <QDir>
#include <QFile>
#include <QDebug>
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
