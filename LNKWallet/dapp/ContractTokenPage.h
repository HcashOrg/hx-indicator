#ifndef CONTRACTTOKENPAGE_H
#define CONTRACTTOKENPAGE_H

#include <QWidget>

namespace Ui {
class ContractTokenPage;
}

class ContractTokenPage : public QWidget
{
    Q_OBJECT

public:
    explicit ContractTokenPage(QWidget *parent = 0);
    ~ContractTokenPage();

private:
    Ui::ContractTokenPage *ui;
};

#endif // CONTRACTTOKENPAGE_H
