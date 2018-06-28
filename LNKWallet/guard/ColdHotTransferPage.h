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

struct ColdHotSignTransaction
{
    QString trxId;
    QString coldHotCrosschainTrxId;
    QString guardAddress;
};

class ColdHotTransferPage : public QWidget
{
    Q_OBJECT

public:
    explicit ColdHotTransferPage(QWidget *parent = 0);
    ~ColdHotTransferPage();

    void init();

    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_transferBtn_clicked();

    void on_coldHotTransactionTableWidget_cellClicked(int row, int column);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_coldHotTransactionTableWidget_cellPressed(int row, int column);

private:
    Ui::ColdHotTransferPage *ui;
    bool inited = false;

    QMap<QString,ColdHotTransaction> coldHotTransactionMap;
    QMap<QString,ColdHotCrosschainTransaction> coldHotCrosschainTransactionMap;
    QMap<QString,ColdHotSignTransaction> coldHotSignTransactionMap;
    void fetchColdHotTransaction();

    void showColdHotTransactions();
    void refreshColdHotTtransactionsState();
    QString lookupCrosschainTrxByColdHotTrxId(QString coldHotTrxId);
    QStringList lookupSignedGuardByCrosschainTrx(QString crosschainTrxId);

    void paintEvent(QPaintEvent*);
};

#endif // COLDHOTTRANSFERPAGE_H
