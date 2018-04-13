#ifndef CONTACTADDWIDGET_H
#define CONTACTADDWIDGET_H

#include <memory>
#include <QWidget>

//////////////////////////////////////////////////////////////////////////
///<summary>新增联系人界面 </summary>
///
///<remarks> 2018.03.30 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
class ContactSheet;

namespace Ui {
class ContactAddWidget;
}

class ContactAddWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactAddWidget(std::shared_ptr<ContactSheet> contactSheet,QWidget *parent = 0);
    ~ContactAddWidget();
signals:
    void addContactFinishedSignal(const QString &address);
public slots:
    void groupModifySlots();
protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void addressChangeSlots(const QString &address);

    void addNewContactSlots();
private:
    //验证地址合法性
    bool validateAddress(const QString &address);
private:
    void InitStyle();
    void InitWidget();

    void InitComboBox();
private:
    Ui::ContactAddWidget *ui;
private:
    class ContactAddWidgetPrivate;
    ContactAddWidgetPrivate *_p;
};

#endif // CONTACTADDWIDGET_H
