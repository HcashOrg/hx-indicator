#ifndef ACCOUNTPAGE_H
#define ACCOUNTPAGE_H

#include <QWidget>
#include "wallet.h"

namespace Ui {
class AccountPage;
}

#define MODULE_ACCOUNT_PAGE "ACCOUNT_PAGE"


class AccountPage : public QWidget
{
    Q_OBJECT

public:
    explicit AccountPage(QString name,QWidget *parent = 0);
    ~AccountPage();

    void updateTransactionsList();

    void updateContractFee();

public slots:
    void refresh();

private slots:

    void on_copyBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void jsonDataUpdated(QString id);

//    void on_accountTransactionsTableWidget_cellClicked(int row, int column);

    void on_prePageBtn_clicked();

    void on_nextPageBtn_clicked();

    void on_firstPageBtn_clicked();

    void on_lastPageBtn_clicked();

    void on_pageLineEdit_editingFinished();

    void on_pageLineEdit_textEdited(const QString &arg1);

    void on_withdrawBtn_clicked();

    void on_accountTransactionsTableWidget_cellPressed(int row, int column);

    void on_qrcodeBtn_clicked();

signals:
    void back();
    void accountChanged(QString);
    void showShadowWidget();
    void hideShadowWidget();
    void showApplyDelegatePage(QString);

private:
    Ui::AccountPage *ui;
    QString accountName;
    QString address;
    int transactionType;
    bool inited;
    int currentPageIndex;
    QString registeredLabelString;
    bool assetUpdating;

    void paintEvent(QPaintEvent*);
    void init();
    void showTransactions();
    void showNormalTransactions();


    struct TransactionDetail
    {
        QString opposite;
        int type = 0;   // 0:默认/自己转自己  1:转出 2:转入    3:领取合约手续费  4:代理领取工资   7:调用合约  8:合约资产销毁/清退   9:合约升级
        AssetAmount assetAmount;
        QString memo;
        bool includeFee = false; // 金额里是否包含手续费  是的话需扣去
    };
    TransactionDetail getDetail(TransactionInfo info);    // 从transactioninfo中分析交易类型

};

#endif // ACCOUNTPAGE_H
