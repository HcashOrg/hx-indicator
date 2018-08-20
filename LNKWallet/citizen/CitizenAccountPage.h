#ifndef CITIZENACCOUNTPAGE_H
#define CITIZENACCOUNTPAGE_H

#include <QWidget>

namespace Ui {
class CitizenAccountPage;
}

class CitizenAccountPage : public QWidget
{
    Q_OBJECT

public:
    explicit CitizenAccountPage(QWidget *parent = 0);
    ~CitizenAccountPage();

    void init();

private slots:
    void on_accountComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::CitizenAccountPage *ui;

    void paintEvent(QPaintEvent*);
    void showLockBalance();
};

#endif // CITIZENACCOUNTPAGE_H
