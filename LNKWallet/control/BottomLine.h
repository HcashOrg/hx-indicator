#ifndef BOTTOMLINE_H
#define BOTTOMLINE_H

#include <QFrame>

class BottomLine : public QFrame
{
    Q_OBJECT
public:
    explicit BottomLine(QWidget *parent = 0);

    void setWidth(int _width);
    void setHeight(int _height);

    void attachToWidget(QWidget* w);

    QWidget* currentAttached()
    {
        return currentAttachedWidget;
    }

private:
    QWidget* currentAttachedWidget = nullptr;
};

#endif // BOTTOMLINE_H
