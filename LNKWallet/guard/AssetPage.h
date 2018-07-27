#ifndef ASSETPAGE_H
#define ASSETPAGE_H

#include <QWidget>

#include "poundage/PageScrollWidget.h"
#include "control/BlankDefaultWidget.h"
namespace Ui {
class AssetPage;
}

class AssetPage : public QWidget
{
    Q_OBJECT

public:
    explicit AssetPage(QWidget *parent = 0);
    ~AssetPage();

    void showAssetsInfo();


private slots:
    void on_assetTableWidget_cellPressed(int row, int column);

    void pageChangeSlot(unsigned int page);
private:
    Ui::AssetPage *ui;
    void paintEvent(QPaintEvent*);
    PageScrollWidget *pageWidget;
    BlankDefaultWidget *blankWidget;

};

#endif // ASSETPAGE_H
