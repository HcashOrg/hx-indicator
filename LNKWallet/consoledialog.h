#ifndef CONSOLEDIALOG_H
#define CONSOLEDIALOG_H

#include <QDialog>
#include <QString>
#include <QStringList>

namespace Ui {
class ConsoleDialog;
}

class ConsoleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConsoleDialog(QWidget *parent = 0);
    ~ConsoleDialog();

//    virtual void  setVisible(bool visiable);

    void pop();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_closeBtn_clicked();

    void on_consoleLineEdit_returnPressed();

    void jsonDataUpdated(QString id);

    void on_clearBtn_clicked();
private:
    void parseHelpCommand(const QString &helpStr,QStringList &helpList);
private:
    Ui::ConsoleDialog *ui;
    QVector<QString> cmdVector;
    int cmdIndex;

    bool eventFilter(QObject *watched, QEvent *e);
    bool mouse_press;
    QPoint move_point;
};

#endif // CONSOLEDIALOG_H
