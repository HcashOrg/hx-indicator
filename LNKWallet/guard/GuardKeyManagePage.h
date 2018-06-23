#ifndef GUARDKEYMANAGE_H
#define GUARDKEYMANAGE_H

#include <QWidget>

namespace Ui {
class GuardKeyManagePage;
}

class GuardKeyManagePage : public QWidget
{
    Q_OBJECT

public:
    explicit GuardKeyManagePage(QWidget *parent = 0);
    ~GuardKeyManagePage();

    void init();

    void showMultisigInfo();

private slots:
    void jsonDataUpdated(QString id);

    void on_multisigTableWidget_cellClicked(int row, int column);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::GuardKeyManagePage *ui;

    void paintEvent(QPaintEvent*);
};

#endif // GUARDKEYMANAGE_H
