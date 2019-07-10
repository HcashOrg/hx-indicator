#ifndef SHOWTRXCODEDIALOG_H
#define SHOWTRXCODEDIALOG_H

#include <QDialog>

namespace Ui {
class ShowTrxCodeDialog;
}

class ShowTrxCodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowTrxCodeDialog(QString text, QWidget *parent = nullptr);
    ~ShowTrxCodeDialog();

private slots:
    void on_copyBtn_clicked();

private:
    Ui::ShowTrxCodeDialog *ui;
};

#endif // SHOWTRXCODEDIALOG_H
