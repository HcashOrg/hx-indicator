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
    explicit PageScrollWidget(unsigned int buttonNumber = 5,QWidget *parent = 0);
    ~PageScrollWidget();
public:
    void SetTotalPage(unsigned int number);//count

    void SetCurrentPage(unsigned int number);///--totalpage

    unsigned int GetTotalPage()const;
    unsigned int GetCurrentPage()const;

public:
    void setShowTip(unsigned int totalItem,unsigned int pageItem);//设置显示条目提示
signals:
    void currentPageChangeSignal(unsigned int);//0-->totalpage-1
private slots:
    void buttonClickSlots();
    void LineEditFinishSlots();
private:
    void RefreshData();
    void RefreshButton();
private:
    void SwitchMoreButton();
    void RearrangePage();
private:
    void InitWidget();
    void InitStyle();
    void ResetButton();

    void updateShowTip();
protected:
    void resizeEvent(QResizeEvent *event);
private:
    Ui::PageScrollWidget *ui;
private:
    class PageScrollWidgetPrivate;
    PageScrollWidgetPrivate *_p;
};

#endif // PAGESCROLLWIDGET_H
