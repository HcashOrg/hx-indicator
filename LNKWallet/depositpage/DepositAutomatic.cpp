#include "DepositAutomatic.h"

#include "wallet.h"
#include <mutex>
#include <memory>
#include <list>

#include <QTimer>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "extra/HttpManager.h"

class DepositAutomatic::DataPrivate
{
public:
    struct accountInfo
    {
        accountInfo()
            :captialFee(0)
            ,transactionFinished(false)
        {

        }

        QString accountAddress;
        QString accountName;
        QString tunnelAddress;
        QString assetSymbol;
        QString assetMultiAddress;
        QString assetNumber;
        double captialFee;

        QJsonObject transactionDetail;
        bool transactionFinished;
    };
public:
    DataPrivate()
        :autoDeposit(HXChain::getInstance()->autoDeposit)
        ,isInUpdate(false)
    {

    }
public:
    bool isInUpdate;//是否处于更新阶段，防止重复更新
    bool autoDeposit;//是否自动充值
    std::mutex mutexLock;//数据修改锁
    std::mutex updateMutexLock;//更新标志锁
    std::list<std::shared_ptr<accountInfo>> accounts;//所有账户信息
    std::list<QString> assetSymbols;//资产类型
    QTimer *timer;
    HttpManager httpManager;////用于查询通道账户余额

public:
    //清理空tunnel账户
    void clearEmptyTunnel()
    {
        accounts.erase(std::remove_if(accounts.begin(),accounts.end(),[](std::shared_ptr<accountInfo> info){
                                       return info->tunnelAddress.isEmpty();})
                        ,accounts.end());
    }


    //是否已有通道地址
    bool findTunel(const QString &tunnel){
        for(auto it = accounts.begin();it != accounts.end();++it)
        {
            if(!(*it)->tunnelAddress.isEmpty() && tunnel == (*it)->tunnelAddress){
                return true;
            }

        }
        return false;
    }
    //更新通道账户
    void updateTunnel(const QString &accountAddress,const QString &symbol,const QString &tunnelAddress){
        std::lock_guard<std::mutex> lockguard(mutexLock);
        for(auto it = accounts.begin();it != accounts.end();++it){
            if(accountAddress == (*it)->accountAddress && symbol==(*it)->assetSymbol){
                (*it)->tunnelAddress = tunnelAddress;
                break;
            }
        }
    }
    //更新多签地址
    void updateMulti(const QString &symbol,const QString &multi){
        std::lock_guard<std::mutex> lockguard(mutexLock);
        for(auto it = accounts.begin();it != accounts.end();++it){
            if(symbol == (*it)->assetSymbol){
                (*it)->assetMultiAddress = multi;
            }
        }
    }
    //更新账户余额
    void updateMoney(const QString &tunnelAddress,const QString &number){
        std::lock_guard<std::mutex> lockguard(mutexLock);
        for(auto it = accounts.begin();it != accounts.end();++it){
            if(tunnelAddress == (*it)->tunnelAddress){
                int pre = 5;
                foreach(AssetInfo asset,HXChain::getInstance()->assetInfoMap){
                    if(asset.symbol == (*it)->assetSymbol)
                    {
                        pre = asset.precision;
                        break;
                    }
                }
                (*it)->assetNumber = QString::number(std::max<double>(0,number.toDouble()-(*it)->captialFee)
                                                     ,'f',pre);
                //qDebug()<<"update-money---"<<tunnelAddress<<(*it)->assetMultiAddress<<(*it)->assetNumber;
                if((*it)->assetNumber.toDouble() < 1e-11)
                {
                    accounts.erase(it);
                }
                break;
            }
        }
    }
    //更新交易详情
    void updateDetail(const QString &tunnelAddress,const QJsonObject &obj){
        std::lock_guard<std::mutex> lockguard(mutexLock);
        for(auto it = accounts.begin();it != accounts.end();++it){
            if(tunnelAddress == (*it)->tunnelAddress){
                (*it)->transactionDetail = obj;
                qDebug()<<"signsign"<<tunnelAddress<<(*it)->transactionDetail;
                break;
            }
        }
    }
    //设置正在更新
    void SetInUpdateTrue()
    {
        std::lock_guard<std::mutex> lockguard(updateMutexLock);
        isInUpdate = true;
    }
    void SetInUpdateFalse()
    {
        std::lock_guard<std::mutex> lockguard(updateMutexLock);
        isInUpdate = false;
    }
};

