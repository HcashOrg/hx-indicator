#ifndef ALLTRANSACTIONWIDGET_H
#define ALLTRANSACTIONWIDGET_H

#include <QWidget>

namespace Ui {
class AllTransactionWidget;
}

class PageScrollWidget;
class BlankDefaultWidget;
class AllTransactionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AllTransactionWidget(QWidget *parent = 0);
    ~AllTransactionWidget();

    enum TypeChoice{AllType, TransferType, DepositType, WithdrawType, ContractType, MineType, GuaranteeType,
                    OtherType, FeedPriceType, ProposalType, BonusType};
    enum TimeChoice{AllTime, DayTime, WeekTime, MonthTime};
private slots:
    void on_typeAllBtn_clicked();
    void on_typeTransferBtn_clicked();
    void on_typeDepositBtn_clicked();
    void on_typeWithdrawBtn_clicked();
    void on_typeContractBtn_clicked();
    void on_typeMineBtn_clicked();
    void on_typeGuaranteeBtn_clicked();
    void on_typeOtherBtn_clicked();
    void on_typeFeedPriceBtn_clicked();
    void on_typeProposalBtn_clicked();
    void on_typeBonusBtn_clicked();

    void on_timeAllBtn_clicked();
    void on_timeDayBtn_clicked();
    void on_timeWeekBtn_clicked();
    void on_timeMonthBtn_clicked();


    void on_accountComboBox_currentTextChanged(const QString &arg1);

    void on_transactionsTableWidget_cellPressed(int row, int column);

    void on_searchBtn_clicked();

    void on_searchLineEdit_textChanged(const QString &arg1);

    void pageChangeSlot(unsigned int page);



    void on_clearDBBtn_clicked();

private:
    Ui::AllTransactionWidget *ui;

    void paintEvent(QPaintEvent*);

    bool inited = false;
    TypeChoice typeChoice = AllType;
    TimeChoice timeChoice = AllTime;
    void clearTypeChoice();
    void clearTimeChoice();

    void hideFilteredTransactions();

    void showTransactions();

    void init();

    double calculateGuaranteeOrderAmount(QString _guaranteeId, double _feeAmount = 0);

    PageScrollWidget *pageWidget;
    BlankDefaultWidget *blankWidget;
    std::vector<int> showRows;
};

#endif // ALLTRANSACTIONWIDGET_H
