#ifndef FUNCTIONACCOUNTWIDGET_H
#define FUNCTIONACCOUNTWIDGET_H

#include <QWidget>

namespace Ui {
class FunctionAccountWidget;
}

class FunctionAccountWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionAccountWidget(QWidget *parent = 0);
    ~FunctionAccountWidget();

    void retranslator();
public slots:
    void DefaultShow();
private slots:
    void AssetShowSlots();
    void MinerShowSlots();
    void BonusShowSlots();

signals:
    void showAccountSignal();
    void showMinerSignal();
    void showBonusSignal();

private:
    void InitWidget();
    void InitStyle();
private:
    Ui::FunctionAccountWidget *ui;

    void paintEvent(QPaintEvent*);
};

#endif // FUNCTIONACCOUNTWIDGET_H
