#ifndef PRICEDEPTHWIDGET_H
#define PRICEDEPTHWIDGET_H

#include <QWidget>

namespace Ui {
class PriceDepthWidget;
}

class PriceDepthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PriceDepthWidget(QWidget *parent = nullptr);
    ~PriceDepthWidget();

    double length = 0;
    void setLength(double _length);
    int type = 0;   // 0: sell  1: buy
    void setType(int _type);


private:
    Ui::PriceDepthWidget *ui;

    void paintEvent(QPaintEvent* event);

};

#endif // PRICEDEPTHWIDGET_H
