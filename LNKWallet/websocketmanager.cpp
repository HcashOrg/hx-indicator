#include "websocketmanager.h"

#include "wallet.h"

WebSocketManager::WebSocketManager(QObject *parent)
    : QThread(parent)
    , busy(true)
    , m_webSocket(NULL)
{

}

WebSocketManager::~WebSocketManager()
{
    if(m_webSocket)
    {
        delete m_webSocket;
        m_webSocket = NULL;
    }
}


void WebSocketManager::connectToClient()
{
    m_webSocket->open( QUrl(QString("ws://127.0.0.1:%1").arg(CLIENT_RPC_PORT)) );
}

void WebSocketManager::processRPC(QString _rpcId, QString _rpcCmd)
{
    busy = true;
    m_rpcId = _rpcId;
    m_buff.clear();
    m_webSocket->sendTextMessage(_rpcCmd);
}

void WebSocketManager::processRPCs(QString _rpcId, QString _rpcCmd)
{
    if(pendingRpcs.contains(_rpcId))      return;

    pendingRpcs.append(_rpcId + "***" + _rpcCmd);

    if(busy)
    {
        if(pendingRpcs.size() > 0)
        {
            qDebug() << "busy is " << pendingRpcs.at(0);
        }

        return;
    }

//retry:
//    if(!busy)
//    {
//        processRPC(_rpcId,_rpcCmd);
//    }
//    else
//    {
//        qDebug() << "busy is " << m_rpcId;
//    }

//    if( !processed)
//    {
//        if(loopCount > 100)
//        {
//            m_buff.clear();
//            m_rpcId.clear();
//            loopCount = 0;

//            busy = false;
//            return;
//        }
//        else
//        {
//            QThread::msleep(100);
//            loopCount++;
//            goto retry;
//        }

//    }
}

void WebSocketManager::run()
{
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimer()));
    timer->start(10);

    m_webSocket = new QWebSocket;
    connect(m_webSocket,SIGNAL(connected()),this,SLOT(onConnected()));
    connect(m_webSocket,SIGNAL(textFrameReceived(QString,bool)),this,SLOT(onTextFrameReceived(QString,bool)));
    connect(m_webSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(onStateChanged(QAbstractSocket::SocketState)));

    connectToClient();

    busy = false;

    exec();
}

void WebSocketManager::onTimer()
{
    if(pendingRpcs.size() > 0)
    {
        if(!busy)
        {
//            qDebug()<<pendingRpcs.size();
            QStringList rpc = pendingRpcs.at(0).split("***");
            processRPC(rpc.at(0),rpc.at(1));
        }
        else
        {
//            qDebug() << "busy is " << pendingRpcs.at(0);
        }

        loopCount++;
        if(loopCount > 100)
        {
            pendingRpcs.removeFirst();
            busy = false;
        }
    }

}

void WebSocketManager::onConnected()
{
    qDebug() << "websocket connected" << m_webSocket->state();

    isConnected = true;
}

void WebSocketManager::onTextFrameReceived(QString _message, bool _isLastFrame)
{
    if(pendingRpcs.size() <= 0)   return;
//    qDebug() << "message received: " << pendingRpcs.at(0) << _isLastFrame;

    m_buff += _message;

    loopCount = 0;

    if(_isLastFrame)
    {

        QString result = m_buff.mid( QString("{\"id\":32800,\"jsonrpc\":\"2.0\",").size());
        result = result.left( result.size() - 1);

        HXChain::getInstance()->updateJsonDataMap(pendingRpcs.at(0).split("***").at(0), result);

        pendingRpcs.removeFirst();

        m_buff.clear();

        busy = false;
    }
}

void WebSocketManager::onStateChanged(QAbstractSocket::SocketState _state)
{
    qDebug() << "websocket onStateChanged: " << _state << m_webSocket->errorString();

    if( _state == QAbstractSocket::UnconnectedState)
    {
        connectToClient();
    }
}
