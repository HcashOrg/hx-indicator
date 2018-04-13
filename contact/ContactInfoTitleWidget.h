#ifndef CONTACTINFOTITLEWIDGET_H
#define CONTACTINFOTITLEWIDGET_H

#include <memory>
#include <QWidget>

//////////////////////////////////////////////////////////////////////////
///<summary>联系人名称等信息界面 </summary>
///
///<remarks> 2018.03.30 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
class ContactPerson;

namespace Ui {
class ContactInfoTitleWidget;
}

class ContactInfoTitleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactInfoTitleWidget(QWidget *parent = 0);
    ~ContactInfoTitleWidget();
signals:
    void transferAccountSignal();
private slots:
    void copyAdressSlots();
    void transferAccountSlots();
public:
    //设置数据
    void setData(const std::shared_ptr<ContactPerson> &person);

    //修改名称（树节点修改名字时使用）
    void refreshName();
private:
    void InitWidget();
    void InitStyle();
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
private:
    class ContactInfoTitleWidgetPrivate;
    ContactInfoTitleWidgetPrivate *_p;
private:
    Ui::ContactInfoTitleWidget *ui;
};

#endif // CONTACTINFOTITLEWIDGET_H
