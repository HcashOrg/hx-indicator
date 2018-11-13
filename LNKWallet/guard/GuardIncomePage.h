#ifndef GUARDINCOMEPAGE_H
#define GUARDINCOMEPAGE_H

#include <QWidget>

#include "wallet.h"
#include "poundage/PageScrollWidget.h"
#include "control/BlankDefaultWidget.h"

namespace Ui {
class GuardIncomePage;
}

class GuardIncomePage : public QWidget
{
    Q_OBJECT

public:
    explicit GuardIncomePage(QWidget *parent = 0);
    ~GuardIncomePage();

    void init();

    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_senatorIncomeTableWidget_cellClicked(int row, int column);

    void pageChangeSlot(unsigned int page);

    void on_obtainAllBtn_clicked();

private:
    Ui::GuardIncomePage *ui;
    PageScrollWidget *pageWidget = NULL;
    BlankDefaultWidget *blankWidget = NULL;

    unsigned long long totalAmount = 0;
    QMap<QString,unsigned long long> incomeFromCitizenMap;
    void fetchSenatorIncomes();
    void showIncomes();

    void paintEvent(QPaintEvent*);
};

#endif // GUARDINCOMEPAGE_H
