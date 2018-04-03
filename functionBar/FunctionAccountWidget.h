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
private:
    void InitWidget();
private:
    Ui::FunctionAccountWidget *ui;
};

#endif // FUNCTIONACCOUNTWIDGET_H
