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

class WithdrawConfirmPage : public QWidget
{
    Q_OBJECT

public:
    explicit WithdrawConfirmPage(QWidget *parent = 0);
    ~WithdrawConfirmPage();

    void init();

    void refresh();

    int currentType = 1;

    void showCrosschainTransactions();
    void refreshCrosschainTransactionsState();
    void showEthFinalTrxs();

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

    void on_ethFinalTrxTableWidget_cellClicked(int row, int column);

    void onAssetComboBoxCurrentIndexChanged(const QString &arg1);

    void on_ethFinalTrxTableWidget_cellPressed(int row, int column);

    void on_autoSignCheckBox_stateChanged(int arg1);

    void on_autoSignSetBtn_clicked();

private:
    Ui::WithdrawConfirmPage *ui;
    bool inited = false;
    HttpManager httpManager;
    void fetchCoinBalance(int id, QString chainId, QString address);
    void fetchEthBalance();


    void paintEvent(QPaintEvent*);
    PageScrollWidget *pageWidget;
    BlankDefaultWidget *blankWidget;
};

#endif // WITHDRAWCONFIRMPAGE_H
