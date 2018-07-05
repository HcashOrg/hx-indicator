#ifndef COLDHOTINFODIALOG_H
#define COLDHOTINFODIALOG_H

#include <QDialog>

namespace Ui {
class ColdHotInfoDialog;
}

class ColdHotTransferPage;

class ColdHotInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColdHotInfoDialog( ColdHotTransferPage* _page, QWidget *parent = 0);
    ~ColdHotInfoDialog();

    void pop();

    void setTrxId(QString _trxId);

private slots:
    void on_closeBtn_clicked();

private:
    Ui::ColdHotInfoDialog *ui;
    ColdHotTransferPage* page = NULL;
};

#endif // COLDHOTINFODIALOG_H
