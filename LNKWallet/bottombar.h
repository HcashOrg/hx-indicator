#ifndef BOTTOMBAR_H
#define BOTTOMBAR_H

#include <QWidget>

namespace Ui {
class BottomBar;
}


class BottomBar : public QWidget
{
    Q_OBJECT

public:
    explicit BottomBar(QWidget *parent = 0);
    ~BottomBar();
    void retranslator();
    void refresh();

signals:
    void showCoverWidget();
    void walletInfoUpdated();

private slots:
    void jsonDataUpdated(QString id);
private:
    void CheckBlockSync();
private:
    Ui::BottomBar *ui;
    int numOfConnections;

    void paintEvent(QPaintEvent*);
    bool eventFilter(QObject *watched, QEvent *e);

};

#endif // BOTTOMBAR_H
