#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class TitleBar;
}

int applyEnable();    //  返回可以申请代理的账户个数

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = 0);
    ~TitleBar();
    void retranslator();

signals:
    void minimum();
    void closeWallet();
    void tray();

    void back();
    void refresh();
public slots:
    void backBtnVis(bool isVisible = true);
    void extendToWidth(int _width);
private slots:
    void on_minBtn_clicked();

    void on_closeBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_refreshBtn_clicked();

    void gifPlayOnce(int num);

private:
    Ui::TitleBar *ui;

    QTimer* refreshTimer = nullptr;
    bool refreshing = false;
    QMovie* gif = nullptr;
    QTimer* gifTimer = nullptr;

    void paintEvent(QPaintEvent*);
};

#endif // TITLEBAR_H
