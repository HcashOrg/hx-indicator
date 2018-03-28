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
    void addTokens();
    void setCurrentToken(QString token);
    void addAccounts();

private slots:
    void on_feedBtn_clicked();

    void on_cancelBtn_clicked();

    void jsonDataUpdated( QString id);

    void on_tokenComboBox_currentIndexChanged(const QString &arg1);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_closeBtn_clicked();

private:
    Ui::FeedPriceDialog *ui;
};

#endif // FEEDPRICEDIALOG_H
