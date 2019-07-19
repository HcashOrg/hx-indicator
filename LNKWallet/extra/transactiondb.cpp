#include "transactiondb.h"

#include "wallet.h"

#include <QBuffer>

TransactionDB::TransactionDB()
{

}

TransactionDB::~TransactionDB()
{
    if(m_transactionStructDB)  delete m_transactionStructDB;
    if(m_accountTransactionIdsDB)  delete m_accountTransactionIdsDB;
    if(m_guaranteeOrderDB)  delete m_guaranteeOrderDB;
    if(m_contractInvokeObjectDB)    delete m_contractInvokeObjectDB;
    if(m_nameTransferTrxDB) delete m_nameTransferTrxDB;
}

bool TransactionDB::init()
{
    leveldb::Options options;
    options.create_if_missing = true;
    QString dirStr = HXChain::getInstance()->walletConfigPath + "/transactionDB";
    QDir dir;
    if(!dir.exists(dirStr))
    {
        bool result = dir.mkdir(dirStr);
        qDebug() << "mkdir" << dirStr << result;
    }

    leveldb::Status status = leveldb::DB::Open(options,QString(HXChain::getInstance()->walletConfigPath + "/transactionDB/transactionStruct").toStdString(), &m_transactionStructDB);
    leveldb::Status status2 = leveldb::DB::Open(options,QString(HXChain::getInstance()->walletConfigPath + "/transactionDB/accountTransactionIds").toStdString(), &m_accountTransactionIdsDB);
    leveldb::Status status3 = leveldb::DB::Open(options,QString(HXChain::getInstance()->walletConfigPath + "/transactionDB/guaranteeOrder").toStdString(), &m_guaranteeOrderDB);
    leveldb::Status status4 = leveldb::DB::Open(options,QString(HXChain::getInstance()->walletConfigPath + "/transactionDB/contractInvokeObject").toStdString(), &m_contractInvokeObjectDB);
    leveldb::Status status5 = leveldb::DB::Open(options,QString(HXChain::getInstance()->walletConfigPath + "/transactionDB/nameTransferTrx").toStdString(), &m_nameTransferTrxDB);
    qDebug() << "transactionstruct db init" << status.ok() << QString::fromStdString( status.ToString())
                                            << status2.ok() << QString::fromStdString( status2.ToString())
                                            << status3.ok() << QString::fromStdString( status3.ToString())
                                            << status4.ok() << QString::fromStdString( status4.ToString())
                                            << status5.ok() << QString::fromStdString( status5.ToString());

    inited = true;
    if(status.ok() && status2.ok() && status3.ok())
    {
        return true;
    }
    else
    {
        return false;
    }
}



void TransactionDB::insertTransactionStruct(QString _transactionId, TransactionStruct& _struct)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    //序列化对象信息
    out << _struct;
    buffer.close();

    writeToDB(m_transactionStructDB, _transactionId, ba);
}

TransactionStruct TransactionDB::getTransactionStruct(QString _transactionId)
{
    QByteArray value = readFromDB(m_transactionStructDB, _transactionId);

    if(value.isEmpty())     return TransactionStruct();

    //读取文件流信息
    QBuffer buffer(&value);
    buffer.open(QIODevice::ReadOnly);

    QDataStream in(&buffer);
    //反序列化，获取对象信息
    TransactionStruct transactionStruct;
    in >> transactionStruct;
    buffer.close();

    return transactionStruct;
}

void TransactionDB::removeTransactionStruct(QString _transactionId)
{
    if(m_transactionStructDB)
    {
        removeFromDB(m_transactionStructDB, _transactionId);

        QStringList keys = getKeys(m_accountTransactionIdsDB);
        foreach (QString key, keys)
        {
            removeAccountTransactionId(key, _transactionId);
        }
    }
}

void TransactionDB::insertGuaranteeOrder(QString _guaranteeId, GuaranteeOrder _order)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    //序列化对象信息
    out << _order;
    buffer.close();

    writeToDB(m_guaranteeOrderDB, _guaranteeId, ba);
}

