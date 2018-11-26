#include "CaptialNotify.h"

#include <mutex>
#include <QTimer>
#include "extra/HttpManager.h"
#include "wallet.h"

static const QString HX_ASSET_ID = "1.3.0";
class CaptialNotify::DataPrivate
{
public:
    struct accountInfo
    {
        accountInfo()
            :assetNumber("0")

        {

        }

        QString accountAddress;
        QString accountName;
        QString tunnelAddress;
        QString assetSymbol;
        QString assetNumber;
    };
public:
    DataPrivate()
        :isNotifyOn(false)
        ,isInUpdate(false)
    {

    }
public:
    bool isNotifyOn;//是否开启通知，顶层控制，关闭后不进行检查
    bool isInUpdate;//是否处于更新阶段，防止重复更新
    std::mutex mutexLock;//数据修改锁
    std::mutex updateMutexLock;//更新标志锁
    std::list<std::shared_ptr<accountInfo>> accounts;//所有账户信息
    QTimer *timer;
    HttpManager httpManager;////用于查询通道账户余额

public:
    void clearAccount()
    {
        std::lock_guard<std::mutex> lockguard(mutexLock);
        accounts.clear();
    }
    //清理空tunnel账户
    void clearEmptyTunnel()
    {
        std::lock_guard<std::mutex> lockguard(mutexLock);
        accounts.erase(std::remove_if(accounts.begin(),accounts.end(),[](std::shared_ptr<accountInfo> info){
                                       return info->tunnelAddress.isEmpty();})
                        ,accounts.end());
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
                (*it)->assetNumber = QString::number(std::max<double>(0,number.toDouble()-HXChain::getInstance()->feeChargeInfo.capitalFee.toDouble())
                                                     ,'f',pre);
                if((*it)->assetNumber.toDouble() < 1e-8)
                {
                    accounts.erase(it);
                }
                break;
            }
        }
    }
    //查询某用户对应资产通道账户的金额
    QString getAssetTunnelMoney(const QString &accountName,const QString &assetName)const
    {
        auto it = std::find_if(accounts.begin(),accounts.end(),[accountName,assetName](std::shared_ptr<accountInfo> info){
            return (accountName == info->accountName) && (assetName == info->assetSymbol);
        });
        return it != accounts.end() ? (*it)->assetNumber : "0";
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

CaptialNotify::CaptialNotify(QObject *parent)
    : QObject(parent)
    , _p(new DataPrivate())
{
    _p->timer = new QTimer(this);
    _p->timer->start(10000);

    connect(_p->timer,&QTimer::timeout,this,&CaptialNotify::updateData);

    connect( HXChain::getInstance(), &HXChain::jsonDataUpdated, this, &CaptialNotify::jsonDataUpdated);
    connect(&_p->httpManager,&HttpManager::httpReplied,this,&CaptialNotify::httpReplied);
}

CaptialNotify::~CaptialNotify()
{
    if(_p)
    {
        delete _p;
        _p = nullptr;
    }
}

bool CaptialNotify::isAccountTunnelMoneyEmpty(const QString &accountName, const QString &assetSymbol) const
{
    return _p->getAssetTunnelMoney(accountName,assetSymbol).toDouble()<1e-8;
}

void CaptialNotify::startCheckTunnelMoney()
{
    _p->isNotifyOn = true;
    updateData();
}

void CaptialNotify::stopCheckTunnelMoney()
{
    _p->timer->stop();
}

void CaptialNotify::jsonDataUpdated(const QString &id)
{
    if(id.startsWith("CaptialNotify-get_binding_account-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        result.prepend("{");
        result.append("}");
    //提取通道账户地址
        ParseTunnel(result);

    }else if("CaptialNotify-finish-tunnel" == id)
    {
        //先排除没有tunnel的账户
        _p->clearEmptyTunnel();
        //开始查找余额
        for(auto it = _p->accounts.begin();it != _p->accounts.end();++it)
        {
            PostQueryTunnelMoney((*it)->assetSymbol,(*it)->tunnelAddress);
        }
        PostQueryTunnelMoney("captialnotifyfinish","captialnotifyfinish");//结束签名,可以进行刷新
    }else if("CaptialNotify-finish-money" == id)
    {
        QTimer::singleShot(500,[this](){
            this->_p->SetInUpdateFalse();
            emit checkTunnelMoneyFinish();

        });
    }
}

void CaptialNotify::httpReplied(QByteArray _data, int _status)
{//解析查询余额的返回，补全账户信息
    qDebug() << "auto--http-- " << _data << _status;
    if(QString(_data).contains("captialnotifyfinish"))
    {
        //结束查找余额--必须等余额查完了再创建交易--余额的查询很慢,而且容易错位到达
        FinishQueryMoney();
        return;
    }

    QJsonObject object  = QJsonDocument::fromJson(_data).object().value("result").toObject();
    QString tunnel   = object.value("address").toString();
    QString number = QString::number( jsonValueToDouble(object.value("balance")));
    _p->updateMoney(tunnel,number);
}

void CaptialNotify::updateData()
{
    if(_p->isInUpdate || !_p->isNotifyOn) return;

    //清空数据
    _p->SetInUpdateTrue();
    _p->clearAccount();

    QMap<QString,AccountInfo> accountInfoMap = HXChain::getInstance()->accountInfoMap;
    QMap<QString,AssetInfo> assetInfoMap = HXChain::getInstance()->assetInfoMap;

    foreach (AccountInfo info, accountInfoMap) {
        foreach(AssetInfo assetInfo,assetInfoMap){
            if(assetInfo.id != HX_ASSET_ID )
            {
                std::shared_ptr<DataPrivate::accountInfo> da = std::make_shared<DataPrivate::accountInfo>();
                da->accountAddress = info.address;
                da->accountName = info.name;
                da->assetSymbol = assetInfo.symbol;
                _p->accounts.push_back(da);
            }
        }
    }

    if(_p->accounts.empty())
    {
       _p->SetInUpdateFalse();
        return;
    }

    //发送寻找对应通道地址
    for(const auto &in : _p->accounts)
    {
        PostQueryTunnelAddress(in->accountName,in->assetSymbol);
    }
    FinishQueryTunnel();
}

void CaptialNotify::PostQueryTunnelAddress(const QString &accountName, const QString &symbol)
{
    if(accountName.isEmpty() || symbol.isEmpty()) return;
    HXChain::getInstance()->postRPC( "CaptialNotify-get_binding_account-"+accountName,
                                     toJsonFormat( "get_binding_account", QJsonArray()
                                                   << accountName<<symbol ));
}

void CaptialNotify::FinishQueryTunnel()
{
    HXChain::getInstance()->postRPC( "CaptialNotify-finish-tunnel",
                                     toJsonFormat( "CaptialNotify-finish-tunnel", QJsonArray()));

}

void CaptialNotify::ParseTunnel(const QString &jsonString)
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

void CaptialNotify::PostQueryTunnelMoney(const QString &symbol, const QString &tunnelAddress)
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

void CaptialNotify::FinishQueryMoney()
{
    HXChain::getInstance()->postRPC( "CaptialNotify-finish-money",
                                     toJsonFormat( "CaptialNotify-finish-money", QJsonArray()));
}
