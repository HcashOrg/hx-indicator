#ifndef DEPOSITRECRDWIDEGET_H
#define DEPOSITRECRDWIDEGET_H

#include <QWidget>

namespace Ui {
class DepositRecrdWideget;
}

class DepositRecrdWideget : public QWidget
{
    Q_OBJECT

public:
    explicit DepositRecrdWideget(QWidget *parent = 0);
    ~DepositRecrdWideget();

private:
    Ui::DepositRecrdWideget *ui;
};

#endif // DEPOSITRECRDWIDEGET_H
