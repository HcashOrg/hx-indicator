#ifndef FEEDPRICEPAGE_H
#define FEEDPRICEPAGE_H

#include <QWidget>

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

private:
    Ui::FeedPricePage *ui;

    void paintEvent(QPaintEvent*);
};

#endif // FEEDPRICEPAGE_H
