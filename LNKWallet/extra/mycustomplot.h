#ifndef MYCUSTOMPLOT_H
#define MYCUSTOMPLOT_H

#include "qcustomplot.h"

class MyQCPAxisTickerDateTime : public QCPAxisTickerDateTime
{
public:
    enum TimePointType {tpNone,tpYear,tpMonth,tpDay} mTimePointType;
    void setTickTimePointType(TimePointType type);
    virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) Q_DECL_OVERRIDE;
};

#endif // MYCUSTOMPLOT_H

