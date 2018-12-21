#include "crosscapitalmark.h"

#include <atomic>
#include <memory>
#include <map>
#include <mutex>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <thread>
#include <chrono>
#include <QTimer>
#include "wallet.h"
#include "extra/HttpManager.h"

struct CrossMarkBaseData
{
    QString accountName;
    std::map<QString,std::map<QString,double>> transaction;//symbol,trxid,number
};

class CrossCapitalMark::DataPrivate
{
public:
    DataPrivate()
    {
        if( HXChain::getInstance()->configFile->contains("/settings/chainPath"))
        {
            crossFilePath = HXChain::getInstance()->configFile->value("/settings/chainPath").toString() + "/crossmark.dat";
        }
        else
        {
            crossFilePath = HXChain::getInstance()->appDataPath + "/crossmark.dat";
        }
    }
public:
    QString crossFilePath;

    std::vector<std::shared_ptr<CrossMarkBaseData>> crossData;

    std::mutex dataMutex;

    std::mutex httpMutex;

    HttpManager httpManager;//中间件查询交易

    std::map<QString,int> currentBlockHeight;//symbol + blockheight主要针对erc eth => ETH
    HttpManager ethHeightHttp;

    std::map<int,QString> queryIdTrxidMap;
};

CrossCapitalMark::CrossCapitalMark(QObject *parent)
    :_p(new DataPrivate())
{
    ReadFromDoc();

    connect( HXChain::getInstance(), &HXChain::jsonDataUpdated, this, &CrossCapitalMark::jsonDataUpdated);

    connect(&_p->httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));
    connect(&_p->ethHeightHttp,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(ethHttpReplied(QByteArray,int)));
}

CrossCapitalMark::~CrossCapitalMark()
{
    WriteToDoc();
    delete _p;
}

bool CrossCapitalMark::ReadFromDoc()
{
    std::lock_guard<std::mutex> mu(_p->dataMutex);
    _p->crossData.clear();

    //解析json文档

    QFile contractFile(_p->crossFilePath);
    if(!contractFile.open(QIODevice::ReadOnly)) return false;

    QString jsonStr(contractFile.readAll());
    contractFile.close();

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(jsonStr);

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);

    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return false;

    QJsonObject jsonObject = parse_doucment.object();
    //开始分析
    foreach (QString key, jsonObject.keys()){
        std::shared_ptr<CrossMarkBaseData> data = std::make_shared<CrossMarkBaseData>();
        data->accountName = key;
        _p->crossData.push_back(data);
        QJsonValue val = jsonObject.value(key);
        if(!val.isArray()) continue;
        QJsonArray jsonArr = val.toArray();
        foreach (QJsonValue arrVal, jsonArr) {
            if(!arrVal.isObject()) continue;
            //解析每组交易
            QJsonObject minObj = arrVal.toObject();
            foreach (QString minkey, minObj.keys()) {//币种
                QJsonValue minVal = minObj.value(minkey);
                if(!minVal.isObject()) continue;
                QJsonObject tras = minVal.toObject();
                foreach (QString id, tras.keys()) {
                    data->transaction[minkey].insert(std::make_pair(id,tras.value(id).toDouble()));
                }
            }
        }
    }
    return true;
}

bool CrossCapitalMark::WriteToDoc()
{
    std::lock_guard<std::mutex> mu(_p->dataMutex);
    //构建QJsonDocument
    QJsonObject mainObject;
    for(auto it = _p->crossData.begin();it != _p->crossData.end();++it)
    {

        QJsonArray transArr;
        int i = 0;
        for(auto tra = (*it)->transaction.begin();tra != (*it)->transaction.end();++tra)
        {
            QJsonObject mi;
            QJsonObject transaction;
            for(auto pa = (*tra).second.begin();pa != (*tra).second.end();++pa)
            {
                transaction.insert((*pa).first,(*pa).second);
            }
            mi.insert((*tra).first,transaction);
            transArr.insert(i++,mi);
        }
        mainObject.insert((*it)->accountName,transArr);
    }

    QJsonDocument document;
    document.setObject(mainObject);

    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    //写文件
    QFile file(_p->crossFilePath);
    if(!file.open(QIODevice::WriteOnly)) return false;

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream<<byte_array;
    stream.setGenerateByteOrderMark(true);
    file.close();
    return true;

}

