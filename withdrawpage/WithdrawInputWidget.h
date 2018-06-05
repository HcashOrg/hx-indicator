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
    void withdrawSignal(const QString &address,const QString & ammount);
public:
    void InitData(const QString &number,const QString &symol);
protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void addressChangeSlots(const QString &address);

    void numberChangeSlots(const QString &number);

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
