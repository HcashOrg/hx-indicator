#ifndef COLDHOTTRANSFERDIALOG_H
#define COLDHOTTRANSFERDIALOG_H

#include <QDialog>
#include "extra/HttpManager.h"


namespace Ui {
class ColdHotTransferDialog;
}

class ColdHotTransferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColdHotTransferDialog(QWidget *parent = 0);
    ~ColdHotTransferDialog();

    void pop();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void httpReplied(QByteArray _data, int _status);

    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_typeComboBox_currentIndexChanged(const QString &arg1);

    void on_amountLineEdit_textEdited(const QString &arg1);

    void on_fromLineEdit_textEdited(const QString &arg1);

private:
    Ui::ColdHotTransferDialog *ui;
    HttpManager httpManager;
    double queriedBalance = 0;

    void showAddresses();
    void queryMultisigBalance();
    void checkOkBtnEnable();
};

#endif // COLDHOTTRANSFERDIALOG_H
