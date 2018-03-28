#ifndef NORMALLOGIN_H
#define NORMALLOGIN_H

#include <QWidget>
#include <QFileInfo>
#include <QTimer>
#include <QLibrary>

namespace Ui {
class NormalLogin;
}

typedef short(*FUN)(int);

class NormalLogin : public QWidget
{
    Q_OBJECT

public:
    explicit NormalLogin(QWidget *parent = 0);
    ~NormalLogin();

private slots:

    void on_enterBtn_clicked();

    void on_pwdLineEdit_returnPressed();

    void on_closeBtn_clicked();

    void on_pwdLineEdit_textChanged(const QString &arg1);

    void pwdConfirmed(QString id);

signals:
    void login();
    void minimum();
    void closeWallet();
    void tray();
    void showShadowWidget();
    void hideShadowWidget();

private:
    Ui::NormalLogin *ui;

    void keyPressEvent(QKeyEvent* e);
    void paintEvent(QPaintEvent*e);
};

#endif // NORMALLOGIN_H
