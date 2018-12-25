#ifndef TOKENTRANSFERWIDGET_H
#define TOKENTRANSFERWIDGET_H

#include <QWidget>

#include "ContractTokenPage.h"

namespace Ui {
class TokenTransferWidget;
}

class FeeChooseWidget;

class TokenTransferWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TokenTransferWidget(QString _accountName, QString _tokenSymbol, ContractTokenPage *parent);
    ~TokenTransferWidget();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void on_sendBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_tokenComboBox_currentIndexChanged(const QString &arg1);

    void on_toolButton_chooseContact_clicked();

    void selectContactSlots(const QString &name,const QString &address);

    void on_memoTextEdit_textChanged();

    void on_sendtoLineEdit_textEdited(const QString &arg1);

    void on_amountLineEdit_textChanged(const QString &arg1);

    void setTokenBalance();

private:
    Ui::TokenTransferWidget *ui;
    bool inited = false;
    ContractTokenPage* page = NULL;
    int stepCount = 0;
    FeeChooseWidget* transferFeeWidget = NULL;

    void calculateTransferFee();
};

#endif // TOKENTRANSFERWIDGET_H
