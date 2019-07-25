#ifndef COVERWIDGET_H
#define COVERWIDGET_H

#include <QWidget>
#include "myprogressbar.h"

namespace Ui {
class CoverWidget;
}

class CoverWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CoverWidget(QWidget *parent = nullptr);
    ~CoverWidget();


    void retranslator();

    bool firstDisplay = true;

public slots:
    void updateData();

private slots:
    void on_hideBtn_clicked();

private:
    Ui::CoverWidget *ui;
    MyProgressBar* m_progressBar = nullptr;
};

#endif // COVERWIDGET_H