void CrossCapitalMark::InsertTransaction(const QString &accountName, const QString &chainType,
                                         const QString &tranID, double number)
{
    std::lock_guard<std::mutex> mu(_p->dataMutex);
    for(auto it = _p->crossData.begin();it != _p->crossData.end();++it)
    {
        if((*it)->accountName == accountName && !tranID.isEmpty())
        {
            (*it)->transaction[chainType].insert(std::make_pair(tranID,number));
            return;
        }

    }
    std::shared_ptr<CrossMarkBaseData> data = std::make_shared<CrossMarkBaseData>();
    data->accountName = accountName;
    data->transaction[chainType].insert(std::make_pair(tranID,number));
    _p->crossData.push_back(data);
}

void CrossCapitalMark::RemoveTransaction(const QString &tranID)
{
    std::lock_guard<std::mutex> mu(_p->dataMutex);
    bool isFind = false;
    for(auto it = _p->crossData.begin();it != _p->crossData.end();++it)
    {
        if(isFind) break;
        for(auto tra = (*it)->transaction.begin();tra != (*it)->transaction.end();++tra)
        {
            if(isFind) break;
            for(auto pa = (*tra).second.begin();pa != (*tra).second.end();++pa)
            {
                if(tranID == (*pa).first)
                {
                    isFind = true;
                    (*tra).second.erase(pa);
                    break;
                }
            }
        }
    }
    if(!isFind) return;
    //去除空内容
    for(auto it = _p->crossData.begin();it != _p->crossData.end();)
    {
        if((*it)->transaction.empty())
        {
            it = _p->crossData.erase(it);
        }
        else
        {
            for(auto tra = (*it)->transaction.begin();tra != (*it)->transaction.end();)
            {
                if((*tra).second.empty())
                {
                    tra = (*it)->transaction.erase(tra);
                }
                else
                {
                    ++tra;
                }
            }


            ++it;
        }
    }
}

void CrossCapitalMark::jsonDataUpdated(const QString &id)
{
    if(id.startsWith("mark_decoderawtransaction+"))
    {//解析出交易id
        QString accountName ;
        QString symbol ;
        double number = 0;
        ParsePostID(id,accountName,symbol,number);
        QString result = HXChain::getInstance()->jsonDataValue( id);
        result.prepend("{");
        result.append("}");
        QString trid = ParseTransactionID(result);
        qDebug()<<trid;
        //加入队列，
        InsertTransaction(accountName,symbol,trid,number);
        //向中间件请求交易详情
        QueryTransaction(symbol,trid);

        //需要更新信息
         QTimer::singleShot(200,[this,symbol](){this->QueryTransaction(symbol,"finish");});
    }
}

void CrossCapitalMark::httpReplied(QByteArray _data, int _status)
{
    std::lock_guard<std::mutex> mu(_p->httpMutex);

    QJsonObject object  = QJsonDocument::fromJson(_data).object().value("result").toObject();
    int queryID = QJsonDocument::fromJson(_data).object().value("id").toInt();

    QString hash = "";
    int confirm = 0;

    if(object.value("chainId").toString()=="hc")
    {
        hash = object.value("data").toObject().value("txid").toString();
        confirm = object.value("data").toObject().value("vin").toArray().at(0).toObject().value("blockheight").toInt();
        //_p->currentBlockHeight["HC"] -  object.value("data").toObject().value("vin").toArray().at(0).toObject().value("blockheight").toInt()>=0?100:0;
    }
    else if(object.value("chainId").toString().contains("erc")||object.value("chainId").toString().contains("eth"))
    {
        hash = object.value("data").toObject().value("source_trx").toObject().value("hash").toString();
        confirm = _p->currentBlockHeight["ETH"] -  object.value("data").toObject().value("source_trx").toObject().value("blockNumber").toString().toInt(nullptr,16)>=0?100:0;
    }
    else
    {
        hash = object.value("data").toObject().value("hash").toString();
        confirm = object.value("data").toObject().value("confirmations").toInt();
    }

    if(!_p->queryIdTrxidMap[queryID].isEmpty() && hash.isEmpty())
    {
        hash = _p->queryIdTrxidMap[queryID];
        _p->queryIdTrxidMap.erase(_p->queryIdTrxidMap.find(queryID));
    }
    qDebug()<<"kkkkkkkkkkkk"<<hash<<confirm;
    qDebug()<<"jjjj"<<object;
    if(confirm >= 8)
    {
        RemoveTransaction(hash);
    }

    if(!object.value("error_message").toString().isEmpty() || hash.isEmpty())
    {
        if(!hash.isEmpty())
        {
            RemoveTransaction(hash);
        }
        emit updateMark();
    }
    qDebug()<<hash<<object;
}

