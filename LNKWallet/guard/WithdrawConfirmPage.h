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

class WithdrawConfirmPage : public QWidget
{
    Q_OBJECT

public:
    explicit WithdrawConfirmPage(QWidget *parent = 0);
    ~WithdrawConfirmPage();

    void init();

    void fetchCrosschainTransactions(int type = 1);


    QMap<QString,ApplyTransaction> applyTransactionMap;
    QMap<QString,GeneratedTransaction> generatedTransactionMap;

private slots:
    void jsonDataUpdated(QString id);

    void on_crosschainTransactionTableWidget_cellClicked(int row, int column);

private:
    Ui::WithdrawConfirmPage *ui;

    void paintEvent(QPaintEvent*);
};

#endif // WITHDRAWCONFIRMPAGE_H
