#ifndef CONTACTWIDGET_H
#define CONTACTWIDGET_H

#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>联系人总界面 </summary>
///
///<remarks> 2018.03.30 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
class QCloseEvent;

namespace Ui {
class ContactWidget;
}

class ContactWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactWidget(QWidget *parent = 0);
    ~ContactWidget();
signals:
    //发送转账信号（frame中链接对应的槽函数，显示转账界面）
    void gotoTransferPage(QString,QString);
    void backBtnVisible(bool);
private slots:
    //添加联系人
    void AddNewContactSlots();
    //显示联系人
    void ShowContactInfoSlots(const QString &address);
private:
    void InitStyle();
    void InitWidget();
    void InitData();
    void SaveData();
protected:
    void closeEvent(QCloseEvent *event);
    //void paintEvent(QPaintEvent *event);
private:
    Ui::ContactWidget *ui;
private:
    class ContactWidgetPrivate;
    ContactWidgetPrivate *_p;
};

#endif // CONTACTWIDGET_H
