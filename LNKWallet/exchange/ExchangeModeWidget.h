#ifndef EXCHANGEMODEWIDGET_H
#define EXCHANGEMODEWIDGET_H

#include <QWidget>

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

private:
    Ui::ExchangeModeWidget *ui;

    void init();
};

#endif // EXCHANGEMODEWIDGET_H
