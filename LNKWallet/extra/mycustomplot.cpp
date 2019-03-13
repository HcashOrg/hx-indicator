#include <QObject>

#include "mycustomplot.h"


void MyQCPAxisTickerDateTime::setTickTimePointType(MyQCPAxisTickerDateTime::TimePointType type)
{
    mTimePointType = type;
}

QString MyQCPAxisTickerDateTime::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision)
{
    Q_UNUSED(precision)
    Q_UNUSED(formatChar)

    QString format = mDateTimeFormat;
    if(mTimePointType == tpDay)
    {
        if(keyToDateTime(tick).toTime_t() % (3600 * 24) == 0)
        {
            format = QObject::tr("dd.MMMM");
        }
    }
    return locale.toString(keyToDateTime(tick).toTimeSpec(mDateTimeSpec), format);
}
