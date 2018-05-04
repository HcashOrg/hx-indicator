#ifndef ONCHAINORDERPAGE_H
#define ONCHAINORDERPAGE_H

#include <QWidget>

#include "extra/HttpManager.h"

namespace Ui {
class OnchainOrderPage;
}

class OnchainOrderPage : public QWidget
{
    Q_OBJECT

public:
    explicit OnchainOrderPage(QWidget *parent = 0);
    ~OnchainOrderPage();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void httpReplied(QByteArray _data, int _status);

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_assetComboBox2_currentIndexChanged(const QString &arg1);

    void on_ordersTableWidget_cellPressed(int row, int column);

private:
    Ui::OnchainOrderPage *ui;
    HttpManager httpManager;

    void queryContractOrders();

    void paintEvent(QPaintEvent*);
};

#endif // ONCHAINORDERPAGE_H
