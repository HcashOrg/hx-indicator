#ifndef CONTACTSEARCHWIDGET_H
#define CONTACTSEARCHWIDGET_H

#include <QWidget>

//////////////////////////////////////////////////////////////////////////
///<summary>搜索界面 </summary>
///
///<remarks> 2018.03.30 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class ContactSearchWidget;
}

class ContactSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactSearchWidget(QWidget *parent = 0);
    ~ContactSearchWidget();
signals:
    void searchSignal(const QString &searchString);
private slots:
    void StartSearchSlots();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::ContactSearchWidget *ui;
private:
    class ContactSearchWidgetPrivate;
    ContactSearchWidgetPrivate *_p;
};

#endif // CONTACTSEARCHWIDGET_H
