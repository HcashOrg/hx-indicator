#ifndef WITNESSCONFIG_H
#define WITNESSCONFIG_H

#include <QObject>
#include <QSettings>

class WitnessConfig : public QObject
{
    Q_OBJECT
public:
    explicit WitnessConfig(QObject *parent = 0);
    ~WitnessConfig();

    int init();

    QStringList getTrackAddresses();
signals:

public slots:

private:
    QSettings* file = NULL;
};

#endif // WITNESSCONFIG_H