DepositAutomatic::DepositAutomatic(QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate())
{
    _p->timer = new QTimer(this);
    connect(_p->timer,&QTimer::timeout,this,&DepositAutomatic::autoDeposit);
    _p->timer->start(30000);

    connect( HXChain::getInstance(), &HXChain::jsonDataUpdated, this, &DepositAutomatic::jsonDataUpdated);

    connect(&_p->httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));
    //test
    //autoDeposit();
}

DepositAutomatic::~DepositAutomatic()
{
   delete _p;
    _p = nullptr;
}

void DepositAutomatic::autoDeposit()
{
    _p->autoDeposit = HXChain::getInstance()->autoDeposit;
    if(_p->autoDeposit && !_p->isInUpdate && HXChain::getInstance()->GetBlockSyncFinish())
    {
        //_p->timer->stop();
        updateData();
    }
}

void DepositAutomatic::PostQueryTunnelAddress(const QString &accountName, const QString &symbol)
{
    if(accountName.isEmpty() || symbol.isEmpty()) return;
    HXChain::getInstance()->postRPC( "automatic-get_binding_account-"+accountName,
                                     toJsonFormat( "get_binding_account", QJsonArray()
                                     << accountName<<symbol ));

}

void DepositAutomatic::PostQueryMultiAddress(const QString &symbol)
{
    if(symbol.isEmpty()) return;
    HXChain::getInstance()->postRPC( "automatic-get_current_multi_address_"+symbol,
                                     toJsonFormat( "get_current_multi_address", QJsonArray()
                                     << symbol));

}

void DepositAutomatic::PostQueryTunnelMoney(const QString &symbol,const QString &tunnelAddress)
{
    if(symbol.isEmpty() || tunnelAddress.isEmpty()) return;
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","Zchain.Address.GetBalance");
    QJsonObject paramObject;
    paramObject.insert("chainId",symbol);
    paramObject.insert("addr",tunnelAddress);
    object.insert("params",paramObject);
    _p->httpManager.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());
}

void DepositAutomatic::PostCreateTransaction(const QString &fromAddress, const QString &toAddress,
                                             const QString &number, const QString &symbol)
{
//    for(auto it = _p->accounts.begin();it != _p->accounts.end();++it)
//    {
//        qDebug()<<"justbeforetransaction"<<(*it)->tunnelAddress<<(*it)->assetSymbol<<(*it)->assetMultiAddress<<(*it)->assetNumber;
//    }
    if(fromAddress.isEmpty() || toAddress.isEmpty() || number.isEmpty() || symbol.isEmpty() || number.toDouble()<1e-10) return;
    HXChain::getInstance()->postRPC( "automatic-createrawtransaction_"+fromAddress,
                                     toJsonFormat( "createrawtransaction", QJsonArray()
                                     << fromAddress<<toAddress<<number<<symbol ));

//    qDebug()<<"auto-post-trasaction----"<<toJsonFormat( "createrawtransaction", QJsonArray()
//                                                        << fromAddress<<toAddress<<number<<symbol );

}

void DepositAutomatic::PostSignTrasaction(const QString &fromAddress, const QString &symbol,
                                          const QJsonObject &detail)
{
    if(fromAddress.isEmpty() || symbol.isEmpty() || detail.isEmpty()) return;
    HXChain::getInstance()->postRPC( "automatic-signrawtransaction",
                                     toJsonFormat( "signrawtransaction", QJsonArray()
                                     <<fromAddress<<symbol<<detail<<true ));

}

void DepositAutomatic::FinishQueryTunnel()
{
    HXChain::getInstance()->postRPC( "automatic-finish-tunnel",
                                     toJsonFormat( "automatic-finish-tunnel", QJsonArray()));

}

void DepositAutomatic::FinishQueryMoney()
{
    HXChain::getInstance()->postRPC( "automatic-finish-money",
                                     toJsonFormat( "automatic-finish-money", QJsonArray()));

}

void DepositAutomatic::FinishQueryMulti()
{
    HXChain::getInstance()->postRPC( "automatic-finish-multi",
                                     toJsonFormat( "automatic-finish-multi", QJsonArray()));

}

