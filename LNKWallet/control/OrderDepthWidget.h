#ifndef ORDERDEPTHWIDGET_H
#define ORDERDEPTHWIDGET_H

#include <QWidget>

namespace Ui {
class OrderDepthWidget;
}

class OrderDepthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OrderDepthWidget(QWidget *parent = nullptr);
    ~OrderDepthWidget();

public:
    void setType(bool _sellOrNot = true);
    bool sellOrNot = true;

    void setNumber(int _number);
    int number = 0;

    void setPrice(QString _price);
    QString price;

    void setAmount(QString _amount);
    QString amount;

    void setLength(double _length);
    double length = 0;

    void setBaseAmount(unsigned long long _baseAmount);
    unsigned long long baseAmount = 0;


private:
    Ui::OrderDepthWidget *ui;

    void paintEvent(QPaintEvent* event);
};

#endif // ORDERDEPTHWIDGET_H
