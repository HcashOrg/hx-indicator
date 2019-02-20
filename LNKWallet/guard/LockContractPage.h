#ifndef LOCKCONTRACTPAGE_H
#define LOCKCONTRACTPAGE_H

#include <QWidget>

namespace Ui {
class LockContractPage;
}

class LockContractPage : public QWidget
{
    Q_OBJECT

public:
    explicit LockContractPage(QWidget *parent = nullptr);
    ~LockContractPage();

    void init();

    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_lockFundTableWidget_cellClicked(int row, int column);

    void on_lockBtn_clicked();

private:
    Ui::LockContractPage *ui;

    void paintEvent(QPaintEvent*);

    void getUserLockInfo(QString accountName);
    void getUsers();
};

#endif // LOCKCONTRACTPAGE_H
