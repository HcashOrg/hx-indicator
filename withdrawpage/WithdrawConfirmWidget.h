#ifndef WITHDRAWCONFIRMWIDGET_H
#define WITHDRAWCONFIRMWIDGET_H

#include <QWidget>

namespace Ui {
class WithdrawConfirmWidget;
}

class WithdrawConfirmWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WithdrawConfirmWidget(QWidget *parent = 0);
    ~WithdrawConfirmWidget();

private:
    Ui::WithdrawConfirmWidget *ui;
};

#endif // WITHDRAWCONFIRMWIDGET_H
