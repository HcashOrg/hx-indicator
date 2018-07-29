#ifndef FORECLOSEDIALOG_H
#define FORECLOSEDIALOG_H

#include <QDialog>

namespace Ui {
class ForecloseDialog;
}

class ForecloseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ForecloseDialog(const QString &accountName, QString _assetSymbol, QString _maxAmount,QWidget *parent = 0);
    ~ForecloseDialog();

    bool pop();
    QString amountStr;

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void on_amountLineEdit_textEdited(const QString &arg1);

private:
    Ui::ForecloseDialog *ui;
    bool yesOrNo = false;
    QString assetSymbol;
    QString maxAmount;

};

#endif // FORECLOSEDIALOG_H
