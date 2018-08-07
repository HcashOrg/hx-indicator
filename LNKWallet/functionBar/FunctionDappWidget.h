#ifndef FUNCTIONDAPPWIDGET_H
#define FUNCTIONDAPPWIDGET_H

#include <QWidget>

namespace Ui {
class FunctionDappWidget;
}

class FunctionDappWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionDappWidget(QWidget *parent = 0);
    ~FunctionDappWidget();
    void retranslator();
public slots:
    void DefaultShow();
private:
    void InitWidget();
    void InitStyle();

signals:
    void showContractTokenSignal();
private slots:

    void on_contractTokenBtn_clicked();

private:
    Ui::FunctionDappWidget *ui;

    void paintEvent(QPaintEvent*);
};

#endif // FUNCTIONDAPPWIDGET_H
