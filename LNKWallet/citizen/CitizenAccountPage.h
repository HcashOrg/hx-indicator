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

    void refresh();

private slots:
    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_newCitizenBtn_clicked();

private:
    Ui::CitizenAccountPage *ui;
    bool inited = false;

    void paintEvent(QPaintEvent*);
    void showLockBalance();
};

#endif // CITIZENACCOUNTPAGE_H
