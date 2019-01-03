#ifndef ExchangeSinglePairCellWidget_H
#define ExchangeSinglePairCellWidget_H

#include <QWidget>
#include "wallet.h"


namespace Ui {
class ExchangeSinglePairCellWidget;
}

class ExchangeSinglePairCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExchangeSinglePairCellWidget(QWidget *parent = nullptr);
    ~ExchangeSinglePairCellWidget();

    void setPair(const ExchangePair& _pair);
    ExchangePair pair;
private:
    Ui::ExchangeSinglePairCellWidget *ui;

    void paintEvent(QPaintEvent *event);

};

#endif // ExchangeSinglePairCellWidget_H
