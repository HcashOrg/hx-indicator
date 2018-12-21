#ifndef CROSSCAPITALMARK_H
#define CROSSCAPITALMARK_H     

#include <QString>
#include <QObject>
class CrossCapitalMark:public QObject
{
    Q_OBJECT
public:
    explicit CrossCapitalMark(QObject *parent = 0);
    ~CrossCapitalMark();
signals:
    void updateMark();
private:
    bool ReadFromDoc();
    bool WriteToDoc();
public:
    double CalTransaction(const QString &accountName,const QString &chainType)const;

    void checkUpData(const QString &accountName,const QString &chainType);

    void TransactionInput(const QString &jsonString,const QString &symbol,const QString &accountName,double number);
private:

    void InsertTransaction(const QString &accountName,const QString &chainType,const QString &tranID,double number);
    void RemoveTransaction(const QString &tranID);
private slots:
    void jsonDataUpdated(const QString &id);
    void httpReplied(QByteArray _data, int _status);
    void ethHttpReplied(QByteArray _data, int _status);

private:
    QString ParseTransactionID(const QString &jsonStr);
    void QueryTransaction(const QString &symbol,const QString &id);
    void QueryETHorHCHeight(const QString &symbol);

    void ParsePostID(const QString &postID,QString &name,QString &symbol,double &number);
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // CROSSCAPITALMARK_H
