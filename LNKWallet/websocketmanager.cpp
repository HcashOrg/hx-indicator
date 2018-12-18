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

//    logToFile( QStringList() << QString("rpc posted: %1 %2").arg(_rpcId)
//               .arg(_rpcCmd) );

}

void WebSocketManager::processRPCs(QString _rpcId, QString _rpcCmd, int _priority)
{
//    if(pendingRpcs.contains(_rpcId))      return;

//    pendingRpcs.append(_rpcId + "***" + _rpcCmd);

    if(pendingRpcs.value(_priority).contains(_rpcId))   return;
    pendingRpcs[_priority].append(_rpcId + "***" + _rpcCmd);

//    if(busy)
//    {
//        if(pendingRpcs.size() > 0)
//        {
////            qDebug() << "busy is " << pendingRpcs.at(0);
//        }

//        return;
//    }

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
    timer->start(1);

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
//    if(pendingRpcs.size() > 0)
//    {
//        if(!busy)
//        {
////            qDebug()<<pendingRpcs.size();
//            QStringList rpc = pendingRpcs.at(0).split("***");
//            processRPC(rpc.at(0),rpc.at(1));
//        }
//        else
//        {
////            qDebug() << "busy is " << pendingRpcs.at(0);
//        }

////        loopCount++;
////        if(loopCount > 1000)
////        {
////            pendingRpcs.removeFirst();
////            busy = false;
////        }
//    }

    if( !processingRpc.isEmpty())
    {
        loopCount++;
        if(loopCount >= 1000 && loopCount % 1000 == 0 )
        {
            logToFile( QStringList() << QString("rpc timeout: %1 %2").arg(QString::number(loopCount / 1000))
                       .arg(processingRpc) );
        }

        if(loopCount >= 60000 && loopCount % 60000 == 0)
        {
            processRPC("testrpc+info", toJsonFormat("info", QJsonArray()));
            logToFile( QStringList() << QString("test rpc connect: %1 ").arg(QString::number(loopCount / 60000)));
        }
        return;
    }

    if(!busy)
    {
        foreach(int i, pendingRpcs.keys())
        {
            QStringList& rpcs = pendingRpcs[i];
            if(rpcs.size() == 0)    continue;

            processingRpc = rpcs.takeFirst();
            QStringList rpc = processingRpc.split("***");
            processRPC(rpc.at(0),rpc.at(1));

            return;
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
    if(m_rpcId == "testrpc+info")
    {
        // 如果 rpc还能连接 则丢弃当前 继续处理余下的rpc指令
        m_buff.clear();
        m_rpcId.clear();
        loopCount = 0;
        processingRpc.clear();
        busy = false;

        logToFile( QStringList() << QString("test rpc connect: successfully "));

        return;
    }

    if(processingRpc.isEmpty())   return;
//    if(pendingRpcs.size() <= 0)   return;
//    qDebug() << "message received: " << pendingRpcs.at(0) << _isLastFrame;

    m_buff += _message;
    logToFile( QStringList() << QString::number(loopCount) << processingRpc);
    loopCount = 0;

    if(_isLastFrame)
    {

        QString result = m_buff.mid( QString("{\"id\":32800,\"jsonrpc\":\"2.0\",").size());
        result = result.left( result.size() - 1);

        HXChain::getInstance()->updateJsonDataMap(processingRpc.split("***").at(0), result);

//        pendingRpcs.removeFirst();
        processingRpc.clear();

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
