#ifndef CAPITALTRANSFERPAGE_H
#define CAPITALTRANSFERPAGE_H

#include <QWidget>

namespace Ui {
class CapitalTransferPage;
}

class CapitalTransferPage : public QWidget
{
    Q_OBJECT
public:
    struct CapitalTransferInput{
        CapitalTransferInput(const QString &accountname,const QString &_account_address,const QString &_symbol)
            :account_name(accountname),account_address(_account_address),symbol(_symbol)
        {

        }
        QString account_name;
        QString account_address;
        QString symbol;
    };

public:
    explicit CapitalTransferPage(const CapitalTransferInput &data,QWidget *parent = 0);
    ~CapitalTransferPage();
signals:
    void backBtnVisible(bool isShow);
private slots:
    void ConfirmSlots();

    void radioChangedSlots();

    void jsonDataUpdated(QString id);
    void httpReplied(QByteArray _data, int _status);

    void passwordConfirmSlots();
    void passwordCancelSlots();

    void numberChangeSlots(const QString &number);

    void addressChangeSlots(const QString &address);

    void onSliderValueChanged(int value);
private:
    bool validateAddress(const QString &address);

    void PostQueryTunnelMoney(const QString &symbol,const QString &tunnelAddress);

    void CreateTransaction();

    void getMarkNumber();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::CapitalTransferPage *ui;
private:
    class CapitalTransferPagePrivate;
    CapitalTransferPagePrivate *_p;
};

#endif // CAPITALTRANSFERPAGE_H
