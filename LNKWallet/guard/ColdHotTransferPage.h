#ifndef COLDHOTTRANSFERPAGE_H
#define COLDHOTTRANSFERPAGE_H

#include <QWidget>
#include <QMap>

namespace Ui {
class ColdHotTransferPage;
}

struct ColdHotTransaction
{
    QString trxId;
    QString expirationTime;
    QString withdrawAddress;
    QString depositAddress;
    QString amount;
    QString assetSymbol;
    QString assetId;
    QString guard;
    QString guardId;
    QString memo;
};

struct ColdHotCrosschainTransaction
{
    QString trxId;
    QString coldHotTrxId;
};

class ColdHotTransferPage : public QWidget
{
    Q_OBJECT

public:
    explicit ColdHotTransferPage(QWidget *parent = 0);
    ~ColdHotTransferPage();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void on_transferBtn_clicked();

    void on_coldHotTransactionTableWidget_cellClicked(int row, int column);

private:
    Ui::ColdHotTransferPage *ui;

    QMap<QString,ColdHotTransaction> coldHotTransactionMap;
    QMap<QString,ColdHotCrosschainTransaction> coldHotCrosschainTransactionMap;
    void fetchColdHotTransaction(int type = 1);

    void paintEvent(QPaintEvent*);
};

#endif // COLDHOTTRANSFERPAGE_H
