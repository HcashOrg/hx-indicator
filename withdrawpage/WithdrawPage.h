#ifndef WITHDRAWPAGE_H
#define WITHDRAWPAGE_H

#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>提现界面 </summary>
///
///<remarks> 2018.04.16 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class WithdrawPage;
}

class WithdrawPage : public QWidget
{
    Q_OBJECT
public:
    struct WithdrawDataInput{
        WithdrawDataInput(QString name,QString address,QString type,QString ammount,QString id="")
            :accountName(name),accountAddress(address),assetSymbol(type),assetAmmount(ammount),assetID(id){}
        QString accountName;
        QString accountAddress;
        QString assetSymbol;//BTC LTC...
        QString assetAmmount;
        QString assetID;//1.3.1  1.3.2 ...
    };
public:
    explicit WithdrawPage(const WithdrawDataInput &data,QWidget *parent = 0);
    ~WithdrawPage();
signals:
    void backBtnVisible(bool);
private slots:
    void ShowRecordSlots();

    void ShowConfirmWidget(const QString &address,const QString & ammount);
    void on_withdrawRecordBtn_clicked();

private:
    void InitWidget();
    void InitStyle();
private:
    Ui::WithdrawPage *ui;
private:
    class WithdrawPagePrivate;
    WithdrawPagePrivate *_p;
};

#endif // WITHDRAWPAGE_H
