#ifndef POUNDAGEDATAUTIL_H
#define POUNDAGEDATAUTIL_H

#include <memory>
#include <vector>
#include <QString>

class PoundageUnit
{
public:
    PoundageUnit()
        :sourceCoinType("link")
        ,sourceCoinNumber(0)
        ,targetCoinNumber(0)
    {

    }
public:
    QString publishTime;//发行时间("yyyy/M/d HH:mm")
    QString sourceCoinType;//源币类型
    QString targetCoinType;//目标币类型
    double sourceCoinNumber;//源币数量
    double targetCoinNumber;//目标币数量
};

class PoundageSheet
{
public:
    PoundageSheet()
    {

    }
public:
    std::vector<std::shared_ptr<PoundageUnit>> poundages;
};

class PoundageDataUtil
{
public:
    PoundageDataUtil();

};

#endif // POUNDAGEDATAUTIL_H
