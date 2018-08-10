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

private slots:
    void on_createTokenBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_addTokenBtn_clicked();

    void on_tokenTableWidget_cellClicked(int row, int column);

private:
    Ui::ContractTokenPage *ui;
    void paintEvent(QPaintEvent*);

    QMap<QString,ContractTokenInfo> contractTokenInfoMap;
    QMap<QString,QMap<QString,TokenBalance>>    accountContractTokenBalanceMap;
    void fetchTokensInfo();
    void fetchTokensBalance();

    void showAccountTokens();
};

#endif // CONTRACTTOKENPAGE_H
