#include "mycheckbtn.h"

MyCheckBtn::MyCheckBtn(QWidget *parent)
    : QToolButton(parent),
      isChecked(false)
{
    setCursor(Qt::PointingHandCursor);
}

void MyCheckBtn::setCheckStyle(QString style)
{
    checkedStyle = style;
}

void MyCheckBtn::setUncheckStyle(QString style)
{
    uncheckedStyle = style;
}

void MyCheckBtn::setChecked(bool _isChecked)
{
    isChecked = _isChecked;
    if(isChecked)
    {
        setStyleSheet(checkedStyle);
    }
    else
    {
        setStyleSheet(uncheckedStyle);
    }
}

void MyCheckBtn::mousePressEvent(QMouseEvent *event)
{
    emit pressed(isChecked);
}
