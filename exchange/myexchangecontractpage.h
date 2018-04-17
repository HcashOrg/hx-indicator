#ifndef MYEXCHANGECONTRACTPAGE_H
#define MYEXCHANGECONTRACTPAGE_H

#include <QWidget>

namespace Ui {
class MyExchangeContractPage;
}

class MyExchangeContractPage : public QWidget
{
    Q_OBJECT

public:
    explicit MyExchangeContractPage(QWidget *parent = 0);
    ~MyExchangeContractPage();

    void init();

private slots:
    void jsonDataUpdated(QString id);

    void on_createBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::MyExchangeContractPage *ui;
    bool inited = false;

    void paintEvent(QPaintEvent*);
};

#endif // MYEXCHANGECONTRACTPAGE_H
