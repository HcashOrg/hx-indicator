#ifndef FUNCTIONBAR_H
#define FUNCTIONBAR_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class FunctionBar;
}

class FunctionBar : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionBar(QWidget *parent = 0);
    ~FunctionBar();

    int currentPageIndex = 0;     // 1 账户列表  2 转账  3 联系人  4 账单页  5 智能合约  6 喂价页  7 多重签名
    void choosePage(int);

    void retranslator();

private:
    QTimer* timerForScanLabel;
    int scanLabelCount;


public slots:
    void refresh();

signals:
    void showMainPage();
    void showAccountPage();
    void showSmartContractPage();
    void showTransferPage();
    void showContactPage();
    void showFeedPage();
    void showMultiSigPage();

    void lock();

private slots:
    void on_accountBtn_clicked();

    void on_transferBtn_clicked();

    void on_contactBtn_clicked();

    void on_smartContractBtn_clicked();

    void on_feedBtn_clicked();

    void on_multiSigBtn_clicked();

    void on_advanceBtn_clicked();

    void on_accountListBtn_clicked();

    void on_billInfoBtn_clicked();

    void on_choiceBtn_clicked();

private:
    Ui::FunctionBar *ui;

    void paintEvent(QPaintEvent*);
};

#endif // FUNCTIONBAR_H
