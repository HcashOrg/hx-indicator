#ifndef SmartContractPage_H
#define SmartContractPage_H

#include <QWidget>

#include "contract//singlecontractwidget.h"
#include "contractdetailwidget.h"

namespace Ui {
class SmartContractPage;
}

class SmartContractPage : public QWidget
{
    Q_OBJECT

public:
    explicit SmartContractPage(QWidget *parent = 0);
    ~SmartContractPage();

    void showContracts(QVector<SingleContractWidget*> vector);

public slots:
    void refresh();

private slots:
    void on_hotBtn_clicked();

    void on_normalBtn_clicked();

    void on_mortgageBtn_clicked();

    void on_myBtn_clicked();

    void showDetailWidget(QString contractAddress);
    void hideDetailWidget();

    void on_addContractBtn_clicked();

    void on_createContractBtn_clicked();

private:
    Ui::SmartContractPage *ui;

    void paintEvent(QPaintEvent*);

    QVector<SingleContractWidget*> contractVector;
    void releaseVector();
    void updateContractVector();
    SingleContractWidget* getWidgetFromVector(QString _contractAddress);

    ContractDetailWidget* detailWidget;
};

#endif // SmartContractPage_H
