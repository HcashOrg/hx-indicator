#ifndef CONTACTINFOHISTORYWIDGET_H
#define CONTACTINFOHISTORYWIDGET_H

#include <QWidget>

namespace Ui {
class ContactInfoHistoryWidget;
}

class ContactInfoHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactInfoHistoryWidget(QWidget *parent = 0);
    ~ContactInfoHistoryWidget();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::ContactInfoHistoryWidget *ui;
private:
    class ContactInfoHistoryWidgetPrivate;
    ContactInfoHistoryWidgetPrivate *_p;
};

#endif // CONTACTINFOHISTORYWIDGET_H
