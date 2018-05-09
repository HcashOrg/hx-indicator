#ifndef BUYORDERWIDGET_H
#define BUYORDERWIDGET_H

#include <QWidget>

namespace Ui {
class BuyOrderWidget;
}

class BuyOrderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BuyOrderWidget(QWidget *parent = 0);
    ~BuyOrderWidget();

    void init();

    void setAccount(QString _accountName);
    void setPrice(QString _price, QString _assetSymbol, QString _assetSymbol2);
    void setContractAddress(QString _contractAddress);

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_allBtn_clicked();

private:
    Ui::BuyOrderWidget *ui;
    QString accountName;
    QString payAsset;

    void paintEvent(QPaintEvent*);
};

#endif // BUYORDERWIDGET_H
