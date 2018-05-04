#ifndef MESSAGENOTIFYWIDGET_H
#define MESSAGENOTIFYWIDGET_H

#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>右下角窗口提示（调用ShowWidget即可） </summary>
///
///<remarks> 2018.05.04 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class MessageNotifyWidget;
}

class MessageNotifyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageNotifyWidget(const QString &info = " ",QWidget *parent = 0);
    ~MessageNotifyWidget();
public:
    void SetMessageContext(const QString &info);//修改提示内容

    void ShowWidget();//显示窗口
private slots:
    void OnShowSlots();
    void OnStaySlots();
    void OnCloseSlots();
protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
private:
    void InitWdiget();
    void InitStyle();
private:
    Ui::MessageNotifyWidget *ui;
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // MESSAGENOTIFYWIDGET_H