void CrossCapitalMark::ethHttpReplied(QByteArray _data, int _status)
{
    int id = QJsonDocument::fromJson(_data).object().value("id").toInt();
    if(!QJsonDocument::fromJson(_data).object().value("result").toObject().value("error_message").toString().isEmpty())
    {
        return;
    }
    int height  = QJsonDocument::fromJson(_data).object().value("result").toInt();
    if(5748 == id)
    {
        _p->currentBlockHeight["ETH"] = height;
    }
    else if(6748 == id)
    {
        _p->currentBlockHeight["HC"] = height;
    }
}

QString CrossCapitalMark::ParseTransactionID(const QString &jsonString)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return "";
    QJsonObject jsonObject = parse_doucment.object();
    qDebug()<<jsonObject;
    QString trxid = jsonObject.value("result").toObject().value("hash").toString();
    return trxid.isEmpty()?jsonObject.value("result").toObject().value("trxid").toString():trxid;
}

void CrossCapitalMark::QueryTransaction(const QString &symbol, const QString &id)
{
    static std::atomic_int queryId = 0;
    int queid = queryId.fetch_add(1);
    if(id != "finish" && !id.isEmpty())
    {
        _p->queryIdTrxidMap[queid] = id;
    }
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",queid);
    object.insert("method","Zchain.Trans.queryTrans");
    QJsonObject paramObject;
    paramObject.insert("chainId",symbol);
    paramObject.insert("trxid",id);
    object.insert("params",paramObject);
    qDebug()<<"query transid"<<id;
    qDebug()<<"bbbbb"<<object;
    _p->httpManager.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());
}

void CrossCapitalMark::QueryETHorHCHeight(const QString &symbol)
{
    int id = 0;
    if(symbol.contains("erc",Qt::CaseInsensitive)||symbol.contains("eth",Qt::CaseInsensitive))
    {
        id = 5748;
    }
    else if(symbol.contains("hc",Qt::CaseInsensitive))
    {
        id = 6748;
    }
    else
    {
        id =45;
    }
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",id);
    object.insert("method","Zchain.Query.getBlockHeight");
    QJsonObject paramObject;
    paramObject.insert("chainId",symbol);
    object.insert("params",paramObject);
    _p->ethHeightHttp.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());

}

void CrossCapitalMark::ParsePostID(const QString &postID, QString &name, QString &symbol, double &number)
{
    QStringList listStr = postID.split("+");
    name = listStr.at(1);
    symbol = listStr.at(2);
    number = listStr.at(3).toDouble();
}

double CrossCapitalMark::CalTransaction(const QString &accountName, const QString &chainType) const
{
    double number = 0;

    for(auto it = _p->crossData.begin();it != _p->crossData.end();++it)
    {
        if(accountName != (*it)->accountName) continue;

        for(auto tra = (*it)->transaction.begin();tra != (*it)->transaction.end();++tra)
        {
            if(chainType != (*tra).first) continue;
            for(auto pa = (*tra).second.begin();pa != (*tra).second.end();++pa)
            {
                number += (*pa).second;
            }
        }
    }
    return number;
}

void CrossCapitalMark::checkUpData(const QString &accountName, const QString &chainType)
{
    if(chainType.contains("erc",Qt::CaseInsensitive) || chainType.contains("eth",Qt::CaseInsensitive) || chainType.contains("hc",Qt::CaseInsensitive))
    {
        QueryETHorHCHeight(chainType);
    }
    for(auto it = _p->crossData.begin();it != _p->crossData.end();++it)
    {
        if(accountName != (*it)->accountName) continue;

        for(auto tra = (*it)->transaction.begin();tra != (*it)->transaction.end();++tra)
        {
            if(chainType != (*tra).first) continue;
            for(auto pa = (*tra).second.begin();pa != (*tra).second.end();++pa)
            {
                QueryTransaction(chainType,(*pa).first);
            }
        }
    }
    QTimer::singleShot(2000,[this,chainType](){this->QueryTransaction(chainType,"finish");});

}

void CrossCapitalMark::TransactionInput(const QString &jsonString,const QString &symbol,const QString &accountName,double number)
{
    //解析该交易
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonString.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return;
    QJsonObject jsonObject = parse_doucment.object();
    QString transHex = jsonObject.value("result").toString();
    //解锁交易id
    HXChain::getInstance()->postRPC( "mark_decoderawtransaction+"+accountName+"+"+symbol+"+"+QString::number(number),
                                     toJsonFormat( "decoderawtransaction", QJsonArray()
                                     << transHex<<symbol ));
}
