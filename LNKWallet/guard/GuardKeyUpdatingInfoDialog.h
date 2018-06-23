#ifndef GUARDKEYUPDATINGINFODIALOG_H
#define GUARDKEYUPDATINGINFODIALOG_H

#include <QDialog>

namespace Ui {
class GuardKeyUpdatingInfoDialog;
}

class GuardKeyUpdatingInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GuardKeyUpdatingInfoDialog(QWidget *parent = 0);
    ~GuardKeyUpdatingInfoDialog();

    void pop();

    void setAsset(QString _assetSymbol);

private slots:
    void on_closeBtn_clicked();

private:
    Ui::GuardKeyUpdatingInfoDialog *ui;
};

#endif // GUARDKEYUPDATINGINFODIALOG_H
