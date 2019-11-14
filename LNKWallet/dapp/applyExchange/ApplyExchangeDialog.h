#ifndef APPLYEXCHANGEDIALOG_H
#define APPLYEXCHANGEDIALOG_H

#include <QDialog>

namespace Ui {
class ApplyExchangeDialog;
}

class ContractTokenPage;
class ApplyExchangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyExchangeDialog(QWidget *parent = nullptr);
    ~ApplyExchangeDialog();

    void pop();

    void init();

    void setParentPage(ContractTokenPage* p);
    ContractTokenPage* page = nullptr;


private slots:
    void on_closeBtn_clicked();

    void on_cancelBtn_clicked();

    void on_okBtn_clicked();


    void on_contractComboBox_currentIndexChanged(const QString &arg1);


    void on_copyBtn_clicked();

    void on_hxCheckBox_stateChanged(int arg1);

    void on_paxCheckBox_stateChanged(int arg1);

private:
    Ui::ApplyExchangeDialog *ui;
    bool inited = false;

    void calculateMemo();
};

#endif // APPLYEXCHANGEDIALOG_H
