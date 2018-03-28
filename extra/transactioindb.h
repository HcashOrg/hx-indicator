#ifndef TRANSACTIOINDB_H
#define TRANSACTIOINDB_H

#include <QObject>

#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <leveldb/slice.h>

class AccountContractTransactions;
class TransactioinDB
{
public:
    TransactioinDB();
    ~TransactioinDB();

    bool init();

    QStringList keys();

    void insertContractTransactions(QString _key, AccountContractTransactions _transactions);
    AccountContractTransactions getContractTransactions(QString _key);



    bool writeToDB(QString _key, QByteArray _value);
    QByteArray readFromDB(QString _key);

private:
    leveldb::DB *db;

};

#endif // TRANSACTIOINDB_H
