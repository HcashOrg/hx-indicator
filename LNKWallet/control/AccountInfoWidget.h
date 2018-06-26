#ifndef ACCOUNTINFOWIDGET_H
#define ACCOUNTINFOWIDGET_H

#include <QWidget>

namespace Ui {
class AccountInfoWidget;
}

class AccountInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountInfoWidget(QWidget *parent = 0);
    ~AccountInfoWidget();

    void init();

    void setAccount(QString accountName);

private:
    Ui::AccountInfoWidget *ui;
};

#endif // ACCOUNTINFOWIDGET_H
