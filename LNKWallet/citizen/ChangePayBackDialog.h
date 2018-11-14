#ifndef CHANGEPAYBACKDIALOG_H
#define CHANGEPAYBACKDIALOG_H

#include <QDialog>
class QLineEdit;
namespace Ui {
class ChangePayBackDialog;
}

class ChangePayBackDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePayBackDialog(const QString &citizenName,QWidget *parent = 0);
    ~ChangePayBackDialog();
private slots:
    void ChangePaybackSlots();//点击确定按钮，发起更换senator提案
    void jsonDataUpdated(QString id);
private:
    void InitWidget();
    void InitData();
    void installDoubleValidator(QLineEdit *line,int mi,int ma);
private:
    Ui::ChangePayBackDialog *ui;
    QString citizenName;
};

#endif // CHANGEPAYBACKDIALOG_H
