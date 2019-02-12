#ifndef TransactionDB_H
#define TransactionDB_H

#include <QObject>

#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <leveldb/slice.h>

class TransactionStruct;
class TransactionTypeId;
typedef QVector<TransactionTypeId>  TransactionTypeIds;
class GuaranteeOrder;
class ContractInvokeObject;

class TransactionDB
{
public:
    TransactionDB();
    ~TransactionDB();

    bool inited = false;
    bool init();

    void insertTransactionStruct(QString _transactionId, TransactionStruct &_struct);
    TransactionStruct getTransactionStruct(QString _transactionId);
    QVector<TransactionStruct>  lookupTransactionStruct(QString _address, int _type = 0);   // 查找地址(不一定是本钱包账户)相关的交易(本钱包内存储了的)
    QStringList getPendingTransactions();

    void insertAccountTransactionTypeIds(QString _accountAddress, TransactionTypeIds _transactionTypeIds);
    void addAccountTransactionId(QString _accountAddress, TransactionTypeId _transactionTypeId);
    void removeAccountTransactionId(QString _accountAddress, QString _transactionId);
    TransactionTypeIds getAccountTransactionTypeIds(QString _accountAddress);
    TransactionTypeIds getAccountTransactionTypeIdsByType(QString _accountAddress, int _type = -2); // 不传入type则返回所有类型的交易

    void removeTransactionStruct(QString _transactionId);

    void insertGuaranteeOrder(QString _guaranteeId, GuaranteeOrder _order);
    GuaranteeOrder getGuaranteeOrder(QString _guaranteeId);

    void insertContractInvokeObject(QString _trxId, ContractInvokeObject _object);
    ContractInvokeObject getContractInvokeObject(QString _trxId);

    bool writeToDB(leveldb::DB* _db, QString _key, QByteArray _value);
    QByteArray readFromDB(leveldb::DB* _db, QString _key);
    bool removeFromDB(leveldb::DB* _db, QString _key);

    QStringList getKeys(leveldb::DB* _db);

    bool clearAllDBs();

private:
    leveldb::DB* m_transactionStructDB = NULL;
    leveldb::DB* m_accountTransactionIdsDB = NULL;
    leveldb::DB* m_guaranteeOrderDB = NULL;
    leveldb::DB* m_contractInvokeObjectDB = NULL;
};

#endif // TransactionDB_H
