#ifndef WAITINGFORSYNC_H
#define WAITINGFORSYNC_H

#include <QWidget>
#include <QDomDocument>

namespace Ui {
class WaitingForSync;
}

class QNetworkReply;

class WaitingForSync : public QWidget
{
    Q_OBJECT

public:
    explicit WaitingForSync(QWidget *parent = 0);
    ~WaitingForSync();


signals:
    void sync();
    void minimum();
    void tray();
    void closeWallet();

private slots:
    void on_closeBtn_clicked();

    void outputMessage(const QString &message);

public:
    QTimer* timerForWSConnected;
private slots:
    void checkConnected();

private:
    QTimer* timerForPic;
    int rotateNum;
private slots:
    void showPic();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::WaitingForSync *ui;

    void paintEvent(QPaintEvent*e);
};

#endif // WAITINGFORSYNC_H
