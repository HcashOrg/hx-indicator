#ifndef FEEGUARANTEEWIDGET_H
#define FEEGUARANTEEWIDGET_H

#include <QWidget>

namespace Ui {
class FeeGuaranteeWidget;
}

class FeeGuaranteeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FeeGuaranteeWidget( QString _guaranteeId, double _feeAmount, QString _trxId, QWidget *parent = 0);
    ~FeeGuaranteeWidget();

    void setBackgroundColor(QString color);

private slots:
    void jsonDataUpdated(QString id);

private:
    Ui::FeeGuaranteeWidget *ui;
    void fetchGuaranteeOrder();

    QString guaranteeId;
    double feeAmount;
    QString trxId;
};

#endif // FEEGUARANTEEWIDGET_H
