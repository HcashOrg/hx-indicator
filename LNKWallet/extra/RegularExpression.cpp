#include "RegularExpression.h"

bool RegularExpression::testRegExp_IPV4(QString str)
{
    QRegExp rx("(2(5[0-5]{1}|[0-4]\\d{1})|[0-1]?\\d{1,2})(\\.(2(5[0-5]{1}|[0-4]\\d{1})|[0-1]?\\d{1,2})){3}");
    return  rx.exactMatch(str);
}

QRegExp RegularExpression::getRegExp_IPV4()
{
    QRegExp rx("(2(5[0-5]{1}|[0-4]\\d{1})|[0-1]?\\d{1,2})(\\.(2(5[0-5]{1}|[0-4]\\d{1})|[0-1]?\\d{1,2})){3}");
    return  rx;
}

bool RegularExpression::testRegExp_port(QString str)
{
    QRegExp rx("^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$)");
    return  rx.exactMatch(str);
}

QRegExp RegularExpression::getRegExp_port()
{
    QRegExp rx("^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$)");
    return  rx;
}
