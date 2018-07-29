#ifndef BUYORDERWIDGET_H
#define BUYORDERWIDGET_H

#include <QWidget>

namespace Ui {
class BuyOrderWidget;
}
class FeeChooseWidget;
class BuyOrderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BuyOrderWidget(QWidget *parent = 0);
    ~BuyOrderWidget();

    void init();

    void setAccount(QString _accountName);
    void setOrderInfo(unsigned long long _buyAmount, QString _buySymbol, unsigned long long _sellAmount, QString _sellSymbol);
    void setContractAddress(QString _contractAddress);

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_allBtn_clicked();

    void on_amountLineEdit_textChanged(const QString &arg1);

    void on_amountLineEdit_textEdited(const QString &arg1);

private:
    Ui::BuyOrderWidget *ui;
    QString accountName;
    QString buySymbol;
    QString sellSymbol;
    unsigned long long buyAmount;
    unsigned long long sellAmount;
    int stepCount = 0;      // 合约执行步数

    void paintEvent(QPaintEvent*);
    void estimateContractFee();

    FeeChooseWidget *feeChoose;
};

#endif // BUYORDERWIDGET_H
