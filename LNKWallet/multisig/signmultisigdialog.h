#ifndef SIGNMULTISIGDIALOG_H
#define SIGNMULTISIGDIALOG_H

#include <QDialog>

namespace Ui {
class SignMultiSigDialog;
}

class SignMultiSigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignMultiSigDialog(QWidget *parent = 0);
    ~SignMultiSigDialog();

    void  pop();

private slots:
    void jsonDataUpdated(QString id);

    void on_nextBtn_clicked();

    void on_cancelBtn_clicked();

    void on_fileCheckBox_clicked();

    void on_bullderCheckBox_clicked();

    void on_selectPathBtn_clicked();

    void on_backBtn_clicked();

    void on_closeBtn_clicked();

    void on_signBtn_clicked();

    void on_closeBtn2_clicked();

    void on_copyBtn_clicked();

    void on_signInfoTableWidget_cellPressed(int row, int column);

    void on_closeBtn_2_clicked();

    void on_closeBtn3_clicked();

private:
    Ui::SignMultiSigDialog *ui;

    bool useFile;
    int localUnsignedCount;         // 还未签名的本地账户数量

    void init();
};

#endif // SIGNMULTISIGDIALOG_H
