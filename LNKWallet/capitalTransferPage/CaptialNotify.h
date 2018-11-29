#ifndef CAPTIALNOTIFY_H
#define CAPTIALNOTIFY_H

#include <QObject>
#include <QString>

class CaptialNotify : public QObject
{
    Q_OBJECT
public:
    explicit CaptialNotify(QObject *parent = nullptr);
    ~CaptialNotify();
signals:
    void checkTunnelMoneyFinish();
public:
    bool isAccountTunnelMoneyEmpty(const QString &accountName,const QString &assetSymbol)const;
public slots:
    void startCheckTunnelMoney();
    void stopCheckTunnelMoney();
private slots:
    void jsonDataUpdated(const QString &id);
    void httpReplied(QByteArray _data, int _status);
private:
    void updateData();
private:
    void PostQueryTunnelAddress(const QString &accountName,const QString &symbol);
    void FinishQueryTunnel();
    void ParseTunnel(const QString &jsonString);

    void PostQueryTunnelMoney(const QString &symbol,const QString &tunnelAddress);
    void FinishQueryMoney();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // CAPTIALNOTIFY_H
