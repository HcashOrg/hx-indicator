#ifndef CONTACTEDITDIALOG_H
#define CONTACTEDITDIALOG_H

#include <QDialog>

namespace Ui {
class ContactEditDialog;
}

class ContactEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactEditDialog(QWidget *parent = 0);
    ~ContactEditDialog();

    int choice = 0;  // 1: edit   2: delete

private slots:
    void on_editBtn_clicked();

    void on_deleteBtn_clicked();

private:
    Ui::ContactEditDialog *ui;
};

#endif // CONTACTEDITDIALOG_H
