#ifndef BONUSPAGE_H
#define BONUSPAGE_H

#include <QWidget>

namespace Ui {
class BonusPage;
}

class BonusPage : public QWidget
{
    Q_OBJECT

public:
    explicit BonusPage(QWidget *parent = 0);
    ~BonusPage();

    void init();

    void refresh();
private slots:
    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_bonusTableWidget_cellPressed(int row, int column);

    void on_obtainAllBtn_clicked();

private:
    Ui::BonusPage *ui;

    void paintEvent(QPaintEvent*);

    void fetchBonusBalance();

    void checkObtainAllBtnVisible();
};

#endif // BONUSPAGE_H
