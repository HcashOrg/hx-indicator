#ifndef EXITINGWIDGET_H
#define EXITINGWIDGET_H

#include <QWidget>

namespace Ui {
class ExitingWidget;
}

class ExitingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExitingWidget(QWidget *parent = 0);
    ~ExitingWidget();

private slots:
    void on_closeBtn_clicked();

private:
    Ui::ExitingWidget *ui;

    void paintEvent(QPaintEvent* e);
};

#endif // EXITINGWIDGET_H
