#ifndef FEEDPRICEPAGE_H
#define FEEDPRICEPAGE_H

#include <QWidget>

#include "poundage/PageScrollWidget.h"
#include "control/BlankDefaultWidget.h"
namespace Ui {
class FeedPricePage;
}

class FeedPricePage : public QWidget
{
    Q_OBJECT

public:
    explicit FeedPricePage(QWidget *parent = 0);
    ~FeedPricePage();

    void init();

    void showAssetsPrice();

    void refresh();
private slots:
    void jsonDataUpdated(QString id);

    void on_assetPriceTableWidget_cellClicked(int row, int column);

    void pageChangeSlot(unsigned int page);
private:
    Ui::FeedPricePage *ui;

    void paintEvent(QPaintEvent*);
    PageScrollWidget *pageWidget;
    BlankDefaultWidget *blankWidget;
};

#endif // FEEDPRICEPAGE_H
