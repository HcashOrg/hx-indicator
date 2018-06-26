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
    explicit FeedPriceDialog(QWidget *parent = 0);
    ~FeedPriceDialog();

    void pop();

    void setAsset(QString _assetSymbol);

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::FeedPriceDialog *ui;
};

#endif // FEEDPRICEDIALOG_H
