#ifndef FEEDPAGE_H
#define FEEDPAGE_H

#include <QWidget>
#include <QMap>

namespace Ui {
class FeedPage;
}


typedef  QMap<QString,unsigned long long>  AddressFeedPriceMap;

class FeedPage : public QWidget
{
    Q_OBJECT

public:
    explicit FeedPage(QWidget *parent = 0);
    ~FeedPage();

    void showTokenPrice();

public slots:
    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_tokenPriceTableWidget_cellPressed(int row, int column);

    void on_feedersPricesTableWidget_cellPressed(int row, int column);

private:
    Ui::FeedPage *ui;

    void paintEvent(QPaintEvent* e);

    void getAllTokenPrices();
    void showFeedPrices(QString token);
    QMap<QString,AddressFeedPriceMap>   tokenFeedPricesMap;

    QString currentContractName;
};

#endif // FEEDPAGE_H
