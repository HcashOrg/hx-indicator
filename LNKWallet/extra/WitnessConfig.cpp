#include "WitnessConfig.h"

#include "wallet.h"

WitnessConfig::WitnessConfig(QObject *parent) : QObject(parent)
{

}

WitnessConfig::~WitnessConfig()
{
    if(file)
    {
        delete file;
    }
}

int WitnessConfig::init()
{
    // TODOTOMORROW
    file = new QSettings(HXChain::getInstance()->appDataPath + "/config.ini", QSettings::IniFormat);


    getTrackAddresses();
    return file->status();
}

QStringList WitnessConfig::getTrackAddresses()
{

    return QStringList();
}
