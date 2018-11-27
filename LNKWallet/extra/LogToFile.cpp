#include "LogToFile.h"


void logToFile(const QStringList &msg, int level)
{
    static QMutex mutex;
    mutex.lock();

    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ");
    QString strMessage = QString("DateTime:%1 Message:%2").arg(strDateTime).arg(msg.join(" "));

    // 输出信息至文件中（读写、追加形式），超过50M删除日志
    QFileInfo info("hxindicator_log.txt");
    if(info.size() > 1024*1024*50) QFile::remove("hxindicator_log.txt");
    QFile file("hxindicator_log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\n";
    file.flush();
    file.close();

    mutex.unlock();
}
