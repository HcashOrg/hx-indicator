#ifndef ADDMYEXCHANGEPAIRSDIALOG_H
#define ADDMYEXCHANGEPAIRSDIALOG_H

#include <QDialog>

namespace Ui {
class AddMyExchangePairsDialog;
}

class CheckExchangePairWidget;
class BottomLine;
typedef QPair<QString,QString>  ExchangePair;

class AddMyExchangePairsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddMyExchangePairsDialog(QWidget *parent = nullptr);
    ~AddMyExchangePairsDialog();

    void pop();

private slots:
    void on_marketBtn1_clicked();

//    void on_marketBtn2_clicked();

    void on_marketBtn3_clicked();

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void onCheckStateChanged(ExchangePair pair, bool isChecked);

private:
    Ui::AddMyExchangePairsDialog *ui;
    BottomLine* bottomLine = nullptr;

    QList<CheckExchangePairWidget*> pairWidgetList;
    void closeAllPairWidgets();
    void showPairsByQuoteAsset(QString quoteAsset);
};

#endif // ADDMYEXCHANGEPAIRSDIALOG_H
