#ifndef CONTACTBRIEFWIDGET_H
#define CONTACTBRIEFWIDGET_H

#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>联系人简介界面 </summary>
///
///<remarks> 2018.03.30 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class ContactBriefWidget;
}

class ContactBriefWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactBriefWidget(QWidget *parent = 0);
    ~ContactBriefWidget();
signals:
    void addNewContactSignal();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::ContactBriefWidget *ui;
private:
    class ContactBriefWidgetPrivate;
    ContactBriefWidgetPrivate *_p;
};

#endif // CONTACTBRIEFWIDGET_H
