#include "RegularExpression.h"

#include <QDebug>

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
    QRegExp rx("^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-5][0-9][0-9][0-9][0-9]$)"
               "|(^6[0-4][0-9][0-9][0-9]$)|(^65[0-4][0-9][0-9]$)|(^655[0-2][0-9]$)|(^6553[0-5]$)|");
    return  rx.exactMatch(str);
}

QRegExp RegularExpression::getRegExp_port()
{
    QRegExp rx("^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-5][0-9][0-9][0-9][0-9]$)"
               "|(^6[0-4][0-9][0-9][0-9]$)|(^65[0-4][0-9][0-9]$)|(^655[0-2][0-9]$)|(^6553[0-5]$)|");
    return  rx;
}

void RegularExpression::test()
{
    for(int  i = 0; i <= 65536; i++)
    {
        QString str = QString::number(i);
        if(!testRegExp_port(str))
        {
            qDebug() << str;
        }
    }
}
