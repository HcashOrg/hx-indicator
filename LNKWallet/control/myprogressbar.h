#ifndef MYPROGRESSBAR_H
#define MYPROGRESSBAR_H

#include <QWidget>
#include <QProgressBar>
#include <QTimer>

class MyProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    MyProgressBar(QWidget* parent = 0);
    ~MyProgressBar();


protected:
    void paintEvent(QPaintEvent* e);

private:
    int m_cacheValue = 1;
    QTimer m_timer;


};

#endif // MYPROGRESSBAR_H