void DepositAutomatic::FinishCreateTransaction()
{
    HXChain::getInstance()->postRPC( "automatic-finish-transaction",
                                     toJsonFormat( "automatic-finish-transaction", QJsonArray()));

}

void DepositAutomatic::FinishSign()
{
    HXChain::getInstance()->postRPC( "automatic-finish-sign",
                                     toJsonFormat( "automatic-finish-sign", QJsonArray()));

}

void DepositAutomatic::jsonDataUpdated(const QString &id)
{
    if(id.startsWith("automatic-get_binding_account-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        result.prepend("{");
        result.append("}");
    //提取通道账户地址
        ParseTunnel(result);

    }
    else if("automatic-finish-tunnel" == id)
    {
        //先排除没有tunnel的账户
        _p->clearEmptyTunnel();
        //qDebug()<<_p->accounts.size();
        //开始查找多签地址
        std::for_each(_p->assetSymbols.begin(),_p->assetSymbols.end(),[this](const QString &symbol){
            this->PostQueryMultiAddress(symbol);
        });
        //结束查找
        FinishQueryMulti();
    }
    else if(id.startsWith("automatic-get_current_multi_address_"))
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        result.prepend("{");
        result.append("}");
        ParseMutli(result);
    }
    else if("automatic-finish-multi" == id)
    {//开始查找余额
        for(auto it = _p->accounts.begin();it != _p->accounts.end();++it)
        {
            PostQueryTunnelMoney((*it)->assetSymbol,(*it)->tunnelAddress);
        }
//        std::for_each(_p->accounts.begin(),_p->accounts.end(),[this](const std::shared_ptr<DepositAutomatic::DataPrivate::accountInfo>& info){
//            this->PostQueryTunnelMoney(info->assetSymbol,info->tunnelAddress);
//        });

        QTimer::singleShot(500,[this](){this->PostQueryTunnelMoney("autofinish","autofinish");});
    }
    else if("automatic-finish-money" == id)
    {//开始创建交易

        std::for_each(_p->accounts.begin(),_p->accounts.end(),[this](const std::shared_ptr<DepositAutomatic::DataPrivate::accountInfo>& info){
            this->PostCreateTransaction(info->tunnelAddress,info->assetMultiAddress,info->assetNumber,info->assetSymbol);
            //qDebug()<<"postdata"<<info->tunnelAddress<<info->assetMultiAddress<<info->assetNumber<<info->assetSymbol;
        });
    //结束交易创建
        FinishCreateTransaction();
    }
    else if(id.startsWith("automatic-createrawtransaction_"))
    {
        QString address = id.mid(QString("automatic-createrawtransaction_").size());

        QString result = HXChain::getInstance()->jsonDataValue( id);
        result.prepend("{");
        result.append("}");
        ParseTransaction(address,result);
//        qDebug()<<id<<"-----"<<HXChain::getInstance()->jsonDataValue( id);
    }
    else if("automatic-finish-transaction" == id)
    {//进行签名
        std::for_each(_p->accounts.begin(),_p->accounts.end(),[this](const std::shared_ptr<DepositAutomatic::DataPrivate::accountInfo>& info){
            this->PostSignTrasaction(info->tunnelAddress,info->assetSymbol,info->transactionDetail);
        });
        FinishSign();

    }
    else if("automatic-signrawtransaction" == id)
    {
        qDebug()<<id<<"-----"<<HXChain::getInstance()->jsonDataValue( id);
    }
    else if("automatic-finish-sign" == id)
    {//结束签名,可以进行刷新
        _p->SetInUpdateFalse();
    }
}

void DepositAutomatic::httpReplied(QByteArray _data, int _status)
{//解析查询余额的返回，补全账户信息
//    qDebug() << "auto--http-- " << _data << _status;

    if(QString(_data).contains("autofinish"))
    {
        //结束查找余额--必须等余额查完了再创建交易--余额的查询很慢
        FinishQueryMoney();

        for(auto it = _p->accounts.begin();it != _p->accounts.end();++it)
        {
            qDebug()<<"justafter--tunnel-money"<<(*it)->tunnelAddress<<(*it)->assetSymbol<<(*it)->assetMultiAddress<<(*it)->assetNumber;
        }
        return;
    }

    QJsonObject object  = QJsonDocument::fromJson(_data).object().value("result").toObject();
    QString tunnel   = object.value("address").toString();
    QString number = QString::number( jsonValueToDouble(object.value("balance")));



    _p->updateMoney(tunnel,number);

}

void DepositAutomatic::updateData()
{
    std::lock_guard<std::mutex> guard(_p->mutexLock);
    _p->SetInUpdateTrue();
    _p->accounts.clear();
    _p->assetSymbols.clear();
    _p->autoDeposit = HXChain::getInstance()->autoDeposit;

    QMap<QString,AccountInfo> accountInfoMap = HXChain::getInstance()->accountInfoMap;
    QMap<QString,AssetInfo> assetInfoMap = HXChain::getInstance()->assetInfoMap;

    foreach (AccountInfo info, accountInfoMap) {
        foreach(AssetInfo assetInfo,assetInfoMap){
            if(assetInfo.id != "1.3.0" && assetInfo.symbol != "ETH" && !assetInfo.symbol.startsWith("ERC"))
            {
                std::shared_ptr<DataPrivate::accountInfo> da = std::make_shared<DataPrivate::accountInfo>();
                da->accountAddress = info.address;
                da->accountName = info.name;
                da->assetSymbol = assetInfo.symbol;
                da->captialFee = static_cast<double>(assetInfo.fee)/std::max<int>(1,assetInfo.precision);
                _p->accounts.push_back(da);
            }
        }
    }
    foreach(AssetInfo assetInfo,assetInfoMap){
        if(assetInfo.id != "1.3.0" && assetInfo.symbol != "ETH" && !assetInfo.symbol.startsWith("ERC"))
        {
            _p->assetSymbols.push_back(assetInfo.symbol);
        }
    }

    if(_p->accounts.empty())
    {
       _p->SetInUpdateFalse();
        return;
    }
    //发送寻找多签地址、以及对应通道地址
    for(const auto &in : _p->accounts)
    {
        PostQueryTunnelAddress(in->accountName,in->assetSymbol);
    }
    FinishQueryTunnel();
}

void DepositAutomatic::ParseTunnel(const QString &jsonString)
{
//    qDebug()<< "auto--"<<jsonString;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return ;
    QJsonObject jsonObject = parse_doucment.object();

    if(!jsonObject.value("result").isArray())
    {
        return ;
    }

    QJsonArray object = jsonObject.value("result").toArray();
    if(object.isEmpty()) return ;

    QString accountAddress = object[0].toObject().value("owner").toString();
    QString symbol = object[0].toObject().value("chain_type").toString();
    QString tunnelAddress = object[0].toObject().value("bind_account").toString();
    if(!accountAddress.isEmpty() && !accountAddress.isEmpty() && !symbol.isEmpty())
    {
        _p->updateTunnel(accountAddress,symbol,tunnelAddress);

    }
}

void DepositAutomatic::ParseMutli(const QString &jsonString)
{
//    qDebug()<< "auto--"<<jsonString;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return ;
    QJsonObject jsonObject = parse_doucment.object();

    if(!jsonObject.value("result").isObject())
    {
        return;
    }
    QString symbol = jsonObject.value("result").toObject().value("chain_type").toString();
    QString multi = jsonObject.value("result").toObject().value("bind_account_hot").toString();
    if(!symbol.isEmpty() && !multi.isEmpty())
    {
        _p->updateMulti(symbol,multi);
    }
}

void DepositAutomatic::ParseTransaction(const QString &address,const QString &jsonString)
{
    qDebug()<< "parse--transaction--"<<jsonString;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return ;
    QJsonObject jsonObject = parse_doucment.object();


    QJsonObject obj =  jsonObject.value("result").toObject();

    //获取对应通道地址
    //QString tunnelAddress;
    //QJsonArray arr = obj.value("trx").toObject().value("vout").toArray();
    //foreach (QJsonValue val, arr) {
    //    if(!val.isObject()) continue;
    //    QJsonObject valObj = val.toObject();
    //    if(valObj.value("n").toInt() == 0)
    //    {
    //        qDebug()<<"dededetail--"<<valObj.value("scriptPubKey").toObject().value("addresses");
    //        tunnelAddress = valObj.value("scriptPubKey").toObject().value("addresses").toArray()[0].toString();
    //        break;
    //    }
    //}
    if(!address.isEmpty())
    {
        qDebug()<<"dededetail--"<<address<<obj;
        _p->updateDetail(address,obj);
    }

}




