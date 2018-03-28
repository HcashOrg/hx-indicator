#ifndef MINTDIALOG_H
#define MINTDIALOG_H

#include <QDialog>

namespace Ui {
class MintDialog;
}

class MintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MintDialog( QString address, QWidget *parent = 0);
    ~MintDialog();

    void  pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_mintLineEdit_textEdited(const QString &arg1);

    void on_closeBtn_clicked();

private:
    Ui::MintDialog *ui;
    QString contractAddress;

    void init();
};

#endif // MINTDIALOG_H
