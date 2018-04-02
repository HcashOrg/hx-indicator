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
    void gotoTransferPage(QString,QString);
private slots:
    void AddNewContactSlots();
    void ShowContactInfoSlots(const QString &address);
private:
    void InitWidget();
    void InitData();
    void SaveData();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::ContactWidget *ui;
private:
    class ContactWidgetPrivate;
    ContactWidgetPrivate *_p;
};

#endif // CONTACTWIDGET_H
