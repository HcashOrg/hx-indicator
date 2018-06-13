#ifndef DEPOSITQRCODEWIDGET_H
#define DEPOSITQRCODEWIDGET_H

#include <QWidget>

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
    void SetSymbol(const QString &symbol);
private:
    void CopySlots();
private:
    void InitWidget();
    void InitStyle();
protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::DepositQrcodeWidget *ui;
};

#endif // DEPOSITQRCODEWIDGET_H
