#ifndef DEPOSITQRCODEWIDGET_H
#define DEPOSITQRCODEWIDGET_H

#include <QWidget>
#include "extra/HttpManager.h"

namespace Ui {
class DepositQrcodeWidget;
}

class DepositQrcodeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DepositQrcodeWidget(QWidget *parent = 0);
    ~DepositQrcodeWidget();
public:
    void SetQRString(const QString &data);
    void SetAddress(const QString &address);
    void SetSymbol(const QString &_symbol);
private:
    void CopySlots();

private:
    QString symbol;
    HttpManager httpManager;////用于查询通道账户余额
    void PostQueryTunnelMoney(const QString &symbol,const QString &tunnelAddress);
private slots:
    void httpReplied(QByteArray _data, int _status);

private:
    void InitWidget();
    void InitStyle();
protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::DepositQrcodeWidget *ui;
};

#endif // DEPOSITQRCODEWIDGET_H
