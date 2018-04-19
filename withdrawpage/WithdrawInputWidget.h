#ifndef WITHDRAWINPUTWIDGET_H
#define WITHDRAWINPUTWIDGET_H

#include <QWidget>

namespace Ui {
class WithdrawInputWidget;
}

class WithdrawInputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WithdrawInputWidget(QWidget *parent = 0);
    ~WithdrawInputWidget();
signals:
    void withdrawSignal(const QString &address,double ammount);
public:
    void setMaxAmmount(double number);
    void setSymbol(const QString &symbol);
private slots:
    void addressChangeSlots(const QString &address);

    void maxButtonSlots();

    void confirmButtonSlots();
private:
    bool validateAddress(const QString &address);
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::WithdrawInputWidget *ui;
};

#endif // WITHDRAWINPUTWIDGET_H
