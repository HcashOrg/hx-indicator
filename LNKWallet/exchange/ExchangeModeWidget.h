#ifndef EXCHANGEMODEWIDGET_H
#define EXCHANGEMODEWIDGET_H

#include <QWidget>
#include "ExchangePairWidget.h"


namespace Ui {
class ExchangeModeWidget;
}

class ExchangeModeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExchangeModeWidget(QWidget *parent = nullptr);
    ~ExchangeModeWidget();

private slots:
    void on_orderModeBtn_clicked();

    void on_favoriteMarketBtn_clicked();

    void on_marketBtn1_clicked();

private:
    Ui::ExchangeModeWidget *ui;
    ExchangePairWidget* exchangePairWidget = nullptr;

    void init();
    void paintEvent(QPaintEvent*);

};

#endif // EXCHANGEMODEWIDGET_H
