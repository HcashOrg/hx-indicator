#ifndef DEPOSITAUTOMATIC_H
#define DEPOSITAUTOMATIC_H

#include <QObject>
//////////////////////////////////////////////////////////////////////////
///<summary>自动充值类 </summary>
///
///<remarks> 2018.05.03 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
#include <QJsonObject>
class DepositAutomatic : public QObject
{
    Q_OBJECT
public:
    explicit DepositAutomatic(QObject *parent = 0);
    ~ DepositAutomatic();
signals:

public slots:
    void autoDeposit();
private:
    void PostQueryTunnelAddress(const QString &accountName,const QString &symbol);
    void PostQueryMultiAddress(const QString &symbol);
    void PostQueryTunnelMoney(const QString &symbol,const QString &tunnelAddress);
    void PostCreateTransaction(const QString &fromAddress,const QString &toAddress,
                               const QString &number,const QString &symbol);
    void PostSignTrasaction(const QString &fromAddress,const QString &symbol,const QJsonObject &detail);

    void FinishQueryTunnel();
    void FinishQueryMoney();
    void FinishQueryMulti();
    void FinishCreateTransaction();
    void FinishSign();
private slots:
    void jsonDataUpdated(const QString &id);
    void httpReplied(QByteArray _data, int _status);
private:
    void updateData();
private:
    class DataPrivate;
    DataPrivate *_p;
private:
    //工具
    void ParseTunnel(const QString &jsonString);
    void ParseMutli(const QString &jsonString);
    void ParseTransaction(const QString &address,const QString &jsonString);
};

#endif // DEPOSITAUTOMATIC_H
