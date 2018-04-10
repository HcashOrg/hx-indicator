#ifndef POUNDAGEDATAUTIL_H
#define POUNDAGEDATAUTIL_H

#include <memory>
#include <vector>
#include <QString>
#include <QDateTime>

class PoundageUnit
{
public:
    PoundageUnit()
        :sourceCoinID("1.3.0")
        ,chainType("BTC")
        ,sourceCoinNumber(0)
        ,targetCoinNumber(0)
    {

    }
public:
    QString poundageID;//手续费ID
    QString ownerAdress;//发行人地址
    QDateTime publishTime;//发行时间
    QString chainType;//发行税单类型
    QString sourceCoinID;//源币类型
    QString targetCoinID;//目标币类型
    double sourceCoinNumber;//源币数量
    double targetCoinNumber;//目标币数量
    double balanceNumber;//账户余额
};

class PoundageSheet
{
public:
    PoundageSheet()
    {

    }
    void clear()
    {
        poundages.clear();
    }
    void push_back(const std::shared_ptr<PoundageUnit> &data)
    {
        poundages.push_back(data);
    }
    int size()
    {
        return static_cast<int>(poundages.size());
    }
    //true表示，按照时间由早到晚
    void sortByTime(bool greater = true)
    {
        if(greater)
        {
            std::stable_sort(poundages.begin(),poundages.end(),[](const std::shared_ptr<PoundageUnit> &left,const std::shared_ptr<PoundageUnit> &right){
                    return left->publishTime < right->publishTime;
            });
        }
        else
        {
            std::stable_sort(poundages.begin(),poundages.end(),[](const std::shared_ptr<PoundageUnit> &left,const std::shared_ptr<PoundageUnit> &right){
                    return left->publishTime > right->publishTime;
            });
        }
    }
    //按链类型删选
    void filterByChainType(const QString &chainType)
    {
        poundages.erase(std::remove_if(poundages.begin(),poundages.end(),[chainType](std::shared_ptr<PoundageUnit> info){
                                       return info->chainType != chainType;})
                        ,poundages.end());
    }
    //按汇率排序,true表示按汇率由低到高
    void sortByRate(bool greater = true)
    {
        std::stable_sort(poundages.begin(),poundages.end(),[greater](const std::shared_ptr<PoundageUnit> &left,const std::shared_ptr<PoundageUnit> &right){
            return greater ? left->sourceCoinNumber/left->targetCoinNumber < right->sourceCoinNumber/right->targetCoinNumber
                           : left->sourceCoinNumber/left->targetCoinNumber > right->sourceCoinNumber/right->targetCoinNumber;
        });
    }


public:
    std::vector<std::shared_ptr<PoundageUnit>> poundages;
};

class PoundageDataUtil
{
public:
    PoundageDataUtil();
public:
    static bool convertJsonToPoundage(const QString &jsonString,std::shared_ptr<PoundageSheet> &sheet);

};

#endif // POUNDAGEDATAUTIL_H
