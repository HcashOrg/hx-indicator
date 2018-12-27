#ifndef EXCHANGEPAIRSELECTDIALOG_H
#define EXCHANGEPAIRSELECTDIALOG_H

#include <QDialog>

namespace Ui {
class ExchangePairSelectDialog;
}

class ExchangePairSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExchangePairSelectDialog(QWidget *parent = nullptr);
    ~ExchangePairSelectDialog();

private slots:
    void on_addBtn_clicked();

private:
    Ui::ExchangePairSelectDialog *ui;
};

#endif // EXCHANGEPAIRSELECTDIALOG_H
