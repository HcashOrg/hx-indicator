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
    explicit ExchangePairSelectDialog(QString _quoteAssetSymbol, QWidget *parent = nullptr);
    ~ExchangePairSelectDialog();

private slots:
    void on_addBtn_clicked();

signals:
    void pairSelected(ExchangePair);
    void addFavoriteClicked();
private:
    QTableWidget* tableWidget = nullptr;
    void showPairs();
private slots:
    void onCellWidgetClicked(int _row, int _column);

private:
    Ui::ExchangePairSelectDialog *ui;
    QString quoteAssetSymbol;       // 如果为空则显示自选 显示addBtn

    bool event(QEvent *event);
    void paintEvent(QPaintEvent* event);
};

#endif // EXCHANGEPAIRSELECTDIALOG_H
