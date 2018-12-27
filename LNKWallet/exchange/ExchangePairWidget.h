#ifndef EXCHANGEPAIRWIDGET_H
#define EXCHANGEPAIRWIDGET_H

#include <QWidget>

namespace Ui {
class ExchangePairWidget;
}

class ExchangePairWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExchangePairWidget(QWidget *parent = nullptr);
    ~ExchangePairWidget();

private:
    Ui::ExchangePairWidget *ui;
};

#endif // EXCHANGEPAIRWIDGET_H
