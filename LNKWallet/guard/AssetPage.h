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

private:
    Ui::AssetPage *ui;

    void paintEvent(QPaintEvent*);
};

#endif // ASSETPAGE_H
