#ifndef CONTRACTBALANCEWIDGET_H
#define CONTRACTBALANCEWIDGET_H

#include <QWidget>

namespace Ui {
class ContractBalanceWidget;
}

class ContractBalanceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContractBalanceWidget(QWidget *parent = 0);
    ~ContractBalanceWidget();

    void init();

    void setAccount(QString _accountName);

private slots:
    void jsonDataUpdated(QString id);

    void on_balancesTableWidget_cellPressed(int row, int column);

    void on_depositBtn_clicked();

private:
    Ui::ContractBalanceWidget *ui;
    QString accountName;

    void paintEvent(QPaintEvent*);
    void showContractBalances();
};

#endif // CONTRACTBALANCEWIDGET_H
