#ifndef KLINEWIDGET_H
#define KLINEWIDGET_H

#include <QWidget>

namespace Ui {
class KLineWidget;
}

class KLineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KLineWidget(QWidget *parent = nullptr);
    ~KLineWidget();

private:
    Ui::KLineWidget *ui;
};

#endif // KLINEWIDGET_H
