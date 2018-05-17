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

signals:
    void showAccountSignal();
    void showMinerSignal();

private:
    void InitWidget();
    void InitStyle();
private:
    Ui::FunctionAccountWidget *ui;
};

#endif // FUNCTIONACCOUNTWIDGET_H
