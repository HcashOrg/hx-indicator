#include "LogToFile.h"


void logToFile(const QStringList &msg, int level, QString fileName)
{
    static QMutex mutex;
    mutex.lock();

    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ");
    QString strMessage = QString("DateTime:%1 Message:%2").arg(strDateTime).arg(msg.join(" "));

    // 输出信息至文件中（读写、追加形式），超过50M删除日志
    if(fileName.isEmpty())      fileName = "hxindicator_log.txt";
    QFileInfo info(fileName);
    if(info.size() > 1024*1024*50) QFile::remove(fileName);
    QFile file(fileName);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\n";
    file.flush();
    file.close();

    mutex.unlock();
}
