#ifndef WORKERTHREADMANAGER_H
#define WORKERTHREADMANAGER_H

#include "workerthread.h"

#include <QTimer>

#define NUM_OF_WORKERTHREADS  1

class WorkerThreadManager:public QObject
{
    Q_OBJECT
public:
    WorkerThreadManager();
    ~WorkerThreadManager();

signals:
    void allConnected();
private slots:
    void checkConnected();
private:
    QTimer* timer;

public slots:
    void processRPCs(QString _rpcId, QString _rpcCmd);

private:
    QMap<int, WorkerThread*> workerThreads;
    bool running;
};

#endif // WORKERTHREADMANAGER_H
