#ifndef GUARDINCOMEPAGE_H
#define GUARDINCOMEPAGE_H

#include <QWidget>

namespace Ui {
class GuardIncomePage;
}

class GuardIncomePage : public QWidget
{
    Q_OBJECT

public:
    explicit GuardIncomePage(QWidget *parent = 0);
    ~GuardIncomePage();

private:
    Ui::GuardIncomePage *ui;
};

#endif // GUARDINCOMEPAGE_H
