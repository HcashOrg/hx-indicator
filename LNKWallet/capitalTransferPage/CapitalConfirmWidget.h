#ifndef CAPITALCONFIRMWIDGET_H
#define CAPITALCONFIRMWIDGET_H

#include <QWidget>

namespace Ui {
class CapitalConfirmWidget;
}

class CapitalConfirmWidget : public QWidget
{
    Q_OBJECT
public:
    struct CapitalConfirmInput{
        CapitalConfirmInput(const QString &account_address,const QString &actual_show,const QString &fee_show,
                             const QString total_show)
            :address(account_address),actual(actual_show),fee(fee_show),total(total_show)
        {
        }

        QString address;
        QString fee;
        QString actual;
        QString total;
    };
public:
    explicit CapitalConfirmWidget(const CapitalConfirmInput &input,QWidget *parent = 0);
    ~CapitalConfirmWidget();
signals:
    void ConfirmSignal();
    void CancelSignal();
protected:
    void paintEvent(QPaintEvent *event);
protected slots:
    void jsonDataUpdated(QString id);
private slots:
    void ConfirmSlots();
    void CancelSlots();
    void passwordChangeSlots(const QString &address);
private:
    void InitData();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::CapitalConfirmWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // CAPITALCONFIRMWIDGET_H
