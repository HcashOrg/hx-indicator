#ifndef FUNCTIONEXCHANGEWIDGET_H
#define FUNCTIONEXCHANGEWIDGET_H

#include <QWidget>

namespace Ui {
class FunctionExchangeWidget;
}

class FunctionExchangeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionExchangeWidget(QWidget *parent = 0);
    ~FunctionExchangeWidget();
    void retranslator();
public slots:
    void DefaultShow();
private:
    void InitWidget();
    void InitStyle();

signals:
    void showExchangeModeSignal();
    void showOnchainOrderSignal();
    void showMyOrderSignal();
    void showContractTokenSignal();

private slots:
    void on_exchangeBtn_clicked();

    void on_onchainOrderBtn_clicked();

    void on_contractTokenBtn_clicked();


private:
    Ui::FunctionExchangeWidget *ui;

    void paintEvent(QPaintEvent*);
};

#endif // FUNCTIONEXCHANGEWIDGET_H
