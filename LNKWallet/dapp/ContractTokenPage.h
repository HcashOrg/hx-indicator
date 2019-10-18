#ifndef CONTRACTTOKENPAGE_H
#define CONTRACTTOKENPAGE_H

#include <QWidget>
#include <QMap>

namespace Ui {
class ContractTokenPage;
}

struct ContractTokenInfo
{
    QString contractId;
    QString name;
    QString symbol;
    QString precision;
    QString supply;
    int registeredBlock = 0;
    QString ownerAddress;
};

struct TokenBalance
{
    QString amount;
    QString contractId;
};

class ContractTokenPage : public QWidget
{
    Q_OBJECT

public:
    explicit ContractTokenPage(QWidget *parent = 0);
    ~ContractTokenPage();

    void init();
    void refresh();
    void setAccount(QString accountName);

    QMap<QString,ContractTokenInfo> contractTokenInfoMap;
    QMap<QString,QMap<QString,TokenBalance>>    accountContractTokenBalanceMap;


signals:
    void backBtnVisible(bool isShow);
    void accountContractTokenBalanceUpdated();
    void gotoTransferPage(QString,QString,QString,QString);     // fromAccount, toAddress, amount, memo

private slots:
    void on_createTokenBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_addTokenBtn_clicked();

    void on_tokenTableWidget_cellClicked(int row, int column);

    void on_historyBtn_clicked();

private:
    Ui::ContractTokenPage *ui;
    bool inited = false;
    void paintEvent(QPaintEvent*);

    void showAccountTokens();

private slots:
    void fetchTokensInfo();
    void fetchTokensBalance();
    void on_applyBtn_clicked();
};

#endif // CONTRACTTOKENPAGE_H
