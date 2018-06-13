#ifndef PASSWORDCONFIRMWIDGET_H
#define PASSWORDCONFIRMWIDGET_H

#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>密码确认界面，成功确认密码发出确认信号（confirmsignal） </summary>
///
///<remarks> 2018.04.18 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class PasswordConfirmWidget;
}

class PasswordConfirmWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PasswordConfirmWidget(QWidget *parent = 0);
    ~PasswordConfirmWidget();
signals:
    void confirmSignal();
    void cancelSignal();
private slots:
    void ConfirmSlots();
    void CancelSlots();

    void passwordChangeSlots(const QString &address);

    void jsonDataUpdated(QString id);

    void passwordReturnPressed();
protected:
    void paintEvent(QPaintEvent *event);

private:
    void InitWidget();
    void InitStyle();
private:
    Ui::PasswordConfirmWidget *ui;
};

#endif // PASSWORDCONFIRMWIDGET_H
