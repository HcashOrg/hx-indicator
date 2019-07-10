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
    void retranslator();
public slots:
    void DefaultShow();
signals:
    void showPoundageSignal();
    void showMultiSigSignal();
    void showNameTransferSignal();
private slots:
    void PoundageShowSlots();
    void MultiSigShowSlots();
    void NameTransferShowSlots();

private:
    void InitWidget();
    void InitStyle();
private:
    Ui::FunctionAdvanceWidget *ui;

    void paintEvent(QPaintEvent*);
};

#endif // FUNCTIONADVANCEWIDGET_H
