#ifndef EXCHANGEPAIRSELECTDIALOG_H
#define EXCHANGEPAIRSELECTDIALOG_H

#include <QDialog>
#include <QTableWidget>

#include "wallet.h"

namespace Ui {
class ExchangePairSelectDialog;
}

class ExchangePairSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExchangePairSelectDialog(bool _showAddBtn = true, QWidget *parent = nullptr);
    ~ExchangePairSelectDialog();

private slots:
    void on_addBtn_clicked();

signals:
    void pairSelected(ExchangePair);
private:
    QTableWidget* tableWidget = nullptr;
    void showPairs();
private slots:
    void onCellWidgetClicked(int _row, int _column);

private:
    Ui::ExchangePairSelectDialog *ui;
    bool showAddBtn = true;

    bool event(QEvent *event);
    void paintEvent(QPaintEvent* event);
};

#endif // EXCHANGEPAIRSELECTDIALOG_H
