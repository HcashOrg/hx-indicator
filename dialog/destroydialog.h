#ifndef DESTROYDIALOG_H
#define DESTROYDIALOG_H

#include <QDialog>

namespace Ui {
class DestroyDialog;
}

class DestroyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DestroyDialog( QString address, QWidget *parent = 0);
    ~DestroyDialog();

    void  pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_amountLineEdit_textEdited(const QString &arg1);

    void on_closeBtn_clicked();

private:
    Ui::DestroyDialog *ui;
    QString contractAddress;

    void init();
};

#endif // DESTROYDIALOG_H
