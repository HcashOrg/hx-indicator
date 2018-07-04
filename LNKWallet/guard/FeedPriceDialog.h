#ifndef FEEDPRICEDIALOG_H
#define FEEDPRICEDIALOG_H

#include <QDialog>

namespace Ui {
class FeedPriceDialog;
}

class FeedPriceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FeedPriceDialog( QString _assetSymbol, QWidget *parent = 0);
    ~FeedPriceDialog();

    void pop();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_quoteLineEdit_textEdited(const QString &arg1);

private:
    Ui::FeedPriceDialog *ui;
};

#endif // FEEDPRICEDIALOG_H
