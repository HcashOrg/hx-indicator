#ifndef CONTACTCHOOSEWIDGET_H
#define CONTACTCHOOSEWIDGET_H

#include <QWidget>
class QTreeWidgetItem;
namespace Ui {
class ContactChooseWidget;
}

class ContactChooseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactChooseWidget(QWidget *parent = 0);
    ~ContactChooseWidget();
public:
    bool isInitSuccess()const;
signals:
    void selectContactSignal(const QString &name,const QString &address);
    void closeSignal();
private slots:
    void itemClickedSlots(QTreeWidgetItem *item);
    void itemDoubleClickedSlots(QTreeWidgetItem *item);

    void confirmSlots();

    //执行查询（会选中符合查询的联系人）
    void QueryPersonSlots();

private:
    void updateUI();
private:
    void InitWidget();
    void InitStyle();
    void InitData();
protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::ContactChooseWidget *ui;
    class ContactChooseWidgetPrivate;
    ContactChooseWidgetPrivate *_p;
};

#endif // CONTACTCHOOSEWIDGET_H
