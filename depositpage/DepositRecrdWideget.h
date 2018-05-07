#ifndef DEPOSITRECRDWIDEGET_H
#define DEPOSITRECRDWIDEGET_H

#include <QWidget>

namespace Ui {
class DepositRecrdWideget;
}

class PageScrollWidget;
class DepositRecrdWideget : public QWidget
{
    Q_OBJECT

public:
    explicit DepositRecrdWideget(QWidget *parent = 0);
    ~DepositRecrdWideget();

    void init();

    void showDepositRecord(QString _tunnelAddress);

private slots:
    void pageChangeSlot(unsigned int page);
private:
    Ui::DepositRecrdWideget *ui;

    QString tunnelAddress;

    void paintEvent(QPaintEvent*);
    PageScrollWidget *pageWidget;
};

#endif // DEPOSITRECRDWIDEGET_H
