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
    QStringList pubKeys;
    int required = 0;
};

class MultiSigPage : public QWidget
{
    Q_OBJECT

public:
    explicit MultiSigPage(QWidget *parent = 0);
    ~MultiSigPage();

private:
    void fetchMultisigInfo();
    QMap<QString,MultiSigInfo>  multiSigInfoMap;

    QMap<QString,AssetAmountMap>    multiSigBalancesMap;
    void fetchBalances();
    void showBalances();

signals:
    void backBtnVisible(bool isShow);

private slots:
    void jsonDataUpdated(QString id);

    void on_createMultiSigBtn_clicked();

//    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_multisigTableWidget_cellClicked(int row, int column);

private:
    Ui::MultiSigPage *ui;

    void init();
    void paintEvent(QPaintEvent*);
};

#endif // MULTISIGPAGE_H
