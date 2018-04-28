#ifndef ACCOUNTMANAGERWIDGET_H
#define ACCOUNTMANAGERWIDGET_H

#include <QWidget>

namespace Ui {
class AccountManagerWidget;
}

class AccountManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountManagerWidget(QWidget *parent = 0);
    ~AccountManagerWidget();
private slots:
    void pageChangeSlot(unsigned int page);

    void deleteButtonSlots();
    void exportButtonSlots();

    void backupButtonSlots();

    void jsonDataUpdated(QString id);
private:
    void RefreshTableShow(unsigned int page);
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::AccountManagerWidget *ui;
    class AccountManagerWidgetPrivate;
    AccountManagerWidgetPrivate *_p;
};

#endif // ACCOUNTMANAGERWIDGET_H
