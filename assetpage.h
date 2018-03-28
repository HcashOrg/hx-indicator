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

    void updateAssetInfo();
    void updateMyAsset();

private slots:
    void jsonDataUpdated(QString id);

    void on_addTokenBtn_clicked();

    void on_erc20TokenBtn_clicked();

    void on_smartTokenBtn_clicked();

    void on_smartTokenTableWidget_cellPressed(int row, int column);

    void on_erc20TokenTableWidget_cellPressed(int row, int column);

    void on_issueTokenBtn_clicked();

private:
    Ui::AssetPage *ui;

    void paintEvent(QPaintEvent*);


};

#endif // ASSETPAGE_H
