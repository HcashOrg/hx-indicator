#ifndef CHECKEXCHANGEPAIRWIDGET_H
#define CHECKEXCHANGEPAIRWIDGET_H

#include <QWidget>
#include "wallet.h"

namespace Ui {
class CheckExchangePairWidget;
}

class CheckExchangePairWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CheckExchangePairWidget(QWidget *parent = nullptr);
    ~CheckExchangePairWidget();

    void setPair(const ExchangePair& _pair);
    ExchangePair pair;

    void setChecked(bool isChecked);

signals:
    void stateChanged(ExchangePair,bool);

private slots:
    void on_checkBox_stateChanged(int arg1);

private:
    Ui::CheckExchangePairWidget *ui;
};

#endif // CHECKEXCHANGEPAIRWIDGET_H
