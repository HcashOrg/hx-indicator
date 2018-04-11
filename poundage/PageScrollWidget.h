#ifndef PAGESCROLLWIDGET_H
#define PAGESCROLLWIDGET_H

#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>底部页码界面 </summary>
///
///<remarks> 2018.04.11 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class PageScrollWidget;
}

class PageScrollWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PageScrollWidget(QWidget *parent = 0);
    ~PageScrollWidget();
public:
    void SetTotalPage(unsigned int number);
private:
    void RefreshButton();
private:
    void InitWidget();
    void InitStyle();
    void ResetButton();
private:
    Ui::PageScrollWidget *ui;
private:
    class PageScrollWidgetPrivate;
    PageScrollWidgetPrivate *_p;
};

#endif // PAGESCROLLWIDGET_H
