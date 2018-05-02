#ifndef ONCHAINORDERPAGE_H
#define ONCHAINORDERPAGE_H

#include <QWidget>

namespace Ui {
class OnchainOrderPage;
}

class OnchainOrderPage : public QWidget
{
    Q_OBJECT

public:
    explicit OnchainOrderPage(QWidget *parent = 0);
    ~OnchainOrderPage();

private:
    Ui::OnchainOrderPage *ui;
};

#endif // ONCHAINORDERPAGE_H
