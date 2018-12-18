#ifndef LOCKCONTRACTPAGE_H
#define LOCKCONTRACTPAGE_H

#include <QWidget>

namespace Ui {
class LockContractPage;
}

class LockContractPage : public QWidget
{
    Q_OBJECT

public:
    explicit LockContractPage(QWidget *parent = nullptr);
    ~LockContractPage();

    void init();

private:
    Ui::LockContractPage *ui;

    void paintEvent(QPaintEvent*);
};

#endif // LOCKCONTRACTPAGE_H
