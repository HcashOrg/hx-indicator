#ifndef ONCHAINORDERPAGE_H
#define ONCHAINORDERPAGE_H

#include <QWidget>
#include <QJsonArray>

#include "extra/HttpManager.h"
#include "FavoritePairsWidget.h"

namespace Ui {
class OnchainOrderPage;
}
class PageScrollWidget;
class BlankDefaultWidget;
class ExchangeModePage;
class OnchainOrderPage : public QWidget
{
    Q_OBJECT

public:
    explicit OnchainOrderPage(QWidget *parent = 0);
    ~OnchainOrderPage();

    void init();

signals:
    void backBtnVisible(bool isShow);
    void showExchangeModePage();
    void showMyOrdersPage();

private slots:
    void jsonDataUpdated(QString id);

    void httpReplied(QByteArray _data, int _status);

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_assetComboBox2_currentIndexChanged(const QString &arg1);

    void onItemClicked(int _row, int _column);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void pageChangeSlot(unsigned int page);
    void on_swapBtn_clicked();

    void on_exchangeModeBtn_clicked();

    void on_myOrdersBtn_clicked();

public slots:
    void on_favoriteBtn_clicked();
    void showPair(QString pairStr);
private:
    void checkFavorite();
    FavoritePairsWidget* favoritePairsWidget = NULL;

private:
    Ui::OnchainOrderPage *ui;
    bool inited = false;
    HttpManager httpManager;
    QWidget* currentWidget;

    void paintEvent(QPaintEvent*);
    void queryContractOrders();
    void updateTableHeaders();

    BlankDefaultWidget *blankWidget = NULL;
    PageScrollWidget *pageWidget = NULL;


    QJsonArray sortArray(const QJsonArray &data,bool greater);
};

#endif // ONCHAINORDERPAGE_H
