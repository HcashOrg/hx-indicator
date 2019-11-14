#ifndef APPLYEXCHANGEINFODIALOG_H
#define APPLYEXCHANGEINFODIALOG_H

#include <QDialog>
#include "extra/HttpManager.h"

namespace Ui {
class ApplyExchangeInfoDialog;
}

class ContractTokenPage;
class ApplyExchangeInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyExchangeInfoDialog(QWidget *parent = nullptr);
    ~ApplyExchangeInfoDialog();

    void pop();

    void init();

    void setParentPage(ContractTokenPage* p);
    ContractTokenPage* page = nullptr;

private slots:
    void httpReplied(QByteArray _data, int _status);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::ApplyExchangeInfoDialog *ui;
    HttpManager httpManager;
};
#endif // APPLYEXCHANGEINFODIALOG_H
