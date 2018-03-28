#ifndef MYCHECKBTN_H
#define MYCHECKBTN_H

#include "QToolButton"

class MyCheckBtn : public QToolButton
{
    Q_OBJECT
public:
    explicit MyCheckBtn(QWidget *parent = 0);

    void setCheckStyle(QString style);
    void setUncheckStyle(QString style);

    void setChecked(bool _isChecked);

    bool isChecked;
    QString checkedStyle;
    QString uncheckedStyle;

signals:
    void pressed(bool isChecked);

protected:
    virtual void mousePressEvent(QMouseEvent *event);

};

#endif // MYCHECKBTN_H
