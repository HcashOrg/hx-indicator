#ifndef FUNCTIONADVANCEWIDGET_H
#define FUNCTIONADVANCEWIDGET_H

#include <QWidget>

namespace Ui {
class FunctionAdvanceWidget;
}

class FunctionAdvanceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionAdvanceWidget(QWidget *parent = 0);
    ~FunctionAdvanceWidget();
signals:
    void showFeedPage();
    void showMultiSigPage();
private:
    void InitWidget();
private:
    Ui::FunctionAdvanceWidget *ui;
};

#endif // FUNCTIONADVANCEWIDGET_H
