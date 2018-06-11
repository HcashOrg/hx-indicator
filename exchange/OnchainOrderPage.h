#ifndef ONCHAINORDERPAGE_H
#define ONCHAINORDERPAGE_H

#include <QWidget>

#include "extra/HttpManager.h"

namespace Ui {
class OnchainOrderPage;
}
class PageScrollWidget;
class BlankDefaultWidget;
class OnchainOrderPage : public QWidget
{
    Q_OBJECT

public:
    explicit OnchainOrderPage(QWidget *parent = 0);
    ~OnchainOrderPage();

    void init();

    void onBack();

signals:
    void backBtnVisible(bool isShow);

private slots:
    void jsonDataUpdated(QString id);

    void httpReplied(QByteArray _data, int _status);

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_assetComboBox2_currentIndexChanged(const QString &arg1);

    void onItemClicked(int _row, int _column);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void pageChangeSlot(unsigned int page);
private:
    Ui::OnchainOrderPage *ui;
    bool inited = false;
    HttpManager httpManager;
    QWidget* currentWidget;

    void paintEvent(QPaintEvent*);
    void queryContractOrders();
    void updateTableHeaders();

    BlankDefaultWidget *blankWidget;
    PageScrollWidget *pageWidget;
};

#endif // ONCHAINORDERPAGE_H
