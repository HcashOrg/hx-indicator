#ifndef LOGTOFILE_H
#define LOGTOFILE_H

#include <QtCore>

void logToFile(const QStringList& msg, int level = 0, QString fileName = "");

#endif // LOGTOFILE_H
