#ifndef WITHDRAWCONFIRMWIDGET_H
#define WITHDRAWCONFIRMWIDGET_H

#include <QWidget>

namespace Ui {
class WithdrawConfirmWidget;
}

class WithdrawConfirmWidget : public QWidget
{
    Q_OBJECT
public:
    struct WithdrawConfirmInput{
        WithdrawConfirmInput(const QString &account_name,const QString &withdraw_ammount,const QString &asset_symbol,
                             const QString target_account)
            :account(account_name),ammount(withdraw_ammount),symbol(asset_symbol),crosschain_account(target_account)
        {
        }

        QString account;
        QString ammount;
        QString symbol;
        QString crosschain_account;
    };

public:
    explicit WithdrawConfirmWidget(const WithdrawConfirmInput &data,QWidget *parent = 0);
    ~WithdrawConfirmWidget();
signals:
    void closeSelf();
private slots:
    void ConfirmSlots();
    void CancelSlots();

    void passwordChangeSlots(const QString &address);
    void jsonDataUpdated(QString id);
private:
    void InitData();
private:
    void InitWidget();
    void InitStyle();
protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::WithdrawConfirmWidget *ui;
private:
    class WithdrawConfirmWidgetPrivate;
    WithdrawConfirmWidgetPrivate *_p;
};

#endif // WITHDRAWCONFIRMWIDGET_H
