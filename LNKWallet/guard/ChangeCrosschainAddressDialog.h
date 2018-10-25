#ifndef CHANGECROSSCHAINADDRESSDIALOG_H
#define CHANGECROSSCHAINADDRESSDIALOG_H

#include <QDialog>
#include "extra/HttpManager.h"

namespace Ui {
class ChangeCrosschainAddressDialog;
}

struct MultisigPair
{
    QString id;
    QString symbol;
    QString hotAddress;
    QString coldAddress;
    int     effectiveBlock = -1;
};

struct ETHMultiAccountCreateTrx
{
    QString symbol;
    QString trxId;
    QString guardSignHotAddress;
    QString guardSignColdAddress;
};

class ChangeCrosschainAddressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeCrosschainAddressDialog(QWidget *parent = 0);
    ~ChangeCrosschainAddressDialog();

    void pop();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void httpReplied(QByteArray _data, int _status);

    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::ChangeCrosschainAddressDialog *ui;
    ETHMultiAccountCreateTrx ethTrx;
    HttpManager httpManager;
    int needSenatorSign = false;
    QString signerId;
    double hotBalance = 0;
    double coldBalance = 0;

    void queryMultiAccountPair();
    void getSenatorMultiAddress();
};

#endif // CHANGECROSSCHAINADDRESSDIALOG_H
