#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QThread>
#include <QWebSocket>



class WebSocketManager : public QThread
{
    Q_OBJECT
public:
    explicit WebSocketManager(QObject *parent = 0);
    ~WebSocketManager();

    bool isConnected = false;

    void init();

    void connectToClient();

    void processRPC(QString _rpcId, QString _rpcCmd);
public slots:
    void processRPCs(QString _rpcId, QString _rpcCmd);
private:
    int loopCount = 0;

protected:
    void run();

signals:

private slots:
    void onConnected();
    void onTextFrameReceived(QString _message, bool _isLastFrame);



private:
    QWebSocket* m_webSocket;
    QString m_buff;
    QString m_rpcId;

    bool busy;
};

#endif // WEBSOCKETMANAGER_H
