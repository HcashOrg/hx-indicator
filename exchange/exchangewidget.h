#ifndef EXCHANGEWIDGET_H
#define EXCHANGEWIDGET_H

#include <QWidget>

namespace Ui {
class ExchangeWidget;
}

class ExchangeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExchangeWidget(QWidget *parent = 0);
    ~ExchangeWidget();

private slots:
    void on_backBtn_clicked();

private:
    Ui::ExchangeWidget *ui;

    void paintEvent(QPaintEvent*);
};

#endif // EXCHANGEWIDGET_H
