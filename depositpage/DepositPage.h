#ifndef DEPOSITPAGE_H
#define DEPOSITPAGE_H

#include <QWidget>

namespace Ui {
class DepositPage;
}

class DepositPage : public QWidget
{
    Q_OBJECT
public:
    struct DepositDataInput{
        DepositDataInput(QString name,QString address,QString type,QString id="")
            :accountName(name),accountAddress(address),assetType(type),assetID(id){}
        QString accountName;
        QString accountAddress;
        QString assetType;//BTC LTC...
        QString assetID;//1.3.1  1.3.2 ...
    };
public:
    explicit DepositPage(DepositDataInput data,QWidget *parent = 0);
    ~DepositPage();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::DepositPage *ui;
};

#endif // DEPOSITPAGE_H
