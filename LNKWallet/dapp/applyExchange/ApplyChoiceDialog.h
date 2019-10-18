#ifndef APPLYCHOICEDIALOG_H
#define APPLYCHOICEDIALOG_H

#include <QDialog>

namespace Ui {
class ApplyChoiceDialog;
}

class ApplyChoiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyChoiceDialog(QWidget *parent = nullptr);
    ~ApplyChoiceDialog();

    int pop();      // 返回 0取消  1去转账  2去申请

private slots:
    void on_payBtn_clicked();

    void on_submitBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::ApplyChoiceDialog *ui;
    int choice = 0;
};

#endif // APPLYCHOICEDIALOG_H
