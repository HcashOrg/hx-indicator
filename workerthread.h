#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QMutex>
#include <QEvent>
#include <QWebSocket>

#define LOGIN_USER "a"
#define LOGIN_PWD  "b"

class QTcpSocket;

class TaskEvent : public QEvent
{
public:
    TaskEvent(QString _rpcId, QString _rpcCmd)
        : QEvent(evType())
        , rpcId(_rpcId)
        , rpcCmd(_rpcCmd)
    {}

    static QEvent::Type evType()
    {
        if(s_evType == QEvent::None)
        {
            s_evType = (QEvent::Type)registerEventType();
        }
        return s_evType;
    }

    QString rpcId;
    QString rpcCmd;

private:
    static QEvent::Type s_evType;
};


class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(int workerId, QObject *parent = 0);
    ~WorkerThread();

    bool isBusy();

    void setBusy(bool busyOrNot);

    void processRPC(QString _rpcId, QString _rpcCmd);

    bool isConnected = false;
    void connectToClient();

protected:
    void run();
    bool event(QEvent *event);

private slots:
    void onConnected();
    void onTextFrameReceived(QString _message, bool _isLastFrame);

private:
    int m_id;
    QTcpSocket* socket;  // 在QThread中定义的所有东西都属于创建该QThread的线程。
                         // 如果不用指针 在构造时初始化socket， socket属于主线程
                         // 在run中 new 则属于该线程

    QWebSocket* m_webSocket;
    QString m_buff;
    QString m_rpcId;

    bool busy;

};

#endif // WORKERTHREAD_H
