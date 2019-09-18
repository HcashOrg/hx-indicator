#include "websocketmanager.h"

#include "wallet.h"

#ifdef LIGHT_MODE
#define RPC_TIMEOUT  3000
#else
#define RPC_TIMEOUT  1000       // rpc超时时间
#endif

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
//    connect(m_webSocket,SIGNAL(binaryFrameReceived(QByteArray,bool)),this,SLOT(onBinaryFrameReceived(QByteArray,bool)));

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

#ifdef PERFORMANCE_ANALYSIS
   timeElapse += timer->interval();

//   if(timeElapse % (1000 * 60 * 1) == 0)
//   {
//       logToFile( QStringList() << "1 minute:" << formatStaticsInfoMap(), 0, "performance_analysis.txt");
//   }

   if(timeElapse % (1000 * 60 * 5) == 0)
   {
       logToFile( QStringList() << "5 minutes:" << formatStaticsInfoMap(), 0, "performance_analysis.txt");
   }

   if(timeElapse % (1000 * 60 * 60) == 0)
   {
       QStringList keys = rpcStatisticsInfoMap.keys();
       int totalTime = 0;
       int tatalTimeOutCount = 0;
       foreach (QString key, keys)
       {
           StatisticInfo info = rpcStatisticsInfoMap.value(key);
           totalTime += info.consumingTime;
           tatalTimeOutCount += info.timeOutCount;
       }

       logToFile( QStringList() << "1 hour data summary:" << QString("total time consumed: %1,\n total time out:%2,\n time elapse:%3,\n ratio: %4\n")
                  .arg(totalTime).arg(tatalTimeOutCount).arg(timeElapse).arg(double(totalTime) / timeElapse),
                  0, "performance_analysis.txt");
   }


#endif

    if( !processingRpc.isEmpty())
    {
        loopCount++;
        if(loopCount >= RPC_TIMEOUT && loopCount % RPC_TIMEOUT == 0 )
        {
            logToFile( QStringList() << QString("rpc timeout: %1 %2").arg(QString::number(loopCount / 1000))
                       .arg(processingRpc) );

#ifdef PERFORMANCE_ANALYSIS
            QStringList rpc = processingRpc.split("***");
            rpcStatisticsInfoMap[formatRpcKey(rpc.at(0))].timeOutCount += 1;
#endif
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

#ifdef PERFORMANCE_ANALYSIS
            rpcStatisticsInfoMap[formatRpcKey(rpc.at(0))].callCount += 1;
#endif

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

    m_buff += _message;

#ifdef PERFORMANCE_ANALYSIS
    QStringList rpc = processingRpc.split("***");
    rpcStatisticsInfoMap[formatRpcKey(rpc.at(0))].consumingTime += loopCount;
#endif

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

void WebSocketManager::onBinaryFrameReceived(QByteArray _message, bool _isLastFrame)
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

//    if(processingRpc.startsWith("NameTransferPage-decode_multisig_transaction-"))
//    {
        qDebug() <<"bbbbbbbbbbbb " << _isLastFrame <<_message;
//    }

    if(processingRpc.isEmpty())   return;
//    if(pendingRpcs.size() <= 0)   return;
//    qDebug() << "message received: " << pendingRpcs.at(0) << _isLastFrame;

    m_buff += _message;
//    logToFile( QStringList() << QString::number(loopCount) << processingRpc);
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

#ifdef PERFORMANCE_ANALYSIS
QString WebSocketManager::formatStaticsInfoMap()
{
    QJsonObject object;
    QStringList keys = rpcStatisticsInfoMap.keys();
    foreach (QString key, keys)
    {
        StatisticInfo info = rpcStatisticsInfoMap.value(key);
        QString str = QString("%1  %2  %3, %4").arg(info.callCount).arg(info.consumingTime).arg(info.timeOutCount).arg(double(info.consumingTime) / info.callCount);
        object.insert(key, str);
    }

    return QJsonDocument(object).toJson();
}

QString WebSocketManager::formatRpcKey(QString rpcId)
{
    QString result = rpcId;
    if(rpcId.count("-") >=2)
    {
        result = rpcId.left(rpcId.indexOf("-",rpcId.indexOf("-") + 1));
    }
    else if(rpcId.count("+") >= 2)
    {
        result = rpcId.left(rpcId.indexOf("+",rpcId.indexOf("+") + 1));
    }

    return result;
}
#endif
