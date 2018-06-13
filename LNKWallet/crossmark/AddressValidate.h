#ifndef ADDRESSVALIDATE_H
#define ADDRESSVALIDATE_H

#include <QObject>

class AddressValidate : public QObject
{
    Q_OBJECT
public:
    explicit AddressValidate(QObject *parent = 0);

signals:
    void AddressValidateSignal(bool);//合法返回true，否则返回false
public slots:
    void startValidateAddress(const QString &chainId,const QString &address);
private slots:
    void httpReplied(QByteArray _data, int _status);
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // ADDRESSVALIDATE_H
