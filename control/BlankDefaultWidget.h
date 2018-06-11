#ifndef BLANKDEFAULTWIDGET_H
#define BLANKDEFAULTWIDGET_H

#include <QWidget>

namespace Ui {
class BlankDefaultWidget;
}

class BlankDefaultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BlankDefaultWidget(QWidget *parent = 0);
    ~BlankDefaultWidget();
public:
    void setTextTip(const QString &tip);
private:
    void InitWidget();

protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::BlankDefaultWidget *ui;
};

#endif // BLANKDEFAULTWIDGET_H
