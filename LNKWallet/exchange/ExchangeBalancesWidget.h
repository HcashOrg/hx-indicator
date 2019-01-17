#ifndef EXCHANGEBALANCESWIDGET_H
#define EXCHANGEBALANCESWIDGET_H

#include <QWidget>


namespace Ui {
class ExchangeBalancesWidget;
}

class ExchangeBalancesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExchangeBalancesWidget(QWidget *parent = nullptr);
    ~ExchangeBalancesWidget();

    void init();

private slots:
    void on_balancesTableWidget_cellClicked(int row, int column);

    void on_hideZeroCheckBox_stateChanged(int arg1);

private:
    void showBalances();
private:
    Ui::ExchangeBalancesWidget *ui;

    void paintEvent(QPaintEvent*);

};

#endif // EXCHANGEBALANCESWIDGET_H