GuaranteeOrder TransactionDB::getGuaranteeOrder(QString _guaranteeId)
{
    if(_guaranteeId.isEmpty())      return GuaranteeOrder();
    QByteArray value = readFromDB(m_guaranteeOrderDB, _guaranteeId);

    if(value.isEmpty())     return GuaranteeOrder();

    //读取文件流信息
    QBuffer buffer(&value);
    buffer.open(QIODevice::ReadOnly);

    QDataStream in(&buffer);
    //反序列化，获取对象信息
    GuaranteeOrder guaranteeOrder;
    in >> guaranteeOrder;
    buffer.close();

    return guaranteeOrder;
}

void TransactionDB::insertContractInvokeObject(QString _trxId, ContractInvokeObject _object)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    //序列化对象信息
    out << _object;
    buffer.close();

    writeToDB(m_contractInvokeObjectDB, _trxId, ba);
}

ContractInvokeObject TransactionDB::getContractInvokeObject(QString _trxId)
{
    if(_trxId.isEmpty())      return ContractInvokeObject();
    QByteArray value = readFromDB(m_contractInvokeObjectDB, _trxId);

    if(value.isEmpty())     return ContractInvokeObject();

    //读取文件流信息
    QBuffer buffer(&value);
    buffer.open(QIODevice::ReadOnly);

    QDataStream in(&buffer);
    //反序列化，获取对象信息
    ContractInvokeObject object;
    in >> object;
    buffer.close();

    return object;
}

void TransactionDB::insertNameTransferTrx(QString _trxCode, QStringList _trxStrAndId)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
qDebug() << "bbbbbbbbbb " << _trxStrAndId;
    QDataStream out(&buffer);
    //序列化对象信息
    out << _trxStrAndId;
    buffer.close();

    writeToDB(m_nameTransferTrxDB, _trxCode, ba);
}

QStringList TransactionDB::getNameTransferTrx(QString _trxCode)
{
    if(_trxCode.isEmpty())      return QStringList();
    QByteArray value = readFromDB(m_nameTransferTrxDB, _trxCode);

    if(value.isEmpty())     return QStringList();

    //读取文件流信息
    QBuffer buffer(&value);
    buffer.open(QIODevice::ReadOnly);

    QDataStream in(&buffer);
    //反序列化，获取对象信息
    QStringList trxStr;
    in >> trxStr;
    buffer.close();

    return trxStr;
}

void TransactionDB::removeNameTransferTrx(QString _trxCode)
{
    if(m_nameTransferTrxDB)
    {
        removeFromDB(m_nameTransferTrxDB, _trxCode);
    }
}

QStringList TransactionDB::getNameTransferTrxsCode()
{
    return getKeys(m_nameTransferTrxDB);
}

QVector<TransactionStruct> TransactionDB::lookupTransactionStruct(QString _address, int _type)
{
    QVector<TransactionStruct> result;

    if(m_transactionStructDB)
    {
        QStringList keys = getKeys(m_transactionStructDB);
        foreach (QString key, keys)
        {
            TransactionStruct ts = getTransactionStruct(key);
            if(ts.type == _type)
            {
                switch (ts.type)
                {
                case 0:
                {
                    QJsonObject object = QJsonDocument::fromJson(ts.operationStr.toLatin1()).object();
                    QString fromAddress = object.take("from_addr").toString();
                    QString toAddress   = object.take("to_addr").toString();
                    if(fromAddress == _address || toAddress == _address)
                    {
                        result.append(ts);
                    }
                }
                    break;
                default:
                    break;
                }
            }
        }
    }

    return result;
}

QStringList TransactionDB::getPendingTransactions()
{
    QStringList result;

    if(m_transactionStructDB)
    {
        QStringList keys = getKeys(m_transactionStructDB);
        foreach (QString key, keys)
        {
            TransactionStruct ts = getTransactionStruct(key);
            if(ts.blockNum == 0)
            {
                result << ts.transactionId;
            }
        }

    }

    return result;
}

void TransactionDB::insertAccountTransactionTypeIds(QString _accountAddress, TransactionTypeIds _transactionTypeIds)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    //序列化对象信息
    out << _transactionTypeIds;
    buffer.close();

    writeToDB(m_accountTransactionIdsDB, _accountAddress, ba);
}

