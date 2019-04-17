#ifndef WITNESSCONFIG_H
#define WITNESSCONFIG_H

#include <QObject>
#include <QFile>
#include <QMutex>

class WitnessConfig : public QObject
{
    Q_OBJECT
public:
    explicit WitnessConfig(QObject *parent = 0);
    ~WitnessConfig();

    bool inited = false;
    int init();
    void save();                                // 所有操作调用save后才保存
    void modify(QString key, QString value, bool isString = false);    // 如果有key有多项 只修改了第一个 如果没有则append
    void append(QString key, QString value, bool isString = false);    // 在前面添加
    void remove(QString key);                   // 如果有key有多项 只删除了第一个
    QString value(QString key);                 // 如果有key有多项 只返回第一个 如果没有 返回""
    QStringList allValue(QString key);          // 一key多值的情况 获取全部值

    void addTrackAddress(QString address);
    QStringList getTrackAddresses();
    void addPrivateKey(QString pubKey, QString privateKey);
    void addMiner(QString minerId);
    QStringList getMiners();
    bool isProductionEnabled();
    void setProductionEnabled(bool enabled);
    QStringList getChainTypes();
    void setChainTypes(QStringList chainTypes);
signals:

public slots:

private:
    QFile* file = NULL;
    QStringList data;
    QMutex mutex;
};

#endif // WITNESSCONFIG_H
