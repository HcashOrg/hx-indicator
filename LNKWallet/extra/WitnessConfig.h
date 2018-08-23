#ifndef WITNESSCONFIG_H
#define WITNESSCONFIG_H

#include <QObject>

class WitnessConfig : public QObject
{
    Q_OBJECT
public:
    explicit WitnessConfig(QObject *parent = 0);

signals:

public slots:
};

#endif // WITNESSCONFIG_H