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
private slots:
    void ConfirmSlots();
    void CancelSlots();

    void passwordChangeSlots(const QString &address);

    void jsonDataUpdated(QString id);
protected:
    void paintEvent(QPaintEvent *event);
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::PasswordConfirmWidget *ui;
};

#endif // PASSWORDCONFIRMWIDGET_H
