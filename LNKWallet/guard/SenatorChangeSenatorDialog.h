#ifndef SENATORCHANGESENATORDIALOG_H
#define SENATORCHANGESENATORDIALOG_H

#include <QDialog>
class QLineEdit;
namespace Ui {
class SenatorChangeSenatorDialog;
}

class SenatorChangeSenatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SenatorChangeSenatorDialog(QWidget *parent = 0);
    ~SenatorChangeSenatorDialog();
private slots:
    void ChangeSenatorSlots();//点击确定按钮，发起更换senator提案
    void jsonDataUpdated(QString id);
private:
    void InitWidget();
    void InitData();
    void installDoubleValidator(QLineEdit *line,double mi,double ma,int pre);

private:
    Ui::SenatorChangeSenatorDialog *ui;
};

#endif // SENATORCHANGESENATORDIALOG_H
