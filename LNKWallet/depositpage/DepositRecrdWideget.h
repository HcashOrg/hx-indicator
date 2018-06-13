#ifndef DEPOSITRECRDWIDEGET_H
#define DEPOSITRECRDWIDEGET_H

#include <QWidget>

namespace Ui {
class DepositRecrdWideget;
}

class PageScrollWidget;
class BlankDefaultWidget;
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
    void on_depositRecordTableWidget_cellPressed(int row, int column);

private:
    Ui::DepositRecrdWideget *ui;

    QString tunnelAddress;

    void paintEvent(QPaintEvent*);
    PageScrollWidget *pageWidget;
    BlankDefaultWidget *blankWidget;
};

#endif // DEPOSITRECRDWIDEGET_H
