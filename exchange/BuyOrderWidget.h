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
    void setSellAsset(QString _assetSymbol);
    void setBuyAsset(QString _assetSymbol);

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_sellComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::BuyOrderWidget *ui;
    QString accountName;

    void paintEvent(QPaintEvent*);
};

#endif // BUYORDERWIDGET_H
