#ifndef ADDTOKENDIALOG_H
#define ADDTOKENDIALOG_H

#include <QDialog>

namespace Ui {
class AddTokenDialog;
}

class AddTokenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTokenDialog(QWidget *parent = 0);
    ~AddTokenDialog();

    void pop();

private slots:
    void jsonDataUpdated(QString id);

    void on_closeBtn_clicked();

    void on_okBtn_clicked();

private:
    Ui::AddTokenDialog *ui;
};

#endif // ADDTOKENDIALOG_H
