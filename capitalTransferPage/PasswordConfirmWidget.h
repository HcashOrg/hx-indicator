#ifndef PASSWORDCONFIRMWIDGET_H
#define PASSWORDCONFIRMWIDGET_H

#include <QWidget>

namespace Ui {
class PasswordConfirmWidget;
}

class PasswordConfirmWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PasswordConfirmWidget(QWidget *parent = 0);
    ~PasswordConfirmWidget();

private:
    Ui::PasswordConfirmWidget *ui;
};

#endif // PASSWORDCONFIRMWIDGET_H
