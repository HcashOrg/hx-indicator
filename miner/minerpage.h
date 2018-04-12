#ifndef MINERPAGE_H
#define MINERPAGE_H

#include <QWidget>

namespace Ui {
class MinerPage;
}

class MinerPage : public QWidget
{
    Q_OBJECT

public:
    explicit MinerPage(QWidget *parent = 0);
    ~MinerPage();

    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_registerBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_lockToMinerBtn_clicked();

    void on_lockBalancesTableWidget_cellPressed(int row, int column);

private:
    Ui::MinerPage *ui;

    void paintEvent(QPaintEvent*);
    void init();

    void getLockBalance();

};

#endif // MINERPAGE_H
