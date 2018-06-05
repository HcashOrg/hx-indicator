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

    void setInfoText(QString _text);
    void setDetailText(QString _text);

    void pop();

private slots:
    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_copyBtn_clicked();
    
private:
    Ui::ErrorResultDialog *ui;
};

#endif // ERRORRESULTDIALOG_H
