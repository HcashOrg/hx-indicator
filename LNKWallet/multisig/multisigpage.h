#ifndef MULTISIGPAGE_H
#define MULTISIGPAGE_H

#include <QWidget>
#include "wallet.h"

namespace Ui {
class MultiSigPage;
}

struct MultiSigInfo
{
    QString owner;
    QStringList addresses;
    int required = 0;
};

class MultiSigPage : public QWidget
{
    Q_OBJECT

public:
    explicit MultiSigPage(QWidget *parent = 0);
    ~MultiSigPage();

public:
    QMap<QString,MultiSigInfo>  multiSigInfoMap;
    void fetchMultisigInfo();
    void showMultiSigInfo();

    QMap<QString,AssetAmountMap>    multiSigBalancesMap;
    void fetchBalances();
    void showBalances();

signals:
    void backBtnVisible(bool isShow);

private slots:
    void jsonDataUpdated(QString id);

    void on_createMultiSigBtn_clicked();

//    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_balanceTableWidget_cellClicked(int row, int column);

    void on_typeBalanceBtn_clicked();

    void on_typeInfoBtn_clicked();

    void on_typeSignBtn_clicked();

    void on_multiSigAddressComboBox_currentIndexChanged(const QString &arg1);

    void on_copyBtn_clicked();

    void on_trxCodeTextBrowser_textChanged();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_signBtn_clicked();

    void on_sendSignatureBtn_clicked();

    void on_copyBtn2_clicked();

    void on_addMultiSigBtn_clicked();

    void on_deleteBtn_clicked();

    void on_infoTableWidget_cellClicked(int row, int column);

private:
    Ui::MultiSigPage *ui;
    int signedNum = 0;
    int requiredNum = 0;

    void init();
    void paintEvent(QPaintEvent*);
};

#endif // MULTISIGPAGE_H
