#ifndef ADDOWNERDIALOG_H
#define ADDOWNERDIALOG_H

#include <QDialog>

namespace Ui {
class AddOwnerDialog;
}

class AddOwnerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOwnerDialog(QWidget *parent = 0);
    ~AddOwnerDialog();

    void  pop();

    QString ownerAddress;

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_addressLineEdit_textEdited(const QString &arg1);

    void on_closeBtn_clicked();

private:
    Ui::AddOwnerDialog *ui;
};

#endif // ADDOWNERDIALOG_H
