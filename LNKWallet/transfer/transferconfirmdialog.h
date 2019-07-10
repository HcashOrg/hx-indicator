#ifndef TRANSFERCONFIRMDIALOG_H
#define TRANSFERCONFIRMDIALOG_H

#include <QDialog>

namespace Ui {
class TransferConfirmDialog;
}

class TransferConfirmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferConfirmDialog(QString address, QString amount, QString assetSymbol,QString fee, QString feeSymbol, QString remark, QWidget *parent = 0);
    ~TransferConfirmDialog();

    bool pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pwdLineEdit_textChanged(const QString &arg1);

    void on_pwdLineEdit_returnPressed();

    void on_pwdLineEdit_textEdited(const QString &arg1);

    void on_closeBtn_clicked();

protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::TransferConfirmDialog *ui;
    QString address;
    QString amount;
    QString assetType;
    QString fee;
    QString feeType;
    QString remark;
    bool yesOrNo;

};

#endif // TRANSFERCONFIRMDIALOG_H
