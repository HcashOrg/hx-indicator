#ifndef DEPOSITPAGE_H
#define DEPOSITPAGE_H

#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>充值界面 </summary>
///
///<remarks> 2018.04.16 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
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
    explicit DepositPage(const DepositDataInput &data,QWidget *parent = 0);
    ~DepositPage();
signals:
    void backBtnVisible(bool);
private slots:
    void jsonDataUpdated(QString id);

    void on_depositRecordBtn_clicked();

private:
    void GetBindTunnelAccount();
    void GetActualBindTunnelAccount();
    void GenerateAddress();
    void BindTunnelAccount();
    void BindActualTunnelAccount();
    void checkTunnelPriKey();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::DepositPage *ui;
private:
    class DepositPagePrivate;
    DepositPagePrivate *_p;
};

#endif // DEPOSITPAGE_H
