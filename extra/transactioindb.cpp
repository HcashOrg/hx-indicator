#include "transactioindb.h"
#include "lnk.h"


TransactioinDB::TransactioinDB()
{
    db = NULL;
}

TransactioinDB::~TransactioinDB()
{
    if(db)  delete db;
}

bool TransactioinDB::init()
{
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options,QString(UBChain::getInstance()->walletConfigPath + "/transactionDB").toStdString(), &db);
    if(status.ok())
    {
        return true;
    }
    else
    {
        return false;
    }
}

QStringList TransactioinDB::keys()
{
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    QStringList keys;
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
        keys.append(QString::fromStdString(it->key().ToString()));
    }
    delete it;

    return   keys;
}

void TransactioinDB::insertContractTransactions(QString _key, AccountContractTransactions _transactions)
{
    QByteArray tmp_array;
    QBuffer buffer(&tmp_array);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    //序列化对象信息
    out << _transactions;
    buffer.close();

    writeToDB(_key, tmp_array);
}

AccountContractTransactions TransactioinDB::getContractTransactions(QString _key)
{
    QByteArray value = readFromDB(_key);
    //读取文件流信息
    QBuffer buffer(&value);
    buffer.open(QIODevice::ReadOnly);

    QDataStream in(&buffer);
    //反序列化，获取对象信息
    AccountContractTransactions transaction;
    in >> transaction;
    buffer.close();

    return transaction;
}

bool TransactioinDB::writeToDB(QString _key, QByteArray _value)
{
    std::string stdKeyStr = _key.toStdString();
    leveldb::Slice key = stdKeyStr;
    std::string stdValueStr = _value.toStdString();
    leveldb::Slice value = stdValueStr;
    leveldb::WriteOptions write_options;
    write_options.sync = true;
    return db->Put(write_options, key, value).ok();
}

QByteArray TransactioinDB::readFromDB(QString _key)
{
    std::string strValue = "";
    std::string stdStr = _key.toStdString();
    leveldb::Slice key = stdStr;
    db->Get(leveldb::ReadOptions(), key, &strValue);
    return QByteArray::fromStdString(strValue);
}

