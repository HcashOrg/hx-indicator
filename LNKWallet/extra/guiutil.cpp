// Copyright (c) 2011-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "guiutil.h"


#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDoubleValidator>
#include <QFileDialog>
#include <QFont>
#include <QLineEdit>
#include <QSettings>
#include <QTextDocument> // for Qt::mightBeRichText
#include <QThread>
#include <QMouseEvent>
#include <QCryptographicHash>

#include "extra/LogToFile.h"



namespace GUIUtil {


void ClickableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    Q_EMIT clicked(event->pos());
}



QString getFileMd5(const QString &sourceFilePath) {

    QFile sourceFile(sourceFilePath);
    int fileSize = static_cast<int>(sourceFile.size());
    const int bufferSize = 10240;

    if (sourceFile.open(QIODevice::ReadOnly)) {
        char buffer[bufferSize];
        int bytesRead=0;
        int readSize = static_cast<int>(qMin(fileSize, bufferSize));

        QCryptographicHash hash(QCryptographicHash::Md5);

        while (readSize > 0 && (bytesRead = static_cast<int>(sourceFile.read(buffer, readSize))) > 0) {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        sourceFile.close();
        return QString(hash.result().toHex());
    }
    return QString();
}

void checkAndLogMd5(const QString &nodePath,const QString &nodeMd5,const QString &clientPath,const QString &clientMd5){
    //获取当前后台的md5，对比发布的md5，写入log文件
    QString node_md5 = getFileMd5(nodePath);
    QString client_md5 = getFileMd5(clientPath);
    if(node_md5 != nodeMd5)
    {
        logToFile( QStringList() <<"WARNING: hx_node md5 mismatching. current:"<<node_md5<<" standard:"<<nodeMd5);
    }
    else
    {
        logToFile( QStringList() <<"LOG: hx_node md5 matched:"<<node_md5);
    }
    if(client_md5 != clientMd5)
    {
        logToFile( QStringList() <<"WARNING: hx_client md5 mismatching. current:"<<client_md5<<" standard:"<<clientMd5);
    }
    else
    {
        logToFile( QStringList() <<"LOG: hx_client md5 matched:"<<client_md5);
    }
}

} // namespace GUIUtil
