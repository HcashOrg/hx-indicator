#ifndef WITHDRAWCONFIRMPAGE_H
#define WITHDRAWCONFIRMPAGE_H

#include <QWidget>
#include <QMap>

namespace Ui {
class WithdrawConfirmPage;
}

struct ApplyTransaction
{
    QString trxId;
    QString expirationTime;
    QString withdrawAddress;
    QString amount;
    QString assetSymbol;
    QString assetId;
    QString crosschainAddress;
    QString memo;
};
struct GeneratedTransaction
{
    QString trxId;
    QStringList ccwTrxIds;      // 交易里包含的apply交易
};

struct SignTransaction
{
    QString trxId;
    QString generatedTrxId;
    QString guardAddress;
};

class WithdrawConfirmPage : public QWidget
{
    Q_OBJECT

public:
    explicit WithdrawConfirmPage(QWidget *parent = 0);
    ~WithdrawConfirmPage();

    void init();

    void refresh();

    void fetchCrosschainTransactions();

    int currentType = 1;
    QMap<QString,ApplyTransaction> applyTransactionMap;
    QMap<QString,ApplyTransaction> pendingApplyTransactionMap;
    QMap<QString,GeneratedTransaction> generatedTransactionMap;
    QMap<QString,SignTransaction>   signTransactionMap;
    QString lookupGeneratedTrxByApplyTrxId(QString applyTrxId);
    QStringList lookupSignedGuardsByGeneratedTrxId(QString generatedTrxId);

private slots:
    void jsonDataUpdated(QString id);

    void on_crosschainTransactionTableWidget_cellClicked(int row, int column);

    void on_crosschainTransactionTableWidget_cellPressed(int row, int column);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_typeCurrentBtn_clicked();

    void on_typeWaitingBtn_clicked();

private:
    Ui::WithdrawConfirmPage *ui;

    void showCrosschainTransactions();
    void refreshCrosschainTransactionsState();

    void paintEvent(QPaintEvent*);
};

#endif // WITHDRAWCONFIRMPAGE_H
