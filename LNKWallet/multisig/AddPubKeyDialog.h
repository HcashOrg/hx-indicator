#ifndef ADDPUBKEYDIALOG_H
#define ADDPUBKEYDIALOG_H

#include <QDialog>

namespace Ui {
class AddPubKeyDialog;
}

class AddPubKeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPubKeyDialog(QWidget *parent = 0);
    ~AddPubKeyDialog();

    void pop();
    QString pubKey;

private slots:
    void on_closeBtn_clicked();

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pubKeyLineEdit_textEdited(const QString &arg1);

private:
    Ui::AddPubKeyDialog *ui;
};

#endif // ADDPUBKEYDIALOG_H
