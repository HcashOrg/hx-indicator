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
private slots:
    void ConfirmSlots();

    void radioChangedSlots();

    void jsonDataUpdated(QString id);

    void passwordConfirmSlots();
    void passwordCancelSlots();

    void numberChangeSlots(const QString &number);

    void addressChangeSlots(const QString &address);
private:
    void updateData();
    bool validateAddress(const QString &address);
protected:
    void paintEvent(QPaintEvent *event);
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
