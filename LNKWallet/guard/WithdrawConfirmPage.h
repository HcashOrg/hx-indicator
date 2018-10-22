#ifndef WITHDRAWCONFIRMPAGE_H
#define WITHDRAWCONFIRMPAGE_H

#include <QWidget>
#include <QMap>
#include "poundage/PageScrollWidget.h"
#include "control/BlankDefaultWidget.h"
#include "extra/HttpManager.h"

namespace Ui {
class WithdrawConfirmPage;
}

class ETHFinalTrx;

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
    QMap<QString,ETHFinalTrx> ethFinalTrxMap;

private slots:
    void jsonDataUpdated(QString id);

    void httpReplied(QByteArray _data, int _status);

    void on_crosschainTransactionTableWidget_cellClicked(int row, int column);

    void on_crosschainTransactionTableWidget_cellPressed(int row, int column);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_typeCurrentBtn_clicked();

    void on_typeWaitingBtn_clicked();
    
    void on_typeETHBtn_clicked();
    
    void pageChangeSlot(unsigned int page);

    void on_autoConfirmBtn_clicked();


    void on_ethFinalTrxTableWidget_cellClicked(int row, int column);

    void onAssetComboBoxCurrentIndexChanged(const QString &arg1);

    void on_ethFinalTrxTableWidget_cellPressed(int row, int column);

private:
    Ui::WithdrawConfirmPage *ui;
    HttpManager httpManager;
    void fetchCoinBalance(int id, QString chainId, QString address);

    void showCrosschainTransactions();
    void refreshCrosschainTransactionsState();

    void fetchEthBalance();
    void showEthFinalTrxs();

    void paintEvent(QPaintEvent*);
    PageScrollWidget *pageWidget;
    BlankDefaultWidget *blankWidget;
};

#endif // WITHDRAWCONFIRMPAGE_H