void TransactionDB::addAccountTransactionId(QString _accountAddress, TransactionTypeId _transactionTypeId)
{
    TransactionTypeIds typeIds = getAccountTransactionTypeIds(_accountAddress);

    if(!typeIds.contains(_transactionTypeId))
    {
        typeIds.append(_transactionTypeId);
    }

    insertAccountTransactionTypeIds(_accountAddress, typeIds);
}

void TransactionDB::removeAccountTransactionId(QString _accountAddress, QString _transactionId)
{
    TransactionTypeIds typeIds = getAccountTransactionTypeIds(_accountAddress);
    TransactionTypeId  typeId;
    typeId.transactionId = _transactionId;
    if(typeIds.contains(typeId))
    {
        typeIds.removeAll(typeId);
    }

    insertAccountTransactionTypeIds(_accountAddress, typeIds);
}

TransactionTypeIds TransactionDB::getAccountTransactionTypeIds(QString _accountAddress)
{
    QByteArray value = readFromDB(m_accountTransactionIdsDB, _accountAddress);

    if(value.isEmpty())     return TransactionTypeIds();

    //读取文件流信息
    QBuffer buffer(&value);
    buffer.open(QIODevice::ReadOnly);

    QDataStream in(&buffer);
    //反序列化，获取对象信息
    TransactionTypeIds transactionTypeIds;
    in >> transactionTypeIds;
    buffer.close();

    return transactionTypeIds;
}

TransactionTypeIds TransactionDB::getAccountTransactionTypeIdsByType(QString _accountAddress, int _type)
{
    TransactionTypeIds transactionTypeIds = getAccountTransactionTypeIds(_accountAddress);
    TransactionTypeIds result;

    foreach (TransactionTypeId transactionTypeId, transactionTypeIds)
    {
        if(transactionTypeId.type == _type || _type == -2 )
        {
            result.append(transactionTypeId);
        }
    }

    return result;
}

bool TransactionDB::writeToDB(leveldb::DB* _db, QString _key, QByteArray _value)
{
    if(!inited)         return false;
    if(_db == NULL)     return false;
    std::string stdKeyStr = _key.toStdString();
    leveldb::Slice key = stdKeyStr;
    std::string stdValueStr = _value.toStdString();
    leveldb::Slice value = stdValueStr;
    leveldb::WriteOptions write_options;
    write_options.sync = true;
    return _db->Put(write_options, key, value).ok();
}

QByteArray TransactionDB::readFromDB(leveldb::DB* _db, QString _key)
{ 
    if(!inited)         return QByteArray();
    if(_db == NULL)     return QByteArray();
    std::string strValue = "";
    std::string stdStr = _key.toStdString();
    leveldb::Slice key = stdStr;
    _db->Get(leveldb::ReadOptions(), key, &strValue);
    return QByteArray::fromStdString(strValue);
}

bool TransactionDB::removeFromDB(leveldb::DB *_db, QString _key)
{
    if(!inited)         return false;
    if(_db == NULL)     return false;
    std::string stdStr = _key.toStdString();
    leveldb::Slice key = stdStr;
    return _db->Delete(leveldb::WriteOptions(), key).ok();
}

QStringList TransactionDB::getKeys(leveldb::DB *_db)
{
    if(!inited)         return QStringList();
    if(_db == NULL)     return QStringList();
    leveldb::Iterator* it = _db->NewIterator(leveldb::ReadOptions());
    QStringList keys;
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
        keys.append(QString::fromStdString(it->key().ToString()));
    }
    delete it;

    return   keys;
}

bool TransactionDB::clearAllDBs()
{
    inited = false;
    if(m_transactionStructDB)  delete m_transactionStructDB;
    if(m_accountTransactionIdsDB)  delete m_accountTransactionIdsDB;
    if(m_guaranteeOrderDB)  delete m_guaranteeOrderDB;
    if(m_contractInvokeObjectDB)    delete m_contractInvokeObjectDB;

    leveldb::Options options;
    leveldb::DestroyDB((HXChain::getInstance()->walletConfigPath + "/transactionDB/transactionStruct").toStdString(), options);
    leveldb::DestroyDB((HXChain::getInstance()->walletConfigPath + "/transactionDB/accountTransactionIds").toStdString(), options);
    leveldb::DestroyDB((HXChain::getInstance()->walletConfigPath + "/transactionDB/guaranteeOrder").toStdString(), options);

    return init();
}

