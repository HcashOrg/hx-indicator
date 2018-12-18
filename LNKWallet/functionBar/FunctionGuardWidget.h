#ifndef FUNCTIONGUARDWIDGET_H
#define FUNCTIONGUARDWIDGET_H

#include <QWidget>

namespace Ui {
class FunctionGuardWidget;
}

class FunctionGuardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionGuardWidget(QWidget *parent = 0);
    ~FunctionGuardWidget();
    void retranslator();
public slots:
    void DefaultShow();
private:
    void InitWidget();
    void InitStyle();

signals:
    void showGuardAccountSignal();
    void showGuardIncomeSignal();
    void showLockContractSignal();
    void showAssetSignal();
    void showKeyManageSignal();
    void showProposalSignal();
    void showWithdrawConfirmSignal();
    void showFeedPriceSignal();
    void showColdHotTransferSignal();

private slots:
    void on_accountInfoBtn_clicked();

    void on_myIncomeBtn_clicked();

    void on_lockContractBtn_clicked();

    void on_assetInfoBtn_clicked();

    void on_keyManageBtn_clicked();

    void on_proposalBtn_clicked();

    void on_withdrawConfirmBtn_clicked();

    void on_feedPriceBtn_clicked();

    void on_coldHotTransferBtn_clicked();


private:
    Ui::FunctionGuardWidget *ui;

    void paintEvent(QPaintEvent*);
};

#endif // FUNCTIONGUARDWIDGET_H
