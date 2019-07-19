#ifndef NAMETRANSFERDIALOG_H
#define NAMETRANSFERDIALOG_H

#include <QDialog>

namespace Ui {
class NameTransferDialog;
}

class NameTransferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NameTransferDialog(QWidget *parent = nullptr);
    ~NameTransferDialog();

    void pop();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_newNameLineEdit_textChanged(const QString &arg1);

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_amountLineEdit_textChanged(const QString &arg1);

    void on_toAddressLineEdit_textChanged(const QString &arg1);

private:
    Ui::NameTransferDialog *ui;
    QString _decodedStr;
    QString _trxCode;

    bool newNameOk = false;
    void checkOkBtnEnabled();

};

#endif // NAMETRANSFERDIALOG_H
