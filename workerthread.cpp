#include "workerthread.h"
#include "lnk.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QTime>
#include <QJsonDocument>

QEvent::Type TaskEvent::s_evType = QEvent::None;

WorkerThread::WorkerThread(int id, QObject *parent)
    : QThread(parent)
    , m_id(id)
    , busy(true)
    , m_webSocket(NULL)
{
    qDebug() << "workthread construct " << m_id;
}


WorkerThread::~WorkerThread()
{
    qDebug() << "workthread destroy " << m_id;

    if(m_webSocket)
    {
        delete m_webSocket;
        m_webSocket = NULL;
    }
}

bool WorkerThread::isBusy()
{
    return busy;
}

void WorkerThread::setBusy(bool busyOrNot)
{
    busy = busyOrNot;
}

void WorkerThread::processRPC(QString _rpcId, QString _rpcCmd)
{
    busy = true;
qDebug() << "11111111111 " << _rpcId << _rpcCmd;
    m_rpcId = _rpcId;
    m_webSocket->sendTextMessage(_rpcCmd);
}

void WorkerThread::connectToClient()
{
    m_webSocket->open( QUrl(QString("ws://127.0.0.1:%1").arg(CLIENT_RPC_PORT)) );
}

void WorkerThread::run()
{
    m_webSocket = new QWebSocket;
    connect(m_webSocket,SIGNAL(connected()),this,SLOT(onConnected()));
    connect(m_webSocket,SIGNAL(textFrameReceived(QString,bool)),this,SLOT(onTextFrameReceived(QString,bool)));

    connectToClient();

    busy = false;

    exec();
}

bool WorkerThread::event(QEvent *e)
{
    if(e->type() == TaskEvent::evType())
    {

        processRPC( static_cast<TaskEvent*>(e)->rpcId, static_cast<TaskEvent*>(e)->rpcCmd);
        return true;
    }
    QThread::event(e);
}

void WorkerThread::onConnected()
{
    qDebug() << "websocket connected" << m_webSocket->state();

    isConnected = true;
}

void WorkerThread::onTextFrameReceived(QString _message, bool _isLastFrame)
{
    qDebug() << "message received: " << _message;

    m_buff += _message;

    if(_isLastFrame)
    {
        UBChain::getInstance()->updateJsonDataMap(m_rpcId, m_buff);
        UBChain::getInstance()->rpcReceivedOrNotMapSetValue(m_rpcId, true);

        m_buff.clear();
        m_rpcId.clear();
        busy = false;
    }
}

