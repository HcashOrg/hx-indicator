#ifndef CONTRACTBALANCEWIDGET_H
#define CONTRACTBALANCEWIDGET_H

#include <QWidget>

namespace Ui {
class ContractBalanceWidget;
}
class PageScrollWidget;
class BlankDefaultWidget;
class ContractBalanceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContractBalanceWidget(QWidget *parent = 0);
    ~ContractBalanceWidget();

    void init();

    void setAccount(QString _accountName);

    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_depositBtn_clicked();

    void onItemClicked(int _row, int _column);

    void on_openForUsersBtn_clicked();

    void pageChangeSlot(unsigned int page);
private:
    Ui::ContractBalanceWidget *ui;
    QString accountName;

    void paintEvent(QPaintEvent*);
    void showContractBalances();

    BlankDefaultWidget *blankWidget;
    PageScrollWidget *pageWidget;
};

#endif // CONTRACTBALANCEWIDGET_H
