#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = 0);
    ~RegisterDialog();

    void  pop();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void on_closeBtn_clicked();

    void on_okBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_okBtn2_clicked();

    void on_cancelBtn_clicked();

    void on_registerNameLineEdit_textChanged(const QString &arg1);

    void on_cancelBtn2_clicked();

protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
