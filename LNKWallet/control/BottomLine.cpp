#include "BottomLine.h"


BottomLine::BottomLine(QWidget *parent):
    QFrame(parent)
{
    setStyleSheet("QFrame{background-color: rgb(83, 61, 138);}");

    setWidth(30);
    setHeight(2);
}

void BottomLine::setWidth(int _width)
{
    setGeometry( this->x(), this->y(), _width, this->height());
}

void BottomLine::setHeight(int _height)
{
    setGeometry( this->x(), this->y(), this->width(), _height);
}

void BottomLine::attachToWidget(QWidget *w)
{
    currentAttachedWidget = w;

    if(w)
    {
        int x = w->x() + w->width() / 2 - this->width() / 2;
        int y = w->y() + w->height() + 2;
        setGeometry( x, y, this->width(), this->height());
    }
}
