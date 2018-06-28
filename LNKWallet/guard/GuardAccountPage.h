#ifndef GUARDACCOUNTPAGE_H
#define GUARDACCOUNTPAGE_H

#include <QWidget>

namespace Ui {
class GuardAccountPage;
}

class GuardAccountPage : public QWidget
{
    Q_OBJECT

public:
    explicit GuardAccountPage(QWidget *parent = 0);
    ~GuardAccountPage();

    void init();

private slots:
    void on_accountComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::GuardAccountPage *ui;

    void paintEvent(QPaintEvent*);
};

#endif // GUARDACCOUNTPAGE_H
