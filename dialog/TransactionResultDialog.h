#ifndef TRANSACTIONRESULTDIALOG_H
#define TRANSACTIONRESULTDIALOG_H

#include <QDialog>

namespace Ui {
class TransactionResultDialog;
}

class TransactionResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransactionResultDialog(QWidget *parent = 0);
    ~TransactionResultDialog();

    void setInfoText(QString _text);
    void setDetailText(QString _text);

    void pop();

private slots:
    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_copyBtn_clicked();

private:
    Ui::TransactionResultDialog *ui;
};

#endif // TRANSACTIONRESULTDIALOG_H
