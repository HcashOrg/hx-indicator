#ifndef ERRORRESULTDIALOG_H
#define ERRORRESULTDIALOG_H

#include <QDialog>

namespace Ui {
class ErrorResultDialog;
}

class ErrorResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorResultDialog(QWidget *parent = 0);
    ~ErrorResultDialog();

private:
    Ui::ErrorResultDialog *ui;
};

#endif // ERRORRESULTDIALOG_H
