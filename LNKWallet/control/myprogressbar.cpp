#include "myprogressbar.h"

#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QApplication>

MyProgressBar::MyProgressBar(QWidget* parent):
    QProgressBar(parent)
{

//    connect(&m_timer, &QTimer::timeout, this, [this]{
//        m_cacheValue++;
//        repaint();
//    });

//    m_timer.start(20);

}

MyProgressBar::~MyProgressBar()
{

}

void MyProgressBar::paintEvent(QPaintEvent *e)
{
//    QStyleOptionProgressBarV2 opt;
//    QRect outerRect = style()->subElementRect(QStyle::SE_ProgressBarGroove, &opt, this);
//    QRect innerRect = style()->subElementRect(QStyle::SE_ProgressBarContents, &opt, this);
////    QMargins borders();
//qDebug() << "ppppppppppppp " << outerRect  << innerRect;
//    QRectF rect(innerRect);
//        QRect labelRect = style()->subElementRect(QStyle::SE_ProgressBarLabel, &opt, this);


    QPainter painter(this);
    painter.setBrush(QColor(229,226,240));
    painter.setPen(QColor(110,160,207));
    painter.drawRect(0,0,width() - 1, height() - 1);

    int length = this->width() * (value() - minimum()) / (maximum() - minimum());
    painter.setBrush(QColor(90,144,207));
    painter.setPen(Qt::NoPen);
    painter.drawRect(1,1, length, height());

    if (m_cacheValue != 0 && length > 0)
    {
        QLinearGradient Linear(0,0,m_cacheValue * 2,height());
        Linear.setColorAt(0,QColor(50,100,100));
        Linear.setColorAt(1,QColor(50,170,100));
        painter.setBrush(Linear);
        painter.drawRect(1,1, m_cacheValue * 1, height());
        if(m_cacheValue * 1 >= length)  m_cacheValue = 0;
    }


    QFont font("\"Microsoft YaHei\"",9,50);
    font.setPixelSize(12);
    painter.setFont(font);
    painter.setPen(QColor(83,61,138));
    int widthOfTitle = painter.fontMetrics().width(text());
    painter.drawText( width() / 2 - widthOfTitle / 2, 17, text());
}




