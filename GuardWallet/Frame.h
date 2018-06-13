#ifndef FRAME_H
#define FRAME_H

#include <QWidget>

class Frame : public QWidget
{
    Q_OBJECT

public:
    Frame(QWidget *parent = 0);
    ~Frame();
};

#endif // FRAME_H
