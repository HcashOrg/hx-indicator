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

private:
    void InitWidget();
    void InitStyle();

signals:
    void showOnchainOrderSignal();
    void showMyOrderSignal();

private slots:
    void on_onchainOrderBtn_clicked();

    void on_myOrderBtn_clicked();

private:
    Ui::FunctionExchangeWidget *ui;
};

#endif // FUNCTIONEXCHANGEWIDGET_H
