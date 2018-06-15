#ifndef ASSETPAGE_H
#define ASSETPAGE_H

#include <QWidget>

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

signals:
    void backBtnVisible(bool isShow);

private slots:
    void on_assetTableWidget_cellPressed(int row, int column);

    void on_assetTableWidget_cellClicked(int row, int column);

private:
    Ui::AssetPage *ui;
    QWidget* currentWidget;

    void paintEvent(QPaintEvent*);
};

#endif // ASSETPAGE_H
