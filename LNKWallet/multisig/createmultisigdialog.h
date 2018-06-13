#ifndef CREATEMULTISIGDIALOG_H
#define CREATEMULTISIGDIALOG_H

#include <QDialog>

namespace Ui {
class CreateMultiSigDialog;
}

class CreateMultiSigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateMultiSigDialog(QWidget *parent = 0);
    ~CreateMultiSigDialog();

    void  pop();
    QString multiSigAddress;

    QStringList owners;
    void updateOwnersList();

private slots:
    void jsonDataUpdated(QString id);

    void on_createBtn_clicked();

    void on_cancelBtn_clicked();

    void on_addBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_ownersTableWidget_cellPressed(int row, int column);

    void on_closeBtn_clicked();

private:
    Ui::CreateMultiSigDialog *ui;
};

#endif // CREATEMULTISIGDIALOG_H
