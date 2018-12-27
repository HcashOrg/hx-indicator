#ifndef LOCKPAGE_H
#define LOCKPAGE_H

#include <QWidget>
#include <QLibrary>
#include <QTimer>

namespace Ui {
class LockPage;
}

typedef short(*FUN)(int);

class LockPage : public QWidget
{
    Q_OBJECT

public:
    explicit LockPage(QWidget *parent = 0);
    ~LockPage();

signals:
    void unlock();
    void minimum();
    void closeWallet();
    void tray();
    void showShadowWidget();
    void hideShadowWidget();

private slots:
    void on_enterBtn_clicked();

    void on_pwdLineEdit_returnPressed();

    void on_closeBtn_clicked();

    void on_pwdLineEdit_textChanged(const QString &arg1);

    void jsonDataUpdated(QString id);
    void on_miniBtn_clicked();

private:
    void InitWidget();
    void InitStyle();
private:
    Ui::LockPage *ui;

    void keyPressEvent(QKeyEvent* e);
    void paintEvent(QPaintEvent*e);

};

#endif // LOCKPAGE_H
