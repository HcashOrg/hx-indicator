#ifndef MULTISIGINFODIALOG_H
#define MULTISIGINFODIALOG_H

#include <QDialog>

namespace Ui {
class MultiSigInfoDialog;
}

class MultiSigInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiSigInfoDialog(QString _multiSigAddress, QWidget *parent = 0);
    ~MultiSigInfoDialog();

    QString multiSigAddress;

    void  pop();

    void showMultiSigInfo();

private slots:
    void on_okBtn_clicked();

    void on_infoTableWidget_cellPressed(int row, int column);

    void on_closeBtn_clicked();

private:
    Ui::MultiSigInfoDialog *ui;
};

#endif // MULTISIGINFODIALOG_H
