#ifndef BACKUPWALLETDIALOG_H
#define BACKUPWALLETDIALOG_H

#include <QDialog>

namespace Ui {
class BackupWalletDialog;
}

class BackupWalletDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BackupWalletDialog(QWidget *parent = 0);
    ~BackupWalletDialog();

    void pop();

protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_backupBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pathBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::BackupWalletDialog *ui;
};

#endif // BACKUPWALLETDIALOG_H
