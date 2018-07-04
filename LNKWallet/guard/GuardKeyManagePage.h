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

    void refresh();

signals:
    void backBtnVisible(bool isShow);

private slots:
    void jsonDataUpdated(QString id);

    void on_multisigTableWidget_cellClicked(int row, int column);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_multisigTableWidget_cellPressed(int row, int column);

    void on_historyBtn_clicked();

    void on_importBtn_clicked();

    void on_changeAddressBtn_clicked();

private:
    Ui::GuardKeyManagePage *ui;
    bool inited = false;

    void paintEvent(QPaintEvent*);
};

#endif // GUARDKEYMANAGE_H
