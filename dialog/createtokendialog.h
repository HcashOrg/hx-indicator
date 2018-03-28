#ifndef ISSUETOKENDIALOG_H
#define ISSUETOKENDIALOG_H

#include <QDialog>
#include <QTimer>

#include "extra/guiutil.h"

namespace Ui {
class CreateTokenDialog;
}

class CreateTokenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateTokenDialog(QWidget *parent = 0);
    ~CreateTokenDialog();

    void  pop();

private:
    GUIUtil::ClickableLabel* noMortgageLabel;
    GUIUtil::ClickableLabel* mortgageLabel;
    int currentChoice = 0;  // 0: no-mortgage   1: mortgage
private slots:
    void onNoMortgageLabelSelected();
    void onMortgageLabelSelected();

private slots:
    void jsonDataUpdated( QString id);

    void on_backBtn_clicked();

    void on_backBtn2_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_accountComboBox2_currentIndexChanged(const QString &arg1);

    void on_issueBtn_clicked();

    void on_issueBtn2_clicked();

    void onTimerForRegister();

    void onTimerForUpgrade();

    void onTimerForInitToken();


    void on_precisionSpinBox_valueChanged(const QString &arg1);

    void on_precisionSpinBox2_valueChanged(const QString &arg1);

    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_okBtn2_clicked();

private:
    Ui::CreateTokenDialog *ui;
    QString contractAddress;
    int contractType;    // 1:普通合约  2:锚定合约

    QTimer* timerForRegister;
    QTimer* timerForUpgrade;
    QTimer* timerForInitToken;
    int period;     // 标识创建合约的阶段  1:已register  2:已upgrade  3：已init_token

    void init();

    void estimateGas();
    QString intToPrecisionString(int precision);    //将 0-8 转换成1,10,... 100000000等格式
    QString addPrecisionString(QString supply, int precision);     //  比如 20000,2  转换成 2000000
    QString getContractParamsString(QStringList params);    // 合约参数拼起来  用","分隔


    void setTotalSupplyValidator();

};

#endif // ISSUETOKENDIALOG_H
