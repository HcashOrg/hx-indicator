#ifndef CHANGESENATORDIALOG_H
#define CHANGESENATORDIALOG_H

#include <QDialog>

namespace Ui {
class ChangeSenatorDialog;
}

class ChangeSenatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeSenatorDialog(QWidget *parent = 0);
    ~ChangeSenatorDialog();
private slots:
    void ChangeSenatorSlots();//点击确定按钮，发起更换senator提案
    void jsonDataUpdated(QString id);
private:
    void InitWidget();
    void InitData();
private:
    Ui::ChangeSenatorDialog *ui;
};

#endif // CHANGESENATORDIALOG_H
