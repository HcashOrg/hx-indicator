#ifndef TOTALLOCKEDDIALOG_H
#define TOTALLOCKEDDIALOG_H

#include <QDialog>

namespace Ui {
class TotalLockedDialog;
}

class TotalLockedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TotalLockedDialog(QWidget *parent = nullptr);
    ~TotalLockedDialog();

private:
    Ui::TotalLockedDialog *ui;

    void calculateTotalLocked();
};

#endif // TOTALLOCKEDDIALOG_H
