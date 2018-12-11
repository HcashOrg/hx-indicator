#ifndef BONUSPAGE_H
#define BONUSPAGE_H

#include <QWidget>

#include "poundage/PageScrollWidget.h"
#include "control/BlankDefaultWidget.h"
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

    void pageChangeSlot(unsigned int page);

private:
    Ui::BonusPage *ui;
    bool inited = false;

    void paintEvent(QPaintEvent*);

    void fetchBonusBalance();

    void checkObtainAllBtnVisible();
    PageScrollWidget *pageWidget;
    BlankDefaultWidget *blankWidget;
};

#endif // BONUSPAGE_H
