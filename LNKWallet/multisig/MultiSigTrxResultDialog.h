#ifndef MULTISIGTRXRESULTDIALOG_H
#define MULTISIGTRXRESULTDIALOG_H

#include <QDialog>

namespace Ui {
class MultiSigTrxResultDialog;
}

class MultiSigTrxResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiSigTrxResultDialog(QWidget *parent = 0);
    ~MultiSigTrxResultDialog();

    void setTrxCode(QString code);
    void pop();

private slots:
    void jsonDataUpdated(QString id);

    void on_closeBtn_clicked();

    void on_okBtn_clicked();

    void on_copyBtn_clicked();

    void on_copyBtn2_clicked();


private:
    Ui::MultiSigTrxResultDialog *ui;
};

#endif // MULTISIGTRXRESULTDIALOG_H
