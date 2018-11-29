#ifndef AUTOUPDATENOTIFY_H
#define AUTOUPDATENOTIFY_H

#include <QObject>

class AutoUpdateNotify : public QObject
{
    Q_OBJECT
public:
    explicit AutoUpdateNotify(QObject *parent = nullptr);
    ~AutoUpdateNotify();
public:
    void startAutoDetect();
    void stopAutoDetect();
private:
    void checkUpdate();
    void InitData();
private slots:
    void CheckResultSlot(const QString &version,bool isupdateForce);
    void UpdateWrongSlot();
    void UpdateFinishSlot();
private:
    void recheckNow();
private:
    class DataPrivate;
    DataPrivate *_p;

};

#endif // AUTOUPDATENOTIFY_H
