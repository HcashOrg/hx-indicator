#include "transactiondb.h"

#include "wallet.h"

#include <QBuffer>

TransactionDB::TransactionDB()
{
    m_transactionStructDB = NULL;
    m_accountTransactionIdsDB = NULL;
}

TransactionDB::~TransactionDB()
{
    if(m_transactionStructDB)  delete m_transactionStructDB;
    if(m_accountTransactionIdsDB)  delete m_accountTransactionIdsDB;
}

bool TransactionDB::init()
{
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options,QString(UBChain::getInstance()->walletConfigPath + "/transactionDB/transactionStruct").toStdString(), &m_transactionStructDB);
    leveldb::Status status2 = leveldb::DB::Open(options,QString(UBChain::getInstance()->walletConfigPath + "/transactionDB/accountTransactionIds").toStdString(), &m_accountTransactionIdsDB);

    if(status.ok() && status2.ok())
    {
        return true;
    }
    else
    {
        return false;
    }
}



void TransactionDB::insertTransactionStruct(QString _transactionId, TransactionStruct _struct)
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
    qDebug() << "111111111111  " << typeIds.size() ;

    if(!typeIds.contains(_transactionTypeId))
    {
        typeIds.append(_transactionTypeId);
    }
    qDebug() << "222222222222  " << typeIds.size();

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

    foreach (TransactionTypeId id, transactionTypeIds) {
        qDebug() << "00000000000  " << id.type << id.transactionId;
    }

    return transactionTypeIds;
}

TransactionTypeIds TransactionDB::getAccountTransactionTypeIdsByType(QString _accountAddress, int _type)
{
    TransactionTypeIds transactionTypeIds = getAccountTransactionTypeIds(_accountAddress);
    TransactionTypeIds result;
    foreach (TransactionTypeId transactionTypeId, transactionTypeIds)
    {
        if(transactionTypeId.type == _type)
        {
            result.append(transactionTypeId);
        }
    }

    return result;
}

bool TransactionDB::writeToDB(leveldb::DB* _db, QString _key, QByteArray _value)
{
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
    std::string strValue = "";
    std::string stdStr = _key.toStdString();
    leveldb::Slice key = stdStr;
    _db->Get(leveldb::ReadOptions(), key, &strValue);
    return QByteArray::fromStdString(strValue);
}

