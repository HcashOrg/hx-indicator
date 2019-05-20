#ifndef REGULAREXPRESSION_H
#define REGULAREXPRESSION_H

#include <QRegExp>

namespace RegularExpression {

    bool testRegExp_IPV4(QString str);
    QRegExp getRegExp_IPV4();

    bool testRegExp_port(QString str);
    QRegExp getRegExp_port();

    void test();
}


#endif // REGULAREXPRESSION_H
