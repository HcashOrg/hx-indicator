#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <mutex>
#include <QDialog>
#include "KeyDataUtil.h"
namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog( QString name, QWidget *parent = 0);
    ~ExportDialog();

    void  pop();

protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_pathBtn_clicked();

    void on_cancelBtn_clicked();

    void on_exportBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_encryptCheckBox_stateChanged(int arg1);

    void on_closeBtn_clicked();

private:
    Ui::ExportDialog *ui;
    QString accoutName;
    QString privateKey;
//    QMap<QString,QString>   assetTunnelPrivateKeyMap;
    bool isEncrypt = true;
    QString pwd;
    QString path;
    KeyDataPtr dataInfo;
    std::mutex mutex;


    void getPrivateKey();

    void ParseTunnel(const QString &jsonString);

    void ParseAllKey(const QString &jsonString);

    bool SaveToPath();
};

#endif // EXPORTDIALOG_H
