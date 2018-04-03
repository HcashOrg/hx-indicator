#ifndef CONTACTINFOWIDGET_H
#define CONTACTINFOWIDGET_H

#include <memory>
#include <QWidget>

//////////////////////////////////////////////////////////////////////////
///<summary>联系人详情界面(暂时未实现交易记录显示功能) </summary>
///
///<remarks> 2018.03.30 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////

class ContactPerson;

namespace Ui {
class ContactInfoWidget;
}

class ContactInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ContactInfoWidget(QWidget *parent = 0);
    ~ContactInfoWidget();
signals:
    void gotoTransferPage(QString,QString);
public slots:
    void PersonModifyFinishedSlots();
private slots:
    void transferAccountSlots();
public:
    //设置界面数据源
    void setData(const std::shared_ptr<ContactPerson> &person);
private:
    void InitWidget();
    void RefreshWidget();
protected:
    //void paintEvent(QPaintEvent *);
    void paintEvent(QPaintEvent *event);
private:
    class ContactInfoWidgetPrivate;
    ContactInfoWidgetPrivate *_p;
private:
    Ui::ContactInfoWidget *ui;
};

#endif // CONTACTINFOWIDGET_H
