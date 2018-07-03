#ifndef CHANGECROSSCHAINADDRESSDIALOG_H
#define CHANGECROSSCHAINADDRESSDIALOG_H

#include <QDialog>

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

    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::ChangeCrosschainAddressDialog *ui;
};

#endif // CHANGECROSSCHAINADDRESSDIALOG_H
