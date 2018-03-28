#ifndef FUNCTIONBARCHOICEDIALOG_H
#define FUNCTIONBARCHOICEDIALOG_H

#include <QDialog>

namespace Ui {
class FunctionBarChoiceDialog;
}

class FunctionBarChoiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FunctionBarChoiceDialog(QWidget *parent = 0);
    ~FunctionBarChoiceDialog();

    int choice;         // 返回选择的值  0:取消  1:lock  2:console  3:setting

private slots:
    void on_lockBtn_clicked();

    void on_consoleBtn_clicked();

    void on_setBtn_clicked();

private:
    Ui::FunctionBarChoiceDialog *ui;
};

#endif // FUNCTIONBARCHOICEDIALOG_H
