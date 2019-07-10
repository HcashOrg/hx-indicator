#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QThread>
#include <QWebSocket>
#include <QTimer>


class WebSocketManager : public QThread
{
    Q_OBJECT
public:
    explicit WebSocketManager(QObject *parent = 0);
    ~WebSocketManager();

    bool isConnected = false;

    void connectToClient();

    void processRPC(QString _rpcId, QString _rpcCmd);
public slots:
    void processRPCs(QString _rpcId, QString _rpcCmd, int _priority);       // 仅不需要及时返回的数据查询priority可以大于0
                                                                             // 同一组查询priority务必相同
private:
    int loopCount = 0;

protected:
    void run();

signals:

private slots:
    void onTimer();
    void onConnected();
    void onTextFrameReceived(QString _message, bool _isLastFrame);
    void onBinaryFrameReceived(QByteArray _message, bool _isLastFrame);


    void onStateChanged(QAbstractSocket::SocketState _state);
private:
    QTimer* timer;
    QWebSocket* m_webSocket;
    QString m_buff;
    QString m_rpcId;
//    QStringList   pendingRpcs;
    QMap<int,QStringList>   pendingRpcs;        // qmap 自动按key值大小排序
    QString processingRpc;

    bool busy;
};

#endif // WEBSOCKETMANAGER_H
