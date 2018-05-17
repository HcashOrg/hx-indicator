#ifndef FEECHARGEWIDGET_H
#define FEECHARGEWIDGET_H

#include <QWidget>

namespace Ui {
class FeeChargeWidget;
}

class FeeChargeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FeeChargeWidget(double feeNumber = 20,const QString &feeType = "LNK",QWidget *parent = 0);
    ~FeeChargeWidget();
public:
    void SetInfo(const QString &info,bool vi = true);
signals:
    void cancelSignal();
    void confirmSignal();
private slots:
    void ConfirmSlots();
    void CancelSlots();
protected:
    void paintEvent(QPaintEvent *event);
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::FeeChargeWidget *ui;
    class FeeChargeWidgetPrivate;
    FeeChargeWidgetPrivate *_p;
};

#endif // FEECHARGEWIDGET_H
