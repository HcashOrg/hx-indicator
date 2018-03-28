#ifndef MULTISIGTRANSACTIONPAGE_H
#define MULTISIGTRANSACTIONPAGE_H

#include <QWidget>
#include "lnk.h"

namespace Ui {
class MultiSigTransactionPage;
}

class MultiSigTransactionPage : public QWidget
{
    Q_OBJECT

public:
    explicit MultiSigTransactionPage(QString _multiSigAddress, QWidget *parent = 0);
    ~MultiSigTransactionPage();

    void updateTransactionsList();

    void updateBalance();

    QString multiSigAddress;


public slots:
    void refresh();

private slots:

    void jsonDataUpdated(QString id);

    void on_copyBtn_clicked();

    void on_qrcodeBtn_clicked();

    void on_prePageBtn_clicked();

    void on_nextPageBtn_clicked();

    void on_firstPageBtn_clicked();

    void on_lastPageBtn_clicked();

    void on_pageLineEdit_editingFinished();

    void on_pageLineEdit_textEdited(const QString &arg1);

    void on_multiSigAddressComboBox_currentIndexChanged(const QString &arg1);

    void on_backBtn_clicked();

    void on_transactionsTableWidget_cellPressed(int row, int column);

signals:
    void back();

private:
    Ui::MultiSigTransactionPage *ui;

    bool inited;
    int currentPageIndex;

    void paintEvent(QPaintEvent*);
    void showTransactions();

    struct TransactionDetail
    {
        QString opposite;
        int type = 0;   // 0:默认/自己转自己  1:转出 2:转入
        AssetAmount assetAmount;
        QString memo;
        bool includeFee = false; // 金额里是否包含手续费  是的话需扣去
    };
    TransactionDetail getDetail(TransactionInfo info);    // 从transactioninfo中分析交易类型

};

#endif // MULTISIGTRANSACTIONPAGE_H
